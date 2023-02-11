// Defines "mechanics" data and utilities that the game references for damage calculation.
// These are values that come from the game's files or from data pools embedded in the game's
// binaries, which are in theory easy to change without changing the core behavior of the damage
// calculation.

#ifndef MECHANICS_HPP_
#define MECHANICS_HPP_

#include <array>
#include <cstdint>
#include <string>
#include "mathutil.hpp"
#include "pmdsky.hpp"

using std::int16_t;
using std::int32_t;

namespace mechanics {
extern const eos::type_matchup TYPE_MATCHUP_TABLE[18][18];
extern const eos::type_matchup TYPE_MATCHUP_COMBINATOR_TABLE[4][4];

extern const Fx32 MATCHUP_IMMUNE_MULTIPLIER_ERRATIC_PLAYER;
extern const Fx32 MATCHUP_NOT_VERY_EFFECTIVE_MULTIPLIER_ERRATIC_PLAYER;
extern const Fx32 MATCHUP_NEUTRAL_MULTIPLIER_ERRATIC_PLAYER;
extern const Fx32 MATCHUP_SUPER_EFFECTIVE_MULTIPLIER_ERRATIC_PLAYER;
extern const Fx32 MATCHUP_IMMUNE_MULTIPLIER;
extern const Fx32 MATCHUP_NOT_VERY_EFFECTIVE_MULTIPLIER;
extern const Fx32 MATCHUP_NEUTRAL_MULTIPLIER;
extern const Fx32 MATCHUP_SUPER_EFFECTIVE_MULTIPLIER;

extern const eos::damage_negating_exclusive_eff_entry
    TYPE_DAMAGE_NEGATING_EXCLUSIVE_ITEM_EFFECTS[28];
extern const eos::exclusive_item_effect_id EXCL_ITEM_EFFECTS_EVASION_BOOST[8];

extern const int16_t STICK_POWER;
extern const int16_t IRON_THORN_POWER;
extern const int16_t SILVER_SPIKE_POWER;
extern const int16_t GOLD_FANG_POWER;
extern const int16_t CACNEA_SPIKE_POWER;
extern const int16_t CORSOLA_TWIG_POWER;
extern const int16_t GOLD_THORN_POWER;

extern const Fx32 DIG_DAMAGE_MULTIPLIER;
extern const Fx32 RAZOR_WIND_DAMAGE_MULTIPLIER;
extern const Fx32 FACADE_DAMAGE_MULTIPLIER;
extern const Fx32 FOCUS_PUNCH_DAMAGE_MULTIPLIER;
extern const Fx32 SKY_ATTACK_DAMAGE_MULTIPLIER;
extern const Fx32 SOLARBEAM_DAMAGE_MULTIPLIER;
extern const Fx32 FLY_DAMAGE_MULTIPLIER;
extern const Fx32 DIVE_DAMAGE_MULTIPLIER;
extern const Fx32 BOUNCE_DAMAGE_MULTIPLIER;
extern const Fx32 SKULL_BASH_DAMAGE_MULTIPLIER;
extern const Fx32 SHADOW_FORCE_DAMAGE_MULTIPLIER;
extern const Fx32 ROLLOUT_DAMAGE_MULT_TABLE[10];
extern const Fx32 REVERSAL_DAMAGE_MULT_TABLE[4];
extern const Fx32 WATER_SPOUT_DAMAGE_MULT_TABLE[4];
extern const Fx32 ERUPTION_DAMAGE_MULT_TABLE[4];
extern const Fx32 WRING_OUT_DAMAGE_MULT_TABLE[4];
extern const Fx32 LAST_RESORT_DAMAGE_MULT_TABLE[4];
extern const Fx32 WEATHER_BALL_DAMAGE_MULT_TABLE[8];
extern const eos::type_id WEATHER_BALL_TYPE_TABLE[8];

extern const Fx32 OFFENSIVE_STAT_STAGE_MULTIPLIERS[21];
extern const Fx32 DEFENSIVE_STAT_STAGE_MULTIPLIERS[21];
extern const Fx32 MALE_ACCURACY_STAGE_MULTIPLIERS[21];
extern const Fx32 MALE_EVASION_STAGE_MULTIPLIERS[21];
extern const Fx32 FEMALE_ACCURACY_STAGE_MULTIPLIERS[21];
extern const Fx32 FEMALE_EVASION_STAGE_MULTIPLIERS[21];

extern const Fx32 TINTED_LENS_MULTIPLIER;
extern const Fx64 SOLID_ROCK_MULTIPLIER;
extern const Fx32 BURN_DAMAGE_MULTIPLIER;
extern const Fx64 CLOUDY_DAMAGE_MULTIPLIER;
extern const Fx32 ME_FIRST_MULTIPLIER;
extern const Fx32 POWER_PITCHER_DAMAGE_MULTIPLIER;
extern const Fx32 AIR_BLADE_DAMAGE_MULTIPLIER;

extern const int16_t POWER_BAND_STAT_BOOST;
extern const int16_t SPECIAL_BAND_STAT_BOOST;
extern const int16_t DEF_SCARF_STAT_BOOST;
extern const int16_t ZINC_BAND_STAT_BOOST;
extern const int16_t AURA_BOW_STAT_BOOST;
extern const int16_t MUNCH_BELT_STAT_BOOST;
extern const int16_t SCOPE_LENS_CRIT_RATE_BOOST;
extern const int16_t SUPER_LUCK_CRIT_RATE_BOOST;
extern const int16_t TYPE_ADVANTAGE_MASTER_CRIT_RATE;
extern const int32_t DETECT_BAND_MOVE_ACCURACY_DROP;
extern const int32_t QUICK_DODGER_MOVE_ACCURACY_DROP;

extern const int16_t TECHNICIAN_MOVE_POWER_THRESHOLD;
extern const int32_t OFFENSE_STAT_MAX;
extern const int32_t MAX_HP_CAP;

// Based on eos::natural_gift_item_info
struct NaturalGiftInfo {
    eos::item_id item_id;
    eos::type_id type_id;
    // This value is the boost that's added to Natural Gift's default base power of 1
    // during damage calculation
    int16_t base_power_boost;
};
extern const NaturalGiftInfo NATURAL_GIFT_ITEM_TABLE[34];

// pmdsky-debug: IsTypeIneffectiveAgainstGhost ([NA] 0x230ABF0)
inline bool type_ineffective_against_ghost(eos::type_id type) {
    return type == eos::TYPE_NORMAL || type == eos::TYPE_FIGHTING;
}

eos::type_id get_move_type(eos::move_id move);
eos::move_category get_move_category(eos::move_id move);
int16_t get_move_base_power(eos::move_id move, bool time_darkness = false);
uint8_t get_move_accuracy(eos::move_id move, bool accuracy2);
uint8_t get_move_crit_chance(eos::move_id move);
uint8_t get_move_max_pp(eos::move_id move);
bool is_recoil_move(eos::move_id move);
bool is_punch_move(eos::move_id move);
bool is_sound_move(eos::move_id move);
// pmdsky-debug: MoveIsNotPhysical ([NA] 0x231EA18)
inline bool move_not_physical(eos::move_id move) {
    return get_move_category(move) != eos::CATEGORY_PHYSICAL;
}
// pmdsky-debug: IsRegularAttackOrProjectile ([NA] 0x2014CEC)
inline bool is_regular_attack_or_projectile(eos::move_id move) {
    return move == eos::MOVE_REGULAR_ATTACK || move == eos::MOVE_PROJECTILE;
}

// pmdsky-debug: IsAuraBow ([NA] 0x200CC14)
inline bool is_aura_bow(eos::item_id item) {
    return item >= eos::ITEM_SILVER_BOW && item <= eos::ITEM_FUCHSIA_BOW;
}

eos::monster_gender get_monster_gender(eos::monster_id monster);
Fx32 get_monster_weight(eos::monster_id monster);

// Defines all the intrinsic info about a given move that should never change
class MoveSpec {
  public:
    eos::move_id id;
    eos::type_id type;
    int32_t power;
    int32_t crit_chance;
    uint8_t pp;
    bool unsupported;
    std::string special_notes;

    MoveSpec(eos::type_id type_, int32_t power_, int32_t crit_chance_ = 0)
        : id(eos::MOVE_NOTHING), type(type_), power(power_), crit_chance(crit_chance_), pp(0),
          unsupported(false), special_notes("User-defined move") {}
    MoveSpec(eos::move_id move_id = eos::MOVE_NOTHING);
};
// Defines all the intrinsic info about a given species that should never change
class SpeciesSpec {
  private:
    // (Ab)use the reserve IDs to encode custom monsters of the different genders
    static const eos::monster_id MONSTER_ID_CUSTOM_MALE = eos::MONSTER_RESERVE_43;
    static const eos::monster_id MONSTER_ID_CUSTOM_FEMALE = eos::MONSTER_RESERVE_44;
    static const eos::monster_id MONSTER_ID_CUSTOM_GENDERLESS = eos::MONSTER_RESERVE_45;

  public:
    eos::monster_id id;
    std::array<eos::type_id, 2> types;
    std::array<eos::ability_id, 2> abilities;
    // uint16_t weight; // TODO?

    SpeciesSpec(std::array<eos::type_id, 2> types_, std::array<eos::ability_id, 2> abilities_,
                eos::monster_gender gender = eos::GENDER_GENDERLESS)
        : types(types_), abilities(abilities_) {
        switch (gender) {
        case eos::GENDER_MALE:
            id = MONSTER_ID_CUSTOM_MALE;
            break;
        case eos::GENDER_FEMALE:
            id = MONSTER_ID_CUSTOM_FEMALE;
            break;
        case eos::GENDER_INVALID: // Treat invalid like genderless
        case eos::GENDER_GENDERLESS:
            id = MONSTER_ID_CUSTOM_GENDERLESS;
            break;
        }
    }
    SpeciesSpec(eos::monster_id monster_id = eos::MONSTER_NONE);

    // Decode the custom monster IDs of different genders
    static eos::monster_gender get_custom_species_gender(eos::monster_id monster_id) {
        switch (monster_id) {
        case MONSTER_ID_CUSTOM_MALE:
            return eos::GENDER_MALE;
        case MONSTER_ID_CUSTOM_FEMALE:
            return eos::GENDER_FEMALE;
        case MONSTER_ID_CUSTOM_GENDERLESS:
            return eos::GENDER_GENDERLESS;
        default:
            return eos::GENDER_INVALID;
        }
    }
};

// Data that the game loads from the file system
namespace data_files {
struct MoveData {
    eos::move_id id;
    int16_t base_power;
    eos::type_id type;
    eos::move_category category;
    uint8_t pp;
    uint8_t accuracy1;
    uint8_t accuracy2;
    uint8_t strikes;
    uint8_t crit_chance;
    bool unsupported;          // Not actually in the data files
    std::string special_notes; // Not actually in the data files
};
extern const MoveData MOVES[559];

struct MonsterData {
    eos::monster_id id;
    eos::monster_gender gender;
    eos::type_id type1;
    eos::type_id type2;
    eos::ability_id ability1;
    eos::ability_id ability2;
    uint16_t weight;
    uint16_t size;
};
extern const MonsterData MONSTERS[1155];
}; // namespace data_files
}; // namespace mechanics

#endif
