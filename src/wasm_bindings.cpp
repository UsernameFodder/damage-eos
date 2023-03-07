#include <algorithm>
#include <iostream>
#include <emscripten/bind.h>
#include "cfgparse.hpp"
#include "damage.hpp"
#include "idmap.hpp"
#include "mechanics.hpp"
#include "pmdsky.hpp"

namespace emscripten {
namespace internal {

// This is a neat trick to implicitly convert std::vector to JS's Array type
// https://github.com/emscripten-core/emscripten/issues/11070#issuecomment-717675128
template <typename T, typename Allocator> struct BindingType<std::vector<T, Allocator>> {
    using ValBinding = BindingType<val>;
    using WireType = ValBinding::WireType;

    static WireType toWireType(const std::vector<T, Allocator>& vec) {
        return ValBinding::toWireType(val::array(vec));
    }

    static std::vector<T, Allocator> fromWireType(WireType value) {
        return vecFromJSArray<T>(ValBinding::fromWireType(value));
    }
};

template <typename T>
struct TypeID<T, typename std::enable_if_t<std::is_same<
                     typename Canonicalized<T>::type,
                     std::vector<typename Canonicalized<T>::type::value_type,
                                 typename Canonicalized<T>::type::allocator_type>>::value>> {
    static constexpr TYPEID get() { return TypeID<val>::get(); }
};

} // namespace internal
} // namespace emscripten

using namespace emscripten;

namespace js {
using nlohmann::json;

std::vector<std::string> get_moves() {
    auto all_moves = ids::MOVE.all_except();
    // Insert the special projectile item moves; see cfgparse.cpp
    all_moves.reserve(all_moves.size() + cfgparse::PROJECTILE_ITEMS.size());
    auto projectile =
        std::find(all_moves.begin(), all_moves.end(), ids::MOVE[eos::MOVE_PROJECTILE]);
    for (auto& item : cfgparse::PROJECTILE_ITEMS) {
        all_moves.insert(++projectile, ids::ITEM[item.id]);
    }
    return all_moves;
}
std::vector<std::string> get_projectile_moves() {
    std::vector<std::string> projectiles;
    projectiles.reserve(1 + cfgparse::PROJECTILE_ITEMS.size());
    projectiles.push_back(ids::MOVE[eos::MOVE_PROJECTILE]);
    for (auto& proj : cfgparse::PROJECTILE_ITEMS) {
        projectiles.push_back(ids::ITEM[proj.id]);
    }
    return projectiles;
}
std::vector<std::string> get_species() {
    return ids::MONSTER.all_except({eos::MONSTER_NONE, eos::MONSTER_NONE_SECONDARY});
}
std::vector<std::string> get_types() { return ids::TYPE.all_except({eos::TYPE_NEUTRAL}); }
std::vector<std::string> get_abilities() {
    // Only a couple abilities are actually used anywhere; only return these
    return {
        ids::ABILITY[eos::ABILITY_UNKNOWN],      ids::ABILITY[eos::ABILITY_THICK_FAT],
        ids::ABILITY[eos::ABILITY_INTIMIDATE],   ids::ABILITY[eos::ABILITY_BATTLE_ARMOR],
        ids::ABILITY[eos::ABILITY_TORRENT],      ids::ABILITY[eos::ABILITY_GUTS],
        ids::ABILITY[eos::ABILITY_SHELL_ARMOR],  ids::ABILITY[eos::ABILITY_OVERGROW],
        ids::ABILITY[eos::ABILITY_SAND_VEIL],    ids::ABILITY[eos::ABILITY_HUGE_POWER],
        ids::ABILITY[eos::ABILITY_VOLT_ABSORB],  ids::ABILITY[eos::ABILITY_WATER_ABSORB],
        ids::ABILITY[eos::ABILITY_HUSTLE],       ids::ABILITY[eos::ABILITY_LIGHTNINGROD],
        ids::ABILITY[eos::ABILITY_COMPOUNDEYES], ids::ABILITY[eos::ABILITY_MARVEL_SCALE],
        ids::ABILITY[eos::ABILITY_WONDER_GUARD], ids::ABILITY[eos::ABILITY_LEVITATE],
        ids::ABILITY[eos::ABILITY_PLUS],         ids::ABILITY[eos::ABILITY_SOUNDPROOF],
        ids::ABILITY[eos::ABILITY_MINUS],        ids::ABILITY[eos::ABILITY_SWARM],
        ids::ABILITY[eos::ABILITY_BLAZE],        ids::ABILITY[eos::ABILITY_FLASH_FIRE],
        ids::ABILITY[eos::ABILITY_PURE_POWER],   ids::ABILITY[eos::ABILITY_ANGER_POINT],
        ids::ABILITY[eos::ABILITY_TINTED_LENS],  ids::ABILITY[eos::ABILITY_MOLD_BREAKER],
        ids::ABILITY[eos::ABILITY_DRY_SKIN],     ids::ABILITY[eos::ABILITY_SCRAPPY],
        ids::ABILITY[eos::ABILITY_SUPER_LUCK],   ids::ABILITY[eos::ABILITY_SOLAR_POWER],
        ids::ABILITY[eos::ABILITY_RECKLESS],     ids::ABILITY[eos::ABILITY_SNIPER],
        ids::ABILITY[eos::ABILITY_HEATPROOF],    ids::ABILITY[eos::ABILITY_DOWNLOAD],
        ids::ABILITY[eos::ABILITY_TANGLED_FEET], ids::ABILITY[eos::ABILITY_ADAPTABILITY],
        ids::ABILITY[eos::ABILITY_TECHNICIAN],   ids::ABILITY[eos::ABILITY_IRON_FIST],
        ids::ABILITY[eos::ABILITY_MOTOR_DRIVE],  ids::ABILITY[eos::ABILITY_UNAWARE],
        ids::ABILITY[eos::ABILITY_RIVALRY],      ids::ABILITY[eos::ABILITY_NO_GUARD],
        ids::ABILITY[eos::ABILITY_NORMALIZE],    ids::ABILITY[eos::ABILITY_SOLID_ROCK],
        ids::ABILITY[eos::ABILITY_FILTER],       ids::ABILITY[eos::ABILITY_KLUTZ],
        ids::ABILITY[eos::ABILITY_FLOWER_GIFT],  ids::ABILITY[eos::ABILITY_SNOW_CLOAK],
        ids::ABILITY[eos::ABILITY_FOREWARN],     ids::ABILITY[eos::ABILITY_STORM_DRAIN],
    };
}
std::vector<std::string> get_held_items() {
    // Only a couple held items are actually used anywhere; only return these
    return {
        ids::ITEM[eos::ITEM_NOTHING],      ids::ITEM[eos::ITEM_Y_RAY_SPECS],
        ids::ITEM[eos::ITEM_SCOPE_LENS],   ids::ITEM[eos::ITEM_PATSY_BAND],
        ids::ITEM[eos::ITEM_POWER_BAND],   ids::ITEM[eos::ITEM_DEF_SCARF],
        ids::ITEM[eos::ITEM_SPECIAL_BAND], ids::ITEM[eos::ITEM_ZINC_BAND],
        ids::ITEM[eos::ITEM_DETECT_BAND],  ids::ITEM[eos::ITEM_SPACE_GLOBE],
        ids::ITEM[eos::ITEM_MUNCH_BELT],   ids::ITEM[eos::ITEM_WEATHER_BAND],
        ids::ITEM[eos::ITEM_HEAL_SEED],    ids::ITEM[eos::ITEM_ORAN_BERRY],
        ids::ITEM[eos::ITEM_SITRUS_BERRY], ids::ITEM[eos::ITEM_EYEDROP_SEED],
        ids::ITEM[eos::ITEM_REVIVER_SEED], ids::ITEM[eos::ITEM_BLINKER_SEED],
        ids::ITEM[eos::ITEM_DOOM_SEED],    ids::ITEM[eos::ITEM_X_EYE_SEED],
        ids::ITEM[eos::ITEM_LIFE_SEED],    ids::ITEM[eos::ITEM_RAWST_BERRY],
        ids::ITEM[eos::ITEM_HUNGER_SEED],  ids::ITEM[eos::ITEM_QUICK_SEED],
        ids::ITEM[eos::ITEM_PECHA_BERRY],  ids::ITEM[eos::ITEM_CHERI_BERRY],
        ids::ITEM[eos::ITEM_TOTTER_SEED],  ids::ITEM[eos::ITEM_SLEEP_SEED],
        ids::ITEM[eos::ITEM_PLAIN_SEED],   ids::ITEM[eos::ITEM_WARP_SEED],
        ids::ITEM[eos::ITEM_BLAST_SEED],   ids::ITEM[eos::ITEM_JOY_SEED],
        ids::ITEM[eos::ITEM_CHESTO_BERRY], ids::ITEM[eos::ITEM_STUN_SEED],
        ids::ITEM[eos::ITEM_GOLDEN_SEED],  ids::ITEM[eos::ITEM_VILE_SEED],
        ids::ITEM[eos::ITEM_PURE_SEED],    ids::ITEM[eos::ITEM_VIOLENT_SEED],
        ids::ITEM[eos::ITEM_VANISH_SEED],  ids::ITEM[eos::ITEM_DROPEYE_SEED],
        ids::ITEM[eos::ITEM_REVISER_SEED], ids::ITEM[eos::ITEM_SLIP_SEED],
        ids::ITEM[eos::ITEM_VIA_SEED],     ids::ITEM[eos::ITEM_OREN_BERRY],
        ids::ITEM[eos::ITEM_DOUGH_SEED],   ids::ITEM[eos::ITEM_SILVER_BOW],
        ids::ITEM[eos::ITEM_BROWN_BOW],    ids::ITEM[eos::ITEM_RED_BOW],
        ids::ITEM[eos::ITEM_PINK_BOW],     ids::ITEM[eos::ITEM_ORANGE_BOW],
        ids::ITEM[eos::ITEM_YELLOW_BOW],   ids::ITEM[eos::ITEM_LIME_BOW],
        ids::ITEM[eos::ITEM_GREEN_BOW],    ids::ITEM[eos::ITEM_VIRIDIAN_BOW],
        ids::ITEM[eos::ITEM_MINTY_BOW],    ids::ITEM[eos::ITEM_SKY_BLUE_BOW],
        ids::ITEM[eos::ITEM_BLUE_BOW],     ids::ITEM[eos::ITEM_COBALT_BOW],
        ids::ITEM[eos::ITEM_PURPLE_BOW],   ids::ITEM[eos::ITEM_VIOLET_BOW],
        ids::ITEM[eos::ITEM_FUCHSIA_BOW],
    };
}
std::vector<std::string> get_weather_types() {
    return ids::WEATHER.all_except({eos::WEATHER_RANDOM});
}
std::vector<std::string> get_iq_skills() {
    // Only a couple IQ skills are actually used anywhere; only return these
    return {
        ids::IQ[eos::IQ_TYPE_ADVANTAGE_MASTER],
        ids::IQ[eos::IQ_SURE_HIT_ATTACKER],
        ids::IQ[eos::IQ_QUICK_DODGER],
        ids::IQ[eos::IQ_SHARPSHOOTER],
        ids::IQ[eos::IQ_AGGRESSOR],
        ids::IQ[eos::IQ_DEFENDER],
        ids::IQ[eos::IQ_POWER_PITCHER],
        ids::IQ[eos::IQ_CONCENTRATOR],
        ids::IQ[eos::IQ_COUNTER_BASHER],
        ids::IQ[eos::IQ_CHEERLEADER],
        ids::IQ[eos::IQ_ERRATIC_PLAYER],
        ids::IQ[eos::IQ_PRACTICE_SWINGER],
        ids::IQ[eos::IQ_CLUTCH_PERFORMER],
        ids::IQ[eos::IQ_CRITICAL_DODGER],
    };
}

struct NameWithAlternates {
    std::string name;
    std::vector<std::string> alternate_names;
};
std::vector<NameWithAlternates> get_statuses() {
    // Only certain statuses are supported by the damage calc, so enumerate them explicitly.
    // See cfgparse.cpp
    constexpr std::array<eos::status_id, 27> supported = {
        eos::STATUS_SLEEP,       eos::STATUS_NIGHTMARE,    eos::STATUS_NAPPING,
        eos::STATUS_BURN,        eos::STATUS_POISONED,     eos::STATUS_BADLY_POISONED,
        eos::STATUS_PARALYSIS,   eos::STATUS_IDENTIFYING,  eos::STATUS_CONFUSED,
        eos::STATUS_SKULL_BASH,  eos::STATUS_FLYING,       eos::STATUS_BOUNCING,
        eos::STATUS_DIVING,      eos::STATUS_DIGGING,      eos::STATUS_SHADOW_FORCE,
        eos::STATUS_CHARGING,    eos::STATUS_REFLECT,      eos::STATUS_LIGHT_SCREEN,
        eos::STATUS_LUCKY_CHANT, eos::STATUS_GASTRO_ACID,  eos::STATUS_SURE_SHOT,
        eos::STATUS_WHIFFER,     eos::STATUS_FOCUS_ENERGY, eos::STATUS_CROSS_EYED,
        eos::STATUS_MIRACLE_EYE, eos::STATUS_MAGNET_RISE,  eos::STATUS_EXPOSED,
    };
    std::vector<NameWithAlternates> statuses;
    statuses.reserve(supported.size());
    for (auto s : supported) {
        statuses.push_back({ids::STATUS[s], ids::STATUS.alternate_names(s)});
    }
    // Special catch-all status; see cfgparse.cpp
    statuses.push_back({"Guts/Marvel Scale",
                        {"Catch-all for status conditions that activate Guts and Marvel Scale"}});
    return statuses;
}

std::vector<NameWithAlternates> get_exclusive_item_effects() {
    // Only some exclusive item effects are actually used anywhere; only return these
    constexpr std::array<eos::exclusive_item_effect_id, 42> used = {
        eos::EXCLUSIVE_EFF_NO_CRITICAL_HITS,
        eos::EXCLUSIVE_EFF_HALVED_PHYSICAL_DAMAGE,
        eos::EXCLUSIVE_EFF_HALVED_SPECIAL_DAMAGE,
        eos::EXCLUSIVE_EFF_EVASION_BOOST_WHEN_CLEAR,
        eos::EXCLUSIVE_EFF_EVASION_BOOST_WHEN_SUNNY,
        eos::EXCLUSIVE_EFF_EVASION_BOOST_WHEN_SANDSTORM,
        eos::EXCLUSIVE_EFF_EVASION_BOOST_WHEN_CLOUDY,
        eos::EXCLUSIVE_EFF_EVASION_BOOST_WHEN_RAINY,
        eos::EXCLUSIVE_EFF_EVASION_BOOST_WHEN_HAIL,
        eos::EXCLUSIVE_EFF_EVASION_BOOST_WHEN_FOGGY,
        eos::EXCLUSIVE_EFF_BYPASS_REFLECT_LIGHT_SCREEN,
        eos::EXCLUSIVE_EFF_SCRAPPY,
        eos::EXCLUSIVE_EFF_MIRACLE_EYE,
        eos::EXCLUSIVE_EFF_HALVED_DAMAGE,
        eos::EXCLUSIVE_EFF_DAMAGE_BOOST_50_PCT,
        eos::EXCLUSIVE_EFF_NO_FIRE_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_WATER_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_GRASS_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_ELECTRIC_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_FIGHTING_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_GROUND_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_FLYING_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_PSYCHIC_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_GHOST_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_DRAGON_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_DARK_DAMAGE,
        eos::EXCLUSIVE_EFF_NO_STEEL_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_FIRE_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_WATER_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_GRASS_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_ELECTRIC_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_ICE_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_FIGHTING_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_GROUND_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_FLYING_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_PSYCHIC_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_BUG_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_ROCK_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_GHOST_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_DRAGON_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_DARK_DAMAGE,
        eos::EXCLUSIVE_EFF_ABSORB_STEEL_DAMAGE,
    };
    std::vector<NameWithAlternates> effects;
    effects.reserve(used.size());
    for (auto u : used) {
        effects.push_back(
            {ids::EXCLUSIVE_ITEM_EFFECT[u], ids::EXCLUSIVE_ITEM_EFFECT.alternate_names(u)});
    }
    return effects;
}

struct MoveDetails {
    int base_power = 0;
    std::string type = "";
    std::string category = "";
    int pp = 0;
    int accuracy1 = 0;
    int accuracy2 = 0;
    int strikes = 0;
    int crit_chance = 0;
    bool unsupported = 0;
    std::string special_notes = "";
};
MoveDetails get_move_details(std::string name, bool time_darkness = false) {
    try {
        mechanics::data_files::MoveData data;
        int16_t base_power;
        std::string special_notes;

        if (auto item = cfgparse::find_projectile_item(name)) {
            // Deal with the special projectile item moves; see cfgparse.cpp
            data = mechanics::data_files::MOVES[eos::MOVE_PROJECTILE];
            base_power = item->base_power;
            special_notes = "Projectile item (not a real move)";
        } else {
            data = mechanics::data_files::MOVES[ids::MOVE[name]];
            base_power = mechanics::get_move_base_power(ids::MOVE[name], time_darkness);
            special_notes = data.special_notes;
        }

        return {
            base_power,    ids::TYPE[data.type], ids::MOVE_CATEGORY[data.category],
            data.pp,       data.accuracy1,       data.accuracy2,
            data.strikes,  data.crit_chance,     data.unsupported,
            special_notes,
        };
    } catch (const std::exception& e) {
        std::cerr << "[get_move_details(" << name << ")] " << e.what() << std::endl;
        return {};
    }
}

struct SpeciesDetails {
    std::string gender = "";
    std::string type1 = "";
    std::string type2 = "";
    std::string ability1 = "";
    std::string ability2 = "";
    int weight = 0;
    int size = 0;
};
SpeciesDetails get_species_details(std::string name) {
    try {
        auto data = mechanics::data_files::MONSTERS[ids::MONSTER[name]];
        return {
            ids::GENDER[data.gender],
            ids::TYPE[data.type1],
            ids::TYPE[data.type2],
            ids::ABILITY[data.ability1],
            ids::ABILITY[data.ability2],
            data.weight,
            data.size,
        };
    } catch (const std::exception& e) {
        std::cerr << "[get_species_details(" << name << ")] " << e.what() << std::endl;
        return {};
    }
}

struct ModifierDetails {
    int item_atk = 0;
    int item_spatk = 0;
    int item_def = 0;
    int item_spdef = 0;
    int ability_offense = 0;
    int ability_defense = 0;
    int iq_skill_offense = 0;
    int iq_skill_defense = 0;
    bool scope_lens_or_sharpshooter = false;
    bool patsy_band = false;
    bool half_physical_damage = false;
    bool half_special_damage = false;
    bool focus_energy = false;
    bool type_advantage_master = false;
    bool cloudy_drop = false;
    bool rain_multiplier = false;
    bool sunny_multiplier = false;
    bool thick_fat_heatproof = false;
    bool flash_fire = false;
    bool levitate = false;
    bool overgrow = false;
    bool swarm = false;
    bool blaze_dry_skin = false;
    bool scrappy = false;
    bool super_luck = false;
    bool sniper = false;
    bool stab = false;
    bool mud_sport_fog = false;
    bool water_sport = false;
    bool charge = false;
    bool ghost_immunity = false;
    bool skull_bash = false;
};
struct CalcDetails {
    int offensive_stat_stage = 0;
    int defensive_stat_stage = 0;
    int offensive_stat = 0;
    int defensive_stat = 0;
    int offense_calc = 0;
    int defense_calc = 0;
    int damage_calc_at = 0;
    int damage_calc_def = 0;
    int damage_calc_flv = 0;
    int damage_calc_base = 0;
    double static_damage_mult = 0;
    int damage_calc = 0;
    int avg_random_damage_mult_pct = 0;
    int min_random_damage_mult_pct = 0;
    int max_random_damage_mult_pct = 0;
    ModifierDetails modifiers = {};
};
struct ResultDetails {
    std::string damage_message = "";
    std::string type_matchup = "";
    std::string indiv_type_matchup1 = "";
    std::string indiv_type_matchup2 = "";
    std::string move_type = "";
    std::string move_category = "";
    bool critical_hit = false;
    bool full_type_immunity = false;
    bool no_damage = false;
    CalcDetails calc = {};
};
struct CalcDamageResult {
    int avg_damage = 0;
    int min_damage = 0;
    int max_damage = 0;
    bool healed = false;
    double hit_chance = 0;
    bool guaranteed_miss = false;
    int crit_chance = 0;
    ResultDetails details = {};
};
CalcDamageResult calc_damage(std::string config_str) {
    try {
        json cfg = json::parse(config_str);
        auto [dungeon, attacker, defender, move, attack_power] = parse_cfg(cfg);
        auto move_spec = mechanics::MoveSpec(move.id);

        // Copy for other damage calcs since the inputs can be modified by the simulation
        DungeonState dungeon_min = dungeon;
        DungeonState dungeon_max = dungeon;
        MonsterEntity attacker_min = attacker;
        MonsterEntity defender_min = defender;
        MonsterEntity attacker_max = attacker;
        MonsterEntity defender_max = defender;
        Move move_min = move;
        Move move_max = move;
        dungeon.rng.variance_dial = 0.5;   // average damage roll
        dungeon_min.rng.variance_dial = 0; // minimum damage roll
        dungeon_max.rng.variance_dial = 1; // maximum damage roll

        DamageData details = {};
        DamageData details_min_var = {};
        DamageData details_max_var = {};
        int32_t damage = 0;
        int32_t damage_min_var = 0;
        int32_t damage_max_var = 0;

        if (move.id == eos::MOVE_PROJECTILE) {
            damage =
                simulate_damage_calc_projectile(details, dungeon, attacker, defender, attack_power);
            damage_min_var = simulate_damage_calc_projectile(
                details_min_var, dungeon_min, attacker_min, defender_min, attack_power);
            damage_max_var = simulate_damage_calc_projectile(
                details_max_var, dungeon_max, attacker_max, defender_max, attack_power);
        } else {
            damage = simulate_damage_calc(details, dungeon, attacker, defender, move);
            damage_min_var = simulate_damage_calc(details_min_var, dungeon_min, attacker_min,
                                                  defender_min, move_min);
            damage_max_var = simulate_damage_calc(details_max_var, dungeon_max, attacker_max,
                                                  defender_max, move_max);
        }

        CalcDamageResult result = {};
        if (details.healed) {
            result.avg_damage = details.damage;
            result.min_damage = details_min_var.damage;
            result.max_damage = details_max_var.damage;
        } else {
            result.avg_damage = damage;
            result.min_damage = damage_min_var;
            result.max_damage = damage_max_var;
        }
        result.healed = details.healed;
        if (dungeon.damage_calc.two_turn_move_forced_miss ||
            dungeon.damage_calc.soundproof_activated ||
            dungeon.damage_calc.first_hit_check_failed || dungeon.damage_calc.dream_eater_failed ||
            dungeon.damage_calc.last_resort_failed) {
            result.guaranteed_miss = true;
            return result;
        }
        result.hit_chance = dungeon.rng.get_combined_hit_percentage();
        result.crit_chance = dungeon.rng.get_computed_crit_chance();

        const auto& calc = dungeon.damage_calc;
        auto& res_details = result.details;
        res_details.damage_message = ids::DAMAGE_MESSAGE[details.damage_message];
        res_details.type_matchup = ids::TYPE_MATCHUP[details.type_matchup];
        res_details.indiv_type_matchup1 = ids::TYPE_MATCHUP[calc.move_indiv_type_matchups[0]];
        res_details.indiv_type_matchup2 = ids::TYPE_MATCHUP[calc.move_indiv_type_matchups[1]];
        res_details.move_type = ids::TYPE[details.type];
        res_details.move_category = ids::MOVE_CATEGORY[details.category];
        res_details.critical_hit = details.critical_hit;
        res_details.full_type_immunity = details.full_type_immunity;
        res_details.no_damage = details.no_damage;

        auto& calc_details = res_details.calc;
        calc_details.offensive_stat_stage = calc.offensive_stat_stage;
        calc_details.defensive_stat_stage = calc.defensive_stat_stage;
        calc_details.offensive_stat = calc.offensive_stat;
        calc_details.defensive_stat = calc.defensive_stat;
        calc_details.offense_calc = calc.offense_calc;
        calc_details.defense_calc = calc.defense_calc;
        calc_details.damage_calc_at = calc.damage_calc_at;
        calc_details.damage_calc_def = calc.damage_calc_def;
        calc_details.damage_calc_flv = calc.damage_calc_flv;
        calc_details.damage_calc_base = calc.damage_calc_base;
        calc_details.static_damage_mult = calc.static_damage_mult.val();
        calc_details.damage_calc = calc.damage_calc;
        calc_details.avg_random_damage_mult_pct = calc.damage_calc_random_mult_pct;
        calc_details.min_random_damage_mult_pct =
            dungeon_min.damage_calc.damage_calc_random_mult_pct;
        calc_details.max_random_damage_mult_pct =
            dungeon_max.damage_calc.damage_calc_random_mult_pct;

        auto& mod_details = calc_details.modifiers;
        mod_details.item_atk = calc.item_atk_modifier;
        mod_details.item_spatk = calc.item_sp_atk_modifier;
        mod_details.item_def = calc.item_def_modifier;
        mod_details.item_spdef = calc.item_sp_def_modifier;
        mod_details.ability_offense = calc.ability_offense_modifier;
        mod_details.ability_defense = calc.ability_defense_modifier;
        mod_details.iq_skill_offense = calc.iq_skill_offense_modifier;
        mod_details.iq_skill_defense = calc.iq_skill_defense_modifier;
        mod_details.scope_lens_or_sharpshooter = calc.scope_lens_or_sharpshooter_activated;
        mod_details.patsy_band = calc.patsy_band_activated;
        mod_details.half_physical_damage = calc.half_physical_damage_activated;
        mod_details.half_special_damage = calc.half_special_damage_activated;
        mod_details.focus_energy = calc.focus_energy_activated;
        mod_details.type_advantage_master = calc.type_advantage_master_activated;
        mod_details.cloudy_drop = calc.cloudy_drop_activated;
        mod_details.rain_multiplier = calc.rain_multiplier_activated;
        mod_details.sunny_multiplier = calc.sunny_multiplier_activated;
        mod_details.thick_fat_heatproof = calc.fire_move_ability_drop_activated;
        mod_details.flash_fire = calc.flash_fire_activated;
        mod_details.levitate = calc.levitate_activated;
        mod_details.overgrow = calc.overgrow_boost_activated;
        mod_details.swarm = calc.swarm_boost_activated;
        mod_details.blaze_dry_skin = calc.fire_move_ability_boost_activated;
        mod_details.scrappy = calc.scrappy_activated;
        mod_details.super_luck = calc.super_luck_activated;
        mod_details.sniper = calc.sniper_activated;
        mod_details.stab = calc.stab_boost_activated;
        mod_details.mud_sport_fog = calc.electric_move_dampened;
        mod_details.water_sport = calc.water_sport_drop_activated;
        mod_details.charge = calc.charge_boost_activated;
        mod_details.ghost_immunity = calc.ghost_immunity_activated;
        mod_details.skull_bash = calc.skull_bash_defense_boost_activated;

        return result;
    } catch (const std::exception& e) {
        std::cerr << "[calc_damage] " << e.what() << std::endl;
        std::cerr << "(config) " << config_str << std::endl;
        return {};
    }
}
}; // namespace js

EMSCRIPTEN_BINDINGS(damagecalc) {
    constant("FIXED_SUBSTITUTE_ROOM", static_cast<int>(eos::FIXED_SUBSTITUTE_ROOM));

    value_object<js::NameWithAlternates>("NameWithAlternates")
        .field("name", &js::NameWithAlternates::name)
        .field("alternateNames", &js::NameWithAlternates::alternate_names);

    value_object<js::MoveDetails>("MoveDetails")
        .field("basePower", &js::MoveDetails::base_power)
        .field("type", &js::MoveDetails::type)
        .field("category", &js::MoveDetails::category)
        .field("pp", &js::MoveDetails::pp)
        .field("accuracy1", &js::MoveDetails::accuracy1)
        .field("accuracy2", &js::MoveDetails::accuracy2)
        .field("strikes", &js::MoveDetails::strikes)
        .field("critChance", &js::MoveDetails::crit_chance)
        .field("unsupported", &js::MoveDetails::unsupported)
        .field("specialNotes", &js::MoveDetails::special_notes);

    value_object<js::SpeciesDetails>("SpeciesDetails")
        .field("gender", &js::SpeciesDetails::gender)
        .field("type1", &js::SpeciesDetails::type1)
        .field("type2", &js::SpeciesDetails::type2)
        .field("ability1", &js::SpeciesDetails::ability1)
        .field("ability2", &js::SpeciesDetails::ability2)
        .field("weight", &js::SpeciesDetails::weight)
        .field("size", &js::SpeciesDetails::size);

    value_object<js::ModifierDetails>("ModifierDetails")
        .field("itemAtk", &js::ModifierDetails::item_atk)
        .field("itemSpAtk", &js::ModifierDetails::item_spatk)
        .field("itemDef", &js::ModifierDetails::item_def)
        .field("itemSpDef", &js::ModifierDetails::item_spdef)
        .field("abilityOffense", &js::ModifierDetails::ability_offense)
        .field("abilityDefense", &js::ModifierDetails::ability_defense)
        .field("iqSkillOffense", &js::ModifierDetails::iq_skill_offense)
        .field("iqSkillDefense", &js::ModifierDetails::iq_skill_defense)
        .field("scopeLensOrSharpshooter", &js::ModifierDetails::scope_lens_or_sharpshooter)
        .field("patsyBand", &js::ModifierDetails::patsy_band)
        .field("halfPhysicalDamage", &js::ModifierDetails::half_physical_damage)
        .field("halfSpecialDamage", &js::ModifierDetails::half_special_damage)
        .field("focusEnergy", &js::ModifierDetails::focus_energy)
        .field("typeAdvantageMaster", &js::ModifierDetails::type_advantage_master)
        .field("cloudyDrop", &js::ModifierDetails::cloudy_drop)
        .field("rainMultiplier", &js::ModifierDetails::rain_multiplier)
        .field("sunnyMultiplier", &js::ModifierDetails::sunny_multiplier)
        .field("thickFatHeatproof", &js::ModifierDetails::thick_fat_heatproof)
        .field("flashFire", &js::ModifierDetails::flash_fire)
        .field("levitate", &js::ModifierDetails::levitate)
        .field("overgrow", &js::ModifierDetails::overgrow)
        .field("swarm", &js::ModifierDetails::swarm)
        .field("blazeDrySkin", &js::ModifierDetails::blaze_dry_skin)
        .field("scrappy", &js::ModifierDetails::scrappy)
        .field("superLuck", &js::ModifierDetails::super_luck)
        .field("sniper", &js::ModifierDetails::sniper)
        .field("stab", &js::ModifierDetails::stab)
        .field("mudSportFog", &js::ModifierDetails::mud_sport_fog)
        .field("waterSport", &js::ModifierDetails::water_sport)
        .field("charge", &js::ModifierDetails::charge)
        .field("ghostImmunity", &js::ModifierDetails::ghost_immunity)
        .field("skullBash", &js::ModifierDetails::skull_bash);
    value_object<js::CalcDetails>("CalcDetails")
        .field("offensiveStatStage", &js::CalcDetails::offensive_stat_stage)
        .field("defensiveStatStage", &js::CalcDetails::defensive_stat_stage)
        .field("offensiveStat", &js::CalcDetails::offensive_stat)
        .field("defensiveStat", &js::CalcDetails::defensive_stat)
        .field("offenseCalc", &js::CalcDetails::offense_calc)
        .field("defenseCalc", &js::CalcDetails::defense_calc)
        .field("damageCalcAt", &js::CalcDetails::damage_calc_at)
        .field("damageCalcDef", &js::CalcDetails::damage_calc_def)
        .field("damageCalcFlv", &js::CalcDetails::damage_calc_flv)
        .field("damageCalcBase", &js::CalcDetails::damage_calc_base)
        .field("staticDamageMult", &js::CalcDetails::static_damage_mult)
        .field("damageCalc", &js::CalcDetails::damage_calc)
        .field("avgRandomDamageMultPct", &js::CalcDetails::avg_random_damage_mult_pct)
        .field("minRandomDamageMultPct", &js::CalcDetails::min_random_damage_mult_pct)
        .field("maxRandomDamageMultPct", &js::CalcDetails::max_random_damage_mult_pct)
        .field("modifiers", &js::CalcDetails::modifiers);
    value_object<js::ResultDetails>("ResultDetails")
        .field("damageMessage", &js::ResultDetails::damage_message)
        .field("typeMatchup", &js::ResultDetails::type_matchup)
        .field("indivTypeMatchup1", &js::ResultDetails::indiv_type_matchup1)
        .field("indivTypeMatchup2", &js::ResultDetails::indiv_type_matchup2)
        .field("moveType", &js::ResultDetails::move_type)
        .field("moveCategory", &js::ResultDetails::move_category)
        .field("criticalHit", &js::ResultDetails::critical_hit)
        .field("fullTypeImmunity", &js::ResultDetails::full_type_immunity)
        .field("noDamage", &js::ResultDetails::no_damage)
        .field("calc", &js::ResultDetails::calc);
    value_object<js::CalcDamageResult>("CalcDamageResult")
        .field("avgDamage", &js::CalcDamageResult::avg_damage)
        .field("minDamage", &js::CalcDamageResult::min_damage)
        .field("maxDamage", &js::CalcDamageResult::max_damage)
        .field("healed", &js::CalcDamageResult::healed)
        .field("hitChance", &js::CalcDamageResult::hit_chance)
        .field("guaranteedMiss", &js::CalcDamageResult::guaranteed_miss)
        .field("critChance", &js::CalcDamageResult::crit_chance)
        .field("details", &js::CalcDamageResult::details);

    function("getMoves", &js::get_moves);
    function("getProjectileMoves", &js::get_projectile_moves);
    function("getSpecies", &js::get_species);
    function("getTypes", &js::get_types);
    function("getAbilities", &js::get_abilities);
    function("getHeldItems", &js::get_held_items);
    function("getWeatherTypes", &js::get_weather_types);
    function("getIqSkills", &js::get_iq_skills);
    function("getStatuses", &js::get_statuses);
    function("getExclusiveItemEffects", &js::get_exclusive_item_effects);
    function("getMoveDetails", &js::get_move_details);
    function("getSpeciesDetails", &js::get_species_details);
    function("calcDamage", &js::calc_damage);
}
