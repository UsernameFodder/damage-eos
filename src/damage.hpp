// Reimplementation of the game's core damage calculation routines

#ifndef DAMAGE_HPP_
#define DAMAGE_HPP_

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include "mathutil.hpp"
#include "mechanics.hpp"
#include "pmdsky.hpp"

using std::int16_t;
using std::int32_t;
using std::int8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint8_t;

// Based on eos::damage_data
struct DamageData {
    int32_t damage = 0;                                            // Damage dealt by the move
    eos::damage_message damage_message = eos::DAMAGE_MESSAGE_MOVE; // Message to display
    eos::type_matchup type_matchup = eos::MATCHUP_IMMUNE; // Result of the type matchup calculation
    eos::type_id type = eos::TYPE_NONE;                   // Type of the move used
    eos::move_category category = eos::CATEGORY_PHYSICAL; // Category of the move used
    bool critical_hit = false;                            // Whether there was a critical hit or not
    // Set to true if the type matchup calculation results in full immunity due to Flash Fire,
    // Levitate, or Magnet Rise. It seems like this is only used to skip the 30-damage override due
    // to the Set Damage status
    bool full_type_immunity = false;
    // This can be set at various points to indicate that no damage should be dealt. This is
    // checked at the very end and overrides all other results of the damage calculation.
    bool no_damage = false;

    // The actual code handles healing during the damage sequence. We're only doing calculation,
    // so keep track of this to report back later
    bool healed = false;
};

// Based on eos::damage_calc_diag
struct DamageCalcDiag {
    eos::type_id move_type = eos::TYPE_NONE;                   // The type of the last move used
    eos::move_category move_category = eos::CATEGORY_PHYSICAL; // The category of the last move used
    // The type matchup of the last move used against the individual types of the defender
    eos::type_matchup move_indiv_type_matchups[2] = {eos::MATCHUP_IMMUNE, eos::MATCHUP_IMMUNE};
    // The modified offensive stat stage of the attacker for the last move used
    uint8_t offensive_stat_stage = 10;
    // The modified defensive stat stage of the defender for the last move used
    uint8_t defensive_stat_stage = 10;
    // The base offensive stat of the attacker for the last move used
    uint16_t offensive_stat = 1;
    // The base defensive stat of the defender for the last move used
    uint16_t defensive_stat = 1;
    // The Flash Fire boost of the attacker when a Fire move was last used
    uint16_t flash_fire_boost = 0;
    // The modified offense value calculated for the attacker for the last move used,
    // prior to being clamped between 0 and 999
    uint16_t offense_calc = 0;
    // The modified defense value calculated for the defender for the last move used
    uint16_t defense_calc = 0;
    uint16_t attacker_level = 0; // The level of the attacker for the last move used
    // The intermediate quantity in the damage calculation called "AT" in debug logging,
    // which corresponds to: round[ min(max(offense_calc, 0), 999) + power_calc ],
    // where power_calc is a modified move power calculated as (intermediate rounding omitted):
    // GetMovePower(...) * (offensive stat stage multipliers) * (offensive multipliers)
    uint16_t damage_calc_at = 0;
    // An identical copy of defense_calc. This is probably a relic of development,
    // when the final defense contribution to the damage formula might have been a different
    // quantity computed from defense_calc, like how damage_calc_at is computed
    // from offense_calc
    uint16_t damage_calc_def = 0;
    // The intermediate quantity in the damage calculation called "FLV" in debug logging
    // (effective level?), which corresponds to: round[ (offense_calc - defense_calc)/8 + level ]
    uint16_t damage_calc_flv = 0;
    // The result of the damage calculation after multiplying the base value by multipliers,
    // but before applying random variation. There are also a couple stray multipliers applied
    // after this result, including multipliers specific to the projectile move (the static 50%,
    // and the Power Pitcher multiplier) and the Air Blade multiplier.
    int32_t damage_calc = 0;
    // The intermediate quantity in the damage calculation resulting from the "base" damage
    // calculation: the sum of the power, attack, defense, and level terms, modified by the
    // non-team-member multiplier if relevant, and clamped between 1 and 999.
    int32_t damage_calc_base = 0;
    // 0x28: The random multiplier applied to the result of the damage calculation, as a
    // percentage (so the actual factor, multiplied by 100), rounded to an integer.
    int32_t damage_calc_random_mult_pct = 0;
    // The calculated "static" damage multiplier applied to the output of the base damage
    // calculation. "Static" in the sense that this part of the multiplier doesn't depend on
    // variables like type-based effects, critical hits, and Reflect/Light Screen. Factors in
    // the static damage multiplier include the argument to CalcDamage, the multiplier due to
    // monster::boosted_attacks, Reckless, and Iron Fist.
    Fx32 static_damage_mult = 0;
    // The net number of attack boosts to an attacker due to a Power Band or Munch Belt.
    // It seems like there's a bug in the code; aura bows do not contribute to this field.
    int8_t item_atk_modifier = 0;
    // The net number of special attack boosts to an attacker due to a Special Band,
    // Munch Belt, or aura bow. It seems like there's a bug in the code; physical attack boosts
    // from aura bows also contribute to this field.
    int8_t item_sp_atk_modifier = 0;
    // The net number of offense boosts to an attacker due to Download, Rivalry,
    // Flower Gift, and Solar Power
    int8_t ability_offense_modifier = 0;
    // The net number of defense boosts to a defender due to Flower Gift
    int8_t ability_defense_modifier = 0;
    // The net number of offense boosts to an attacker due to Aggressor, Defender, and
    // Practice Swinger
    int8_t iq_skill_offense_modifier = 0;
    // The net number of defense boosts to a defender due to Counter Basher, Aggressor, and
    // Defender
    int8_t iq_skill_defense_modifier = 0;
    // The net number of defense boosts to a defender due to a Def Scarf or aura bow.
    // It seems like there's a bug in the code; special defense boosts from aura bows also
    // contribute to this field.
    int8_t item_def_modifier = 0;
    // The net number of special defense boosts to a defender due to a Zinc Band.
    // It seems like there's a bug in the code; aura bows do not contribute to this field.
    int8_t item_sp_def_modifier = 0;
    // Whether or not Scope Lens or Sharpshooter boosted the critical hit rate of a move
    bool scope_lens_or_sharpshooter_activated = false;
    // Whether or not the Patsy Band boosted the critical hit rate of a move
    bool patsy_band_activated = false;
    // Whether or not Reflect or the Time Shield halved the damage from a physical move
    bool half_physical_damage_activated = false;
    // Whether or not Light Screen or the Aqua Mantle halved the damage from a special move
    bool half_special_damage_activated = false;
    // Whether or not the Enhanced critical-hit rate status maxed out the critical hit rate
    // of a move
    bool focus_energy_activated = false;
    // Whether or not Type-Advantage Master boosted the critical hit rate of a move
    bool type_advantage_master_activated = false;
    // Whether or not a non-Normal-type move was dampened by Cloudy weather
    bool cloudy_drop_activated = false;
    // Whether or not a Fire or Water move was affected by Rainy weather
    bool rain_multiplier_activated = false;
    // Whether or not a Fire or Water move was affected by Sunny weather
    bool sunny_multiplier_activated = false;
    // Whether or a Fire move was dampened by Thick Fat or Heatproof
    bool fire_move_ability_drop_activated = false;
    // Whether or not Flash Fire was activated at some point for Fire immunity
    bool flash_fire_activated = false;
    // Whether or not Levitate was activated at some point for Ground immunity
    bool levitate_activated = false;
    bool torrent_boost_activated = false;  // Whether or not a Water move was boosted by Torrent
    bool overgrow_boost_activated = false; // Whether or not a Grass move was boosted by Overgrow
    bool swarm_boost_activated = false;    // Whether or not a Bug move was boosted by Swarm
    // Whether or not a Fire move was boosted by either Blaze or Dry Skin
    bool fire_move_ability_boost_activated = false;
    // Whether or not Scrappy was activated at some point to bypass immunity
    bool scrappy_activated = false;
    // Whether or not Super Luck boosted the critical hit rate for a move
    bool super_luck_activated = false;
    // Whether or not Sniper boosted the critical hit damage multiplier for a move
    bool sniper_activated = false;
    bool stab_boost_activated = false; // Whether or not STAB was activated for a move
    // Whether or not an Electric move was dampened by either Mud Sport or Fog
    bool electric_move_dampened = false;
    // Whether or not Water Sport was activated by a Fire move
    bool water_sport_drop_activated = false;
    bool charge_boost_activated = false; // Whether or not Charge was activated by an Electric move
    // Whether or not a Ghost type's immunity to Normal/Fighting was activated at some point
    bool ghost_immunity_activated = false;
    // Whether or not a defender took less damage due to the Charging Skull Bash status
    bool skull_bash_defense_boost_activated = false;

    // Not actually in the real game, but we need to track these things for the prechecks in
    // ExecuteMoveEffects, since these can block the main damage calculation from even happening
    bool two_turn_move_forced_miss = false;
    bool soundproof_activated = false;
    bool first_hit_check_failed = false;
    bool lightningrod_activated = false;
    bool storm_drain_activated = false;
    // Similarly, the Dream Eater and Last Resort handlers have checks that can block the main
    // damage calculation
    bool dream_eater_failed = false;
    bool last_resort_failed = false;
};

// Encapsulates functionality of the dungeon RNG in a more controlled manner than in-game
class MockDungeonRNG {
  private:
    int32_t crit_chance;
    bool forewarn_active;
    std::optional<int32_t> hit_chance1;
    std::optional<int32_t> hit_chance2;

  public:
    bool huge_pure_power;
    bool critical_hit;
    double variance_dial;

    MockDungeonRNG(bool huge_pure_power_ = false, bool critical_hit_ = false,
                   double variance_dial_ = 0)
        : huge_pure_power(huge_pure_power_), critical_hit(critical_hit_),
          variance_dial(variance_dial_), crit_chance(0), forewarn_active(false),
          hit_chance1(std::nullopt), hit_chance2(std::nullopt) {}

    bool roll_huge_pure_power() const {
        // This is a mock. In-game, it would be:
        // return DungeonRandInt(100) < 33
        return huge_pure_power;
    }

    bool roll_critical_hit(int32_t crit_chance) {
        // This is a mock. In-game, it would be:
        // return DungeonRandInt(100) < crit_chance
        this->crit_chance = crit_chance; // Store this so we can see what it was later
        return critical_hit;
    }
    int32_t get_computed_crit_chance() const { return crit_chance; }

    Fx64 roll_damage_variance() const {
        // This is a mock. In-game, it would be:
        // return Fx64{0, DungeonRandInt(0x4000) + 0xE000}
        uint32_t simulated_rand_outcome =
            std::min(std::max(static_cast<int>(std::round(variance_dial * 0x3FFF)), 0), 0x3FFF);
        return Fx64{0, simulated_rand_outcome + 0xE000};
    }

    bool roll_forewarn() {
        // This is a mock. In-game, it would be:
        // return DungeonRandInt(100) < 20
        forewarn_active = true; // Store this so we know the roll happened later
        return false;
    }
    bool forewarn_was_rolled() const { return forewarn_active; }

    bool roll_hit_chance(int32_t hit_chance, bool accuracy2) {
        // This is a mock. In-game, it would be:
        // return DungeonRandInt(100) < hit_chance

        // Store this so we can see what it was later
        if (accuracy2) {
            hit_chance2 = std::optional<int32_t>{hit_chance};
        } else {
            hit_chance1 = std::optional<int32_t>{hit_chance};
        }
        return true;
    }
    std::optional<int32_t> get_hit_chance1() const { return hit_chance1; }
    std::optional<int32_t> get_hit_chance2() const { return hit_chance2; }
    int32_t get_combined_hit_chance_raw() const {
        return std::min(hit_chance1.value_or(100), 100) * std::min(hit_chance2.value_or(100), 100) *
               (forewarn_active ? 80 : 100);
    }
    double get_combined_hit_probability() const { return get_combined_hit_chance_raw() / 1e6; }
    double get_combined_hit_percentage() const { return get_combined_hit_chance_raw() / 1e4; }
};

// Stand-in for effects on the damage calculation caused by other monsters
struct MockOtherMonsters {
    bool iq_skill_flags[69]; // One for each IQ skill
    bool abilities[124];     // One for each ability

    MockOtherMonsters() {
        std::fill_n(iq_skill_flags, 69, false);
        std::fill_n(abilities, 124, false);
    }
};

// Based on eos::dungeon_generation_info
struct DungeonGenerationInfo {
    eos::fixed_room_id fixed_room_id = eos::FIXED_NONE;
};

// Based on eos::dungeon
struct DungeonState {
    eos::weather_id weather = eos::WEATHER_CLEAR; // Current weather
    uint8_t mud_sport_turns = 0;   // Number of turns left for the Mud Sport condition
    uint8_t water_sport_turns = 0; // Number of turns left for the Water Sport condition
    bool gravity = false;          // Gravity is in effect
    bool plus_is_active[2] = {false,
                              false}; // A monster on the {enemy, team} side has the ability Plus
    bool minus_is_active[2] = {false,
                               false}; // A monster on the {enemy, team} side has the ability Minus
    bool iq_disabled = false;          // IQ skills won't work in the dungeon
    DungeonGenerationInfo gen_info = {};
    DamageCalcDiag damage_calc = {};

    // The actual dungeon RNG uses global state and isn't in the dungeon struct,
    // but this is a convenient place to track it.
    MockDungeonRNG rng = {};
    // The actual code looks up this information by scanning the entity tables and dungeon tiles,
    // but that's out of scope, so this is a more convenient way to encapsulate the necessary info
    MockOtherMonsters other_monsters = {};
};

// Based on eos::move
struct Move {
    eos::move_id id = eos::MOVE_NOTHING;
    uint8_t ginseng = 0;
    uint8_t pp = 0; // only needed for Trump Card and Last Resort

    // Not actually in-game. Only needed for Ice Ball, Rollout, Triple Kick
    uint8_t prior_successive_hits = 0;
    // Not actually in-game. Only needed to support Time/Darkness power differences
    bool time_darkness = false;
};

// Based on eos::item
struct Item {
    bool exists = false; // Validity flag
    bool sticky = false;
    eos::item_id id = eos::ITEM_NOTHING;
};

// Based on eos::monster_stat_modifiers
struct StatModifiers {
    // Stages go from 0-20 inclusive, with normal being 10
    int16_t offensive_stages[2] = {10, 10};  // {atk, sp_atk}
    int16_t defensive_stages[2] = {10, 10};  // {def, sp_def}
    int16_t hit_chance_stages[2] = {10, 10}; // {accuracy, evasion}
    int16_t flash_fire_boost = 0;            // can be 0, 1, or 2
    // Some moves like Screech affect the damage calculation differently than, e.g., Leer
    // {atk, sp_atk}; from Charm, Memento, etc.
    Fx32 offensive_multipliers[2] = {1, 1};
    // {def, sp_def}; from Screech, etc.
    Fx32 defensive_multipliers[2] = {1, 1};
};

// Based on eos::statuses
struct Statuses {
    bool sleep = false;     // sleep group
    bool nightmare = false; // sleep group
    bool napping = false;   // sleep group

    bool burn = false;        // burn group
    bool poison = false;      // burn group
    bool bad_poison = false;  // burn group
    bool paralysis = false;   // burn group
    bool identifying = false; // burn group

    bool confusion = false; // cringe group

    bool skull_bash = false;   // bide group
    bool flying = false;       // bide group
    bool bouncing = false;     // bide group
    bool diving = false;       // bide group
    bool digging = false;      // bide group
    bool charge = false;       // bide group
    bool shadow_force = false; // bide group

    bool reflect = false;      // reflect group
    bool light_screen = false; // reflect group
    bool lucky_chant = false;  // reflect group

    bool gastro_acid = false; // curse group

    bool sure_shot = false;    // sure shot group
    bool whiffer = false;      // sure shot group
    bool focus_energy = false; // sure shot group

    bool cross_eyed = false; // blinded group

    bool miracle_eye = false; // miracle eye group

    bool magnet_rise = false; // magnet rise group

    bool exposed = false;

    // 1 means normal. 0 means half speed. 2, 3, and 4 mean 2x, 3x, and 4x speed.
    int32_t speed_stage = 1;

    uint8_t stockpile_stage = 0;

    // This is a mock for Guts/Marvel Scale. These abilities are activated by a ton of different
    // statuses that don't otherwise matter for the damage calculation. See
    // MonsterEntity::has_negative_status for more details.
    bool other_negative_status = false;

  public:
    // The statuses are stored in "groups" in-game, and only one status per group is allowed.
    // There are also limits on valid values for the stages.
    bool is_valid() const {
        if (static_cast<int>(sleep) + static_cast<int>(nightmare) + static_cast<int>(napping) > 1) {
            return false;
        }
        if (static_cast<int>(burn) + static_cast<int>(poison) + static_cast<int>(bad_poison) +
                static_cast<int>(paralysis) + static_cast<int>(identifying) >
            1) {
            return false;
        }
        if (static_cast<int>(skull_bash) + static_cast<int>(flying) + static_cast<int>(bouncing) +
                static_cast<int>(diving) + static_cast<int>(digging) + static_cast<int>(charge) +
                static_cast<int>(shadow_force) >
            1) {
            return false;
        }
        if (static_cast<int>(reflect) + static_cast<int>(light_screen) +
                static_cast<int>(lucky_chant) >
            1) {
            return false;
        }
        if (static_cast<int>(sure_shot) + static_cast<int>(whiffer) +
                static_cast<int>(focus_energy) >
            1) {
            return false;
        }
        if (speed_stage < 0 || speed_stage > 4) {
            return false;
        }
        if (stockpile_stage > 3) {
            return false;
        }
        return true;
    }
};

// Based on eos::monster
struct Monster {
    eos::monster_id apparent_id = eos::MONSTER_NONE;
    bool is_not_team_member = false; // true for enemies and allied NPCs that aren't on the team
    bool is_team_leader = false;
    uint8_t level = 0;
    int16_t hp = 0;                      // Current HP
    int16_t max_hp_stat = 0;             // Add to max_hp_boost for the actual max HP
    int16_t max_hp_boost = 0;            // From Life Seeds, Sitrus Berries, etc.
    uint8_t offensive_stats[2] = {0, 0}; // {atk, sp_atk}
    uint8_t defensive_stats[2] = {0, 0}; // {def, sp_def}
    int16_t iq = 0;
    StatModifiers stat_modifiers = {};
    eos::type_id types[2] = {eos::TYPE_NONE, eos::TYPE_NONE};
    eos::ability_id abilities[2] = {eos::ABILITY_UNKNOWN, eos::ABILITY_UNKNOWN};
    eos::type_id hidden_power_type = eos::TYPE_NONE;
    int16_t hidden_power_base_power = 0;
    Item held_item = {};
    DecFx16_16 belly = {};
    Statuses statuses = {};
    bool iq_skill_flags[69];               // One for each IQ skill
    bool exclusive_item_effect_flags[129]; // One for each exclusive item effect
    // Stat boosts from exclusive items
    uint8_t exclusive_item_offense_boosts[2] = {0, 0}; // {atk, sp_atk}
    uint8_t exclusive_item_defense_boosts[2] = {0, 0}; // {def, sp_def}
    bool boosted_attacks = false;
    // Set after the monster attacks (true if the attack missed, false otherwise). If true
    // when the monster attacks, Practice Swinger will activate.
    bool practice_swinger_flag = false;
    // Set to true when the monster receives a critical hit. If true when the monster
    // attacks, Anger Point will activate. Set to false after the monster attacks.
    bool anger_point_flag = false;

    // This is a mock for Last Resort. The actual game checks the monster's move array
    uint8_t n_moves_out_of_pp = 0;

    Monster() {
        std::fill_n(iq_skill_flags, 69, false);
        std::fill_n(exclusive_item_effect_flags, 129, false);
    }
};

// Based on eos::entity
class MonsterEntity {
  public:
    Monster monster;

    // pmdsky-debug: EntityIsValid (multiple addresses)
    bool is_valid() const {
        // In-game, this checks whether the entity is valid. Here, it's assumed
        return true;
    }
    // pmdsky-debug: IsMonster (multiple addresses)
    bool is_monster() const {
        // In-game, this checks whether the entity is a monster. Here, it's assumed
        return true;
    }
    bool iq_skill_enabled(eos::iq_skill_id iq, const DungeonState& dungeon) const;
    bool scrappy_should_activate(const MonsterEntity& defender, eos::type_id attack_type,
                                 DungeonState& dungeon) const;
    bool ghost_immunity_active(const MonsterEntity& attacker, int type_idx) const;
    bool no_gastro_acid_status() const;
    bool ability_active(eos::ability_id ability) const;
    bool ability_active(eos::ability_id ability, const MonsterEntity& attacker,
                        bool attacker_ability_active) const;
    bool exclusive_item_effect_active(eos::exclusive_item_effect_id effect) const;
    bool levitate_active(const DungeonState& dungeon) const;
    bool has_conditional_ground_immunity(const DungeonState& dungeon) const;
    bool is_type(eos::type_id t) const;
    eos::weather_id perceived_weather(const DungeonState& dungeon) const;
    bool nearby_ally_iq_skill_enabled(eos::iq_skill_id iq, const DungeonState& dungeon) const;
    bool other_monster_ability_active(eos::ability_id ability, const DungeonState& dungeon) const;
    int32_t exclusive_item_offense_boost(eos::move_category category) const;
    int32_t exclusive_item_defense_boost(eos::move_category category) const;
    bool has_held_item(eos::item_id item) const;
    bool item_active(eos::item_id item) const;
    bool aura_bow_active() const;
    bool has_negative_status(bool check_held_item) const;
    eos::monster_gender gender() const;
    eos::type_id get_move_type(eos::move_id move, const DungeonState& dungeon) const;
    int32_t get_move_power(Move move) const;
    const mechanics::NaturalGiftInfo* natural_gift_info() const;
    eos::type_id weather_ball_type(const DungeonState& dungeon) const;
    bool two_turn_move_forced_miss(eos::move_id move) const;
};

int32_t simulate_damage_calc_generic(DamageData& damage_data, DungeonState& dungeon,
                                     MonsterEntity& attacker, MonsterEntity& defender,
                                     eos::type_id attack_type, int32_t attack_power,
                                     eos::move_id move_id, int32_t crit_chance = 0,
                                     Fx32 damage_mult = 1);
int32_t simulate_damage_calc(DamageData& damage_data, DungeonState& dungeon,
                             MonsterEntity& attacker, MonsterEntity& defender, Move move);
int32_t simulate_damage_calc_projectile(DamageData& damage_data, DungeonState& dungeon,
                                        MonsterEntity& attacker, MonsterEntity& defender,
                                        int32_t attack_power);

#endif
