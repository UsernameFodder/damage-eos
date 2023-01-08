// Command-line application for EoS damage calculation

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include <nlohmann/json.hpp>
using nlohmann::json;

#include "cfgparse.hpp"
#include "damage.hpp"
#include "idmap.hpp"

std::string monster_summary(const Monster& monster);

int main(int argc, char** argv) {
    CLI::App app{"Damage calculator for Pokémon Mystery Dungeon: Explorers of Sky"};

    std::string filename = "config.json";
    int verbose = 0;
    app.add_option("-i, --input-file", filename, "Input config file");
    app.add_flag("-v, --verbose", verbose, "Verbose output, can be specified up to 3 times");
    CLI11_PARSE(app, argc, argv);

    std::ifstream cfg_file(filename);
    if (cfg_file.fail()) {
        std::cerr << "error: could not find config file '" << filename << "'" << std::endl;
        std::cerr << "Run `" << argv[0] << " --help` for usage instructions." << std::endl;
        return 1;
    }

    json cfg = json::parse(cfg_file);

    try {
        auto [dungeon, attacker, defender, move, attack_power] = parse_cfg(cfg);
        auto move_spec = mechanics::MoveSpec(move.id);
        if (move_spec.unsupported) {
            std::cerr << "warning: move '" << ids::MOVE[move.id]
                      << "' is unsupported; results may not make sense" << std::endl;
            if (!verbose) {
                // Only need to do this in non-verbose mode, since we print the notes already in
                // verbose mode
                std::cerr << "move notes: " << move_spec.special_notes << std::endl;
            }
        }
        if (!attacker.monster.statuses.is_valid()) {
            std::cerr << "warning: attacker status configuration is impossible" << std::endl;
        }
        if (!defender.monster.statuses.is_valid()) {
            std::cerr << "warning: defender status configuration is impossible" << std::endl;
        }

        // Copy for a second damage calc since the inputs can be modified by the simulation
        DungeonState dungeon_max = dungeon;
        MonsterEntity attacker_max = attacker;
        MonsterEntity defender_max = defender;
        Move move_max = move;

        dungeon.rng.variance_dial = 0;     // minimum damage roll
        dungeon_max.rng.variance_dial = 1; // maximum damage roll

        DamageData details = {};
        DamageData details_max_var = {};
        int32_t damage = 0;
        int32_t damage_max_var = 0;
        if (move.id == eos::MOVE_PROJECTILE) {
            damage =
                simulate_damage_calc_projectile(details, dungeon, attacker, defender, attack_power);
            damage_max_var = simulate_damage_calc_projectile(
                details_max_var, dungeon_max, attacker_max, defender_max, attack_power);
        } else {
            damage = simulate_damage_calc(details, dungeon, attacker, defender, move);
            damage_max_var = simulate_damage_calc(details_max_var, dungeon_max, attacker_max,
                                                  defender_max, move_max);
        }

        std::cout << std::boolalpha;
        if (details.healed) {
            std::cout << "healed: [" << details.damage << ", " << details_max_var.damage << "]"
                      << std::endl;
        } else {
            std::cout << "damage: [" << damage << ", " << damage_max_var << "]" << std::endl;
        }
        if (dungeon.damage_calc.two_turn_move_forced_miss ||
            dungeon.damage_calc.soundproof_activated ||
            dungeon.damage_calc.first_hit_check_failed || dungeon.damage_calc.dream_eater_failed ||
            dungeon.damage_calc.last_resort_failed) {
            std::cout << "hit chance: guaranteed miss" << std::endl;
            return 0;
        }
        std::cout << "hit chance: " << 100 * dungeon.rng.get_combined_hit_probability() << "%"
                  << std::endl;
        std::cout << "crit chance: " << dungeon.rng.get_computed_crit_chance() << "%" << std::endl;

        const auto& calc = dungeon.damage_calc;
        if (verbose >= 1) {
            // Don't print everything, but just a couple things to serve as a reminder
            std::cout << std::endl;
            std::cout << "== input summary ==" << std::endl;
            std::cout << "move: " << ids::MOVE[move.id];
            if (move.ginseng > 0) {
                std::cout << "+" << static_cast<int>(move.ginseng);
            }
            if (move.prior_successive_hits > 0) {
                std::cout << " (hit " << (static_cast<int>(move.prior_successive_hits) + 1) << ")";
            }
            std::cout << std::endl;
            if (move_spec.special_notes != "") {
                std::cout << "move notes: " << move_spec.special_notes << std::endl;
            }
            std::cout << "attacker: " << monster_summary(attacker.monster) << std::endl;
            std::cout << "defender: " << monster_summary(defender.monster) << std::endl;
            std::cout << "dungeon weather: " << ids::WEATHER[dungeon.weather];
            if (dungeon.mud_sport_turns > 0) {
                std::cout << ", mud sport";
            }
            if (dungeon.water_sport_turns > 0) {
                std::cout << ", water sport";
            }
            std::cout << std::endl;

            std::cout << std::endl;
            std::cout << "== result details ==" << std::endl;
            std::cout << "damage message: " << ids::DAMAGE_MESSAGE[details.damage_message]
                      << std::endl;
            std::cout << "type matchup: " << ids::TYPE_MATCHUP[details.type_matchup] << " ("
                      << ids::TYPE_MATCHUP[calc.move_indiv_type_matchups[0]] << " + "
                      << ids::TYPE_MATCHUP[calc.move_indiv_type_matchups[1]] << ")" << std::endl;
            std::cout << "move type: " << ids::TYPE[details.type] << std::endl;
            std::cout << "move category: " << ids::MOVE_CATEGORY[details.category] << std::endl;
            std::cout << "critical hit: " << details.critical_hit << std::endl;
            std::cout << "full type immunity: " << details.full_type_immunity << std::endl;
            std::cout << "no damage: " << details.no_damage << std::endl;
        }
        if (verbose >= 2) {
            std::cout << std::endl;
            std::cout << "== calculation details ==" << std::endl;
            std::cout << "offensive stat stage: " << static_cast<int>(calc.offensive_stat_stage)
                      << std::endl;
            std::cout << "defensive stat stage: " << static_cast<int>(calc.defensive_stat_stage)
                      << std::endl;
            std::cout << "offensive stat: " << calc.offensive_stat << std::endl;
            std::cout << "defensive stat: " << calc.defensive_stat << std::endl;
            std::cout << "modified offense value: " << calc.offense_calc << std::endl;
            std::cout << "modified defense value: " << calc.defense_calc << std::endl;
            std::cout << "AT value (offense + power): " << calc.damage_calc_at << std::endl;
            std::cout << "DEF value (defense): " << calc.damage_calc_def << std::endl;
            std::cout << "FLV value (level + [offense - defense]/8): " << calc.damage_calc_flv
                      << std::endl;
            std::cout
                << "damage calc base value (153/256*AT - DEF/2 + 50*ln[10*(FLV + 50)] - 311): "
                << calc.damage_calc_base << std::endl;
            std::cout << "damage calc static multiplier: " << calc.static_damage_mult.val()
                      << std::endl;
            std::cout << "damage calc (base * static mult): " << calc.damage_calc << std::endl;
            std::cout << "damage calc random multiplier: [" << calc.damage_calc_random_mult_pct
                      << "%, " << dungeon_max.damage_calc.damage_calc_random_mult_pct << "%]"
                      << std::endl;
        }
        if (verbose >= 3) {
            std::cout << std::endl;
            std::cout << "== modifier details ==" << std::endl;
            std::cout << "item attack modifier: " << static_cast<int>(calc.item_atk_modifier)
                      << std::endl;
            std::cout << "item special attack modifier: "
                      << static_cast<int>(calc.item_sp_atk_modifier) << std::endl;
            std::cout << "item defense modifier: " << static_cast<int>(calc.item_def_modifier)
                      << std::endl;
            std::cout << "item special defense modifier: "
                      << static_cast<int>(calc.item_sp_def_modifier) << std::endl;
            std::cout << "ability offense modifier: "
                      << static_cast<int>(calc.ability_offense_modifier) << std::endl;
            std::cout << "ability defense modifier: "
                      << static_cast<int>(calc.ability_defense_modifier) << std::endl;
            std::cout << "IQ skill offense modifier: "
                      << static_cast<int>(calc.iq_skill_offense_modifier) << std::endl;
            std::cout << "IQ skill defense modifier: "
                      << static_cast<int>(calc.iq_skill_defense_modifier) << std::endl;
            std::cout << "Scope Lens/Sharpshooter: " << calc.scope_lens_or_sharpshooter_activated
                      << std::endl;
            std::cout << "Patsy Band: " << calc.patsy_band_activated << std::endl;
            std::cout << "Half physical damage: " << calc.half_physical_damage_activated
                      << std::endl;
            std::cout << "Half special damage: " << calc.half_special_damage_activated << std::endl;
            std::cout << "Focus Energy: " << calc.focus_energy_activated << std::endl;
            std::cout << "Type-Advantage Master: " << calc.type_advantage_master_activated
                      << std::endl;
            std::cout << "Cloudy: " << calc.cloudy_drop_activated << std::endl;
            std::cout << "Rain: " << calc.rain_multiplier_activated << std::endl;
            std::cout << "Sunny: " << calc.sunny_multiplier_activated << std::endl;
            std::cout << "Thick Fat/Heatproof: " << calc.fire_move_ability_drop_activated
                      << std::endl;
            std::cout << "Flash Fire: " << calc.flash_fire_activated << std::endl;
            std::cout << "Levitate: " << calc.levitate_activated << std::endl;
            std::cout << "Overgrow: " << calc.overgrow_boost_activated << std::endl;
            std::cout << "Swarm: " << calc.swarm_boost_activated << std::endl;
            std::cout << "Blaze/Dry Skin: " << calc.fire_move_ability_boost_activated << std::endl;
            std::cout << "Scrappy: " << calc.scrappy_activated << std::endl;
            std::cout << "Super Luck: " << calc.super_luck_activated << std::endl;
            std::cout << "Sniper: " << calc.sniper_activated << std::endl;
            std::cout << "STAB: " << calc.stab_boost_activated << std::endl;
            std::cout << "Mud Sport/Fog: " << calc.electric_move_dampened << std::endl;
            std::cout << "Water Sport: " << calc.water_sport_drop_activated << std::endl;
            std::cout << "Charge: " << calc.charge_boost_activated << std::endl;
            std::cout << "Ghost immunity: " << calc.ghost_immunity_activated << std::endl;
            std::cout << "Skull Bash: " << calc.skull_bash_defense_boost_activated << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

std::string monster_summary(const Monster& monster) {
    std::string summary =
        "Lv. " + std::to_string(monster.level) + " " + ids::MONSTER[monster.apparent_id] + ", " +
        (monster.is_not_team_member ? "enemy" : "team") + (monster.is_team_leader ? " leader" : "");
    if (monster.held_item.exists && monster.held_item.id != eos::ITEM_NOTHING) {
        summary += std::string(", holding ") + (monster.held_item.sticky ? "sticky " : "") +
                   ids::ITEM[monster.held_item.id];
    }
    return summary;
}
