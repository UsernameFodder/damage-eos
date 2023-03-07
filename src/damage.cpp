#include <limits>
#include "damage.hpp"

// pmdsky-debug: IqSkillIsEnabled ([NA] 0x2301F80)
bool MonsterEntity::iq_skill_enabled(eos::iq_skill_id iq, const DungeonState& dungeon) const {
    if (!monster.is_not_team_member && dungeon.iq_disabled) {
        return false;
    }
    return monster.iq_skill_flags[iq];
}
// pmdsky-debug: ScrappyShouldActivate ([NA] 0x230AB58)
bool MonsterEntity::scrappy_should_activate(const MonsterEntity& defender, eos::type_id attack_type,
                                            DungeonState& dungeon) const {
    for (int i = 0; i < 1; i++) {
        if (ability_active(eos::ABILITY_SCRAPPY) && defender.monster.types[i] == eos::TYPE_GHOST &&
            mechanics::type_ineffective_against_ghost(attack_type)) {
            dungeon.damage_calc.scrappy_activated = true;
            return true;
        }
    }
    return false;
}
// pmdsky-debug: GhostImmunityIsActive ([NA] 0x230AC04)
bool MonsterEntity::ghost_immunity_active(const MonsterEntity& attacker, int type_idx) const {
    return monster.types[type_idx] == eos::TYPE_GHOST && !monster.statuses.exposed &&
           !attacker.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_SCRAPPY);
}
// pmdsky-debug: NoGastroAcidStatus ([NA] 0x2301CDC)
bool MonsterEntity::no_gastro_acid_status() const {
    return is_monster() && !monster.statuses.gastro_acid;
}
// pmdsky-debug: AbilityIsActive ([NA] 0x2301D10)
bool MonsterEntity::ability_active(eos::ability_id ability) const {
    if (!is_monster() || ability == eos::ABILITY_UNKNOWN) {
        return false;
    }
    return (monster.abilities[0] == ability || monster.abilities[1] == ability) &&
           no_gastro_acid_status();
}
// pmdsky-debug: DefenderAbilityIsActive (multiple addresses)
bool MonsterEntity::ability_active(eos::ability_id ability, const MonsterEntity& attacker,
                                   bool attacker_ability_active) const {
    if (this != &attacker && attacker.is_monster() && attacker_ability_active &&
        attacker.ability_active(eos::ABILITY_MOLD_BREAKER)) {
        return false;
    }
    return ability_active(ability);
}
// pmdsky-debug: ExclusiveItemEffectIsActive (multiple addresses)
bool MonsterEntity::exclusive_item_effect_active(eos::exclusive_item_effect_id effect) const {
    return !monster.is_not_team_member && monster.exclusive_item_effect_flags[effect];
}
// pmdsky-debug: LevitateIsActive ([NA] 0x2301E18)
bool MonsterEntity::levitate_active(const DungeonState& dungeon) const {
    return !dungeon.gravity && ability_active(eos::ABILITY_LEVITATE);
}
// pmdsky-debug: HasConditionalGroundImmunity ([NA] 0x2318A4C)
bool MonsterEntity::has_conditional_ground_immunity(const DungeonState& dungeon) const {
    return is_valid() && !dungeon.gravity && monster.statuses.magnet_rise;
}
// pmdsky-debug: MonsterIsType ([NA] 0x2301E50)
bool MonsterEntity::is_type(eos::type_id t) const {
    return t != eos::TYPE_NONE && (monster.types[0] == t || monster.types[1] == t);
}
// pmdsky-debug: GetApparentWeather ([NA] 0x2334D08)
eos::weather_id MonsterEntity::perceived_weather(const DungeonState& dungeon) const {
    if (item_active(eos::ITEM_WEATHER_BAND)) {
        return eos::WEATHER_CLEAR;
    }
    return dungeon.weather;
}
// pmdsky-debug: NearbyAllyIqSkillIsEnabled ([NA] 0x2338288)
bool MonsterEntity::nearby_ally_iq_skill_enabled(eos::iq_skill_id iq,
                                                 const DungeonState& dungeon) const {
    // This implementation is a mock. The actual game checks if there are any monsters on the
    // 8 surrounding tiles that have the relevant IQ skill enabled.
    return dungeon.other_monsters.iq_skill_flags[iq];
}
// pmdsky-debug: OtherMonsterAbilityIsActive ([NA] 0x2301D84)
bool MonsterEntity::other_monster_ability_active(eos::ability_id ability,
                                                 const DungeonState& dungeon) const {
    // This implementation is a mock. The actual game checks if any other monster (possibly
    // needs to be in range? not sure) has some ability active when subject to this monster
    // (i.e., Mold Breaker has an effect).
    return ability != eos::ABILITY_UNKNOWN && !ability_active(eos::ABILITY_MOLD_BREAKER) &&
           dungeon.other_monsters.abilities[ability];
}
// pmdsky-debug: ExclusiveItemOffenseBoost ([NA] 0x230F778)
int32_t MonsterEntity::exclusive_item_offense_boost(eos::move_category category) const {
    return monster.exclusive_item_offense_boosts[category];
}
// pmdsky-debug: ExclusiveItemDefenseBoost ([NA] 0x230F788)
int32_t MonsterEntity::exclusive_item_defense_boost(eos::move_category category) const {
    return monster.exclusive_item_defense_boosts[category];
}
// pmdsky-debug: HasHeldItem ([NA] 0x23467E4)
bool MonsterEntity::has_held_item(eos::item_id item) const {
    return monster.held_item.exists && !monster.held_item.sticky && monster.held_item.id == item;
}
// pmdsky-debug: ItemIsActive (multiple addresses)
bool MonsterEntity::item_active(eos::item_id item) const {
    return !ability_active(eos::ABILITY_KLUTZ) && has_held_item(item);
}
// pmdsky-debug: AuraBowIsActive ([NA] 0x230F6C8)
bool MonsterEntity::aura_bow_active() const {
    return !ability_active(eos::ABILITY_KLUTZ) && monster.held_item.exists &&
           mechanics::is_aura_bow(monster.held_item.id) && has_held_item(monster.held_item.id);
}
// pmdsky-debug: MonsterHasNegativeStatus ([NA] 0x2300634)
bool MonsterEntity::has_negative_status(bool check_held_item) const {
    // This is a mock. The actual game checks loads of individual things, which are condensed
    // down here since the details don't matter. See the actual in-game function or
    // pmdsky-debug for more details.

    // The game doesn't actually check for speed being down this way. It instead checks
    // the all the speed_down_counters for any non-zero value. This isn't necessarily
    // equivalent; a monster could have equally many or more speed_up_counters set
    // and have a non-zero speed stage while still triggering the "negative status" check.
    // This case is considered to be encapsulated by monster.statuses.other_negative_status.
    bool speed_down = monster.statuses.speed_stage == 0;

    // Not 100% sure about the Identifying status...
    return monster.statuses.sleep || monster.statuses.nightmare || monster.statuses.burn ||
           monster.statuses.poison || monster.statuses.bad_poison || monster.statuses.paralysis ||
           monster.statuses.identifying || monster.statuses.confusion ||
           monster.statuses.gastro_acid || monster.statuses.whiffer ||
           (check_held_item && !monster.is_team_leader && item_active(eos::ITEM_Y_RAY_SPECS)) ||
           monster.statuses.cross_eyed || monster.statuses.miracle_eye ||
           monster.statuses.exposed || speed_down || monster.statuses.other_negative_status;
}
// pmdsky-debug: GetMonsterGender ([NA] 0x20527A8)
eos::monster_gender MonsterEntity::gender() const {
    return mechanics::get_monster_gender(monster.apparent_id);
}
// pmdsky-debug: GetMoveTypeForMonster ([NA] 0x230227C)
eos::type_id MonsterEntity::get_move_type(eos::move_id move, const DungeonState& dungeon) const {
    if (!mechanics::is_regular_attack_or_projectile(move) && is_valid() &&
        ability_active(eos::ABILITY_NORMALIZE)) {
        return eos::TYPE_NORMAL;
    }
    if (move == eos::MOVE_HIDDEN_POWER) {
        return monster.hidden_power_type;
    }
    if (move == eos::MOVE_NATURAL_GIFT) {
        auto ng_info = natural_gift_info();
        if (ng_info) {
            return ng_info->type_id;
        }
    }
    if (move == eos::MOVE_WEATHER_BALL) {
        return weather_ball_type(dungeon);
    }
    return mechanics::get_move_type(move);
}
// pmdsky-debug: GetMovePower ([NA] 0x230231C)
int32_t MonsterEntity::get_move_power(Move move) const {
    if (move.id == eos::MOVE_HIDDEN_POWER) {
        return static_cast<int32_t>(move.ginseng) + monster.hidden_power_base_power;
    }
    int32_t power = static_cast<int32_t>(move.ginseng) +
                    mechanics::get_move_base_power(move.id, move.time_darkness);
    if (item_active(eos::ITEM_SPACE_GLOBE)) {
        power *= 2;
    }
    return power;
}
// pmdsky-debug: GetEntityNaturalGiftInfo ([NA] 0x231AE90)
const mechanics::NaturalGiftInfo* MonsterEntity::natural_gift_info() const {
    if (!is_monster() || monster.held_item.id == eos::ITEM_NOTHING) {
        return nullptr;
    }
    for (int i = 0; mechanics::NATURAL_GIFT_ITEM_TABLE[i].item_id != eos::ITEM_NOTHING; i++) {
        if (monster.held_item.id == mechanics::NATURAL_GIFT_ITEM_TABLE[i].item_id) {
            return &mechanics::NATURAL_GIFT_ITEM_TABLE[i];
        }
    }
    return nullptr;
}
// pmdsky-debug: GetEntityWeatherBallType ([NA] 0x231AF0C)
eos::type_id MonsterEntity::weather_ball_type(const DungeonState& dungeon) const {
    return mechanics::WEATHER_BALL_TYPE_TABLE[perceived_weather(dungeon)];
}
// pmdsky-debug: TwoTurnMoveForcedMiss ([NA] 0x2324854)
bool MonsterEntity::two_turn_move_forced_miss(eos::move_id move) const {
    // The actual game checks the monster_statuses::two_turn_move_invincible flag, not the status
    // flags for flying/bouncing, but this is probably equivalent
    if (monster.statuses.flying || monster.statuses.bouncing) {
        return move != eos::MOVE_SKY_UPPERCUT && move != eos::MOVE_TWISTER &&
               move != eos::MOVE_GUST && move != eos::MOVE_THUNDER;
    }
    // If the two_turn_move_invincible flag is not 2, the game returns false here. The following
    // status checks are probably equivalent.
    if (!monster.statuses.diving && !monster.statuses.digging && !monster.statuses.shadow_force) {
        return false;
    }
    if (monster.statuses.diving && (move == eos::MOVE_WHIRLPOOL || move == eos::MOVE_SURF)) {
        return false;
    } else if (monster.statuses.digging) {
        if (move == eos::MOVE_EARTHQUAKE || move == eos::MOVE_MAGNITUDE) {
            return false;
        }
        // There's also a check for whether Nature Power turns into Earthquake here
    }
    return true;
}

// pmdsky-debug: GetTypeMatchup ([NA] 0x230AC58)
eos::type_matchup get_type_matchup(const DungeonState& dungeon, const MonsterEntity& attacker,
                                   const MonsterEntity& defender, int target_type_idx,
                                   eos::type_id attack_type) {
    if ((defender.monster.statuses.miracle_eye ||
         attacker.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_MIRACLE_EYE)) &&
        attack_type == eos::TYPE_PSYCHIC &&
        defender.monster.types[target_type_idx] == eos::TYPE_DARK) {
        return eos::MATCHUP_NEUTRAL;
    }
    if (attack_type == eos::TYPE_GROUND) {
        if (dungeon.gravity) {
            if (defender.monster.types[target_type_idx] == eos::TYPE_FLYING) {
                return eos::MATCHUP_NEUTRAL;
            }
        } else if (defender.has_conditional_ground_immunity(dungeon)) {
            return eos::MATCHUP_IMMUNE;
        }
    }
    return mechanics::TYPE_MATCHUP_TABLE[attack_type][defender.monster.types[target_type_idx]];
}

// pmdsky-debug: FlashFireShouldActivate ([NA] 0x2313C74)
int16_t flash_fire_should_activate(const MonsterEntity& attacker, const MonsterEntity& defender) {
    if (!defender.is_valid()) {
        return 0;
    }
    if (attacker.ability_active(eos::ABILITY_NORMALIZE) ||
        !defender.ability_active(eos::ABILITY_FLASH_FIRE, attacker, true)) {
        return 0;
    }
    return (defender.monster.stat_modifiers.flash_fire_boost < 2) ? 2 : 1;
}

// pmdsky-debug: GendersEqualNotGenderless ([NA] 0x2054E78)
bool genders_equal_not_genderless(eos::monster_id monster1, eos::monster_id monster2) {
    eos::monster_gender gender1 = mechanics::get_monster_gender(monster1);
    eos::monster_gender gender2 = mechanics::get_monster_gender(monster2);
    if (gender1 == eos::GENDER_GENDERLESS || gender2 == eos::GENDER_GENDERLESS) {
        return false;
    }
    return gender1 == gender2;
}

// pmdsky-debug: CalcTypeBasedDamageEffects ([NA] 0x230AD04)
bool calc_type_based_damage_effects(DungeonState& dungeon, Fx64& damage_mult_out,
                                    const MonsterEntity& attacker, const MonsterEntity& defender,
                                    int32_t attack_power, eos::type_id attack_type,
                                    DamageData& damage_out, bool partial)

{
    damage_mult_out = Fx64{1};
    damage_out.critical_hit = false;
    damage_out.full_type_immunity = false;
    if (!defender.is_valid()) {
        return true;
    }

    damage_out.type_matchup = eos::MATCHUP_NEUTRAL;
    // TODO: these two values are initialized to some default values...what are they?
    // It should never affect the output of the function though.
    eos::type_matchup type_matchups[2] = {};
    for (int i = 0; i < 2; i++) {
        Fx64 matchup_multipliers[4];
        if (!partial && (attacker.iq_skill_enabled(eos::IQ_ERRATIC_PLAYER, dungeon) ||
                         defender.iq_skill_enabled(eos::IQ_ERRATIC_PLAYER, dungeon))) {
            matchup_multipliers[0] = Fx64{mechanics::MATCHUP_IMMUNE_MULTIPLIER_ERRATIC_PLAYER};
            matchup_multipliers[1] =
                Fx64{mechanics::MATCHUP_NOT_VERY_EFFECTIVE_MULTIPLIER_ERRATIC_PLAYER};
            matchup_multipliers[2] = Fx64{mechanics::MATCHUP_NEUTRAL_MULTIPLIER_ERRATIC_PLAYER};
            matchup_multipliers[3] =
                Fx64{mechanics::MATCHUP_SUPER_EFFECTIVE_MULTIPLIER_ERRATIC_PLAYER};
        } else {
            matchup_multipliers[0] = Fx64{mechanics::MATCHUP_IMMUNE_MULTIPLIER};
            matchup_multipliers[1] = Fx64{mechanics::MATCHUP_NOT_VERY_EFFECTIVE_MULTIPLIER};
            matchup_multipliers[2] = Fx64{mechanics::MATCHUP_NEUTRAL_MULTIPLIER};
            matchup_multipliers[3] = Fx64{mechanics::MATCHUP_SUPER_EFFECTIVE_MULTIPLIER};
        }

        if (damage_mult_out == 0) {
            break;
        }

        eos::type_matchup matchup;
        if (!attacker.scrappy_should_activate(defender, attack_type, dungeon) &&
            mechanics::type_ineffective_against_ghost(attack_type) &&
            defender.ghost_immunity_active(attacker, i)) {
            matchup = eos::MATCHUP_IMMUNE;
            dungeon.damage_calc.ghost_immunity_activated = true;
        } else {
            matchup = get_type_matchup(dungeon, attacker, defender, i, attack_type);
        }
        if (attacker.iq_skill_enabled(eos::IQ_ERRATIC_PLAYER, dungeon) ||
            matchup != eos::MATCHUP_NEUTRAL) {
            damage_mult_out *= matchup_multipliers[matchup];
        }
        type_matchups[i] = matchup;
    }

    dungeon.damage_calc.move_indiv_type_matchups[0] = type_matchups[0];
    dungeon.damage_calc.move_indiv_type_matchups[1] = type_matchups[1];
    damage_out.type_matchup =
        mechanics::TYPE_MATCHUP_COMBINATOR_TABLE[type_matchups[0]][type_matchups[1]];

    bool super_effective = (damage_out.type_matchup == eos::MATCHUP_SUPER_EFFECTIVE);
    if (!super_effective) {
        if (defender.ability_active(eos::ABILITY_WONDER_GUARD, attacker, true) &&
            attack_type != eos::TYPE_NONE) {
            damage_mult_out = Fx64{0};
        }
    }

    if (attacker.ability_active(eos::ABILITY_TINTED_LENS) &&
        damage_out.type_matchup == eos::MATCHUP_NOT_VERY_EFFECTIVE) {
        damage_mult_out *= Fx64{mechanics::TINTED_LENS_MULTIPLIER};
    }

    if ((defender.ability_active(eos::ABILITY_SOLID_ROCK, attacker, true) ||
         defender.ability_active(eos::ABILITY_FILTER, attacker, true)) &&
        (damage_out.type_matchup == eos::MATCHUP_SUPER_EFFECTIVE)) {
        damage_mult_out *= mechanics::SOLID_ROCK_MULTIPLIER;
    }

    if (defender.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_HALVED_DAMAGE)) {
        damage_mult_out *= Fx64::CONST_0_5;
    }

    if (!partial && attacker.ability_active(eos::ABILITY_TECHNICIAN) &&
        attack_power <= mechanics::TECHNICIAN_MOVE_POWER_THRESHOLD) {
        damage_mult_out *= Fx64::CONST_1_5;
    }

    if ((attack_type == eos::TYPE_FIRE || attack_type == eos::TYPE_ICE) &&
        defender.ability_active(eos::ABILITY_THICK_FAT, attacker, true)) {
        dungeon.damage_calc.fire_move_ability_drop_activated = true;
        damage_mult_out *= Fx64::CONST_0_5;
    }

    if (attack_type == eos::TYPE_FIRE && flash_fire_should_activate(attacker, defender)) {
        dungeon.damage_calc.flash_fire_activated = true;
        damage_mult_out = Fx64{0};
        super_effective = false;
        damage_out.type_matchup = eos::MATCHUP_IMMUNE;
        damage_out.critical_hit = false;
        damage_out.full_type_immunity = true;
    }

    if (attack_type == eos::TYPE_FIRE &&
        defender.ability_active(eos::ABILITY_HEATPROOF, attacker, true)) {
        dungeon.damage_calc.fire_move_ability_drop_activated = true;
        damage_mult_out *= Fx64::CONST_0_5;
    }

    if (attack_type == eos::TYPE_GROUND && ((!attacker.ability_active(eos::ABILITY_MOLD_BREAKER) &&
                                             defender.levitate_active(dungeon)) ||
                                            defender.has_conditional_ground_immunity(dungeon))) {
        damage_mult_out = Fx64{0};
        super_effective = false;
        damage_out.type_matchup = eos::MATCHUP_IMMUNE;
        damage_out.critical_hit = false;
        damage_out.full_type_immunity = true;
    }

    if (attack_type == eos::TYPE_WATER && attacker.ability_active(eos::ABILITY_TORRENT)) {
        int32_t max_hp = attacker.monster.max_hp_stat + attacker.monster.max_hp_boost;
        if (max_hp > mechanics::MAX_HP_CAP) {
            max_hp = mechanics::MAX_HP_CAP;
        }
        if (attacker.monster.hp <= div4_trunc(max_hp)) {
            dungeon.damage_calc.torrent_boost_activated = true;
            damage_mult_out *= Fx64{2};
        }
    }
    if (attack_type == eos::TYPE_GRASS && attacker.ability_active(eos::ABILITY_OVERGROW)) {
        int32_t max_hp = attacker.monster.max_hp_stat + attacker.monster.max_hp_boost;
        if (max_hp > mechanics::MAX_HP_CAP) {
            max_hp = mechanics::MAX_HP_CAP;
        }
        if (attacker.monster.hp <= div4_trunc(max_hp)) {
            dungeon.damage_calc.overgrow_boost_activated = true;
            damage_mult_out *= Fx64{2};
        }
    }
    if (attack_type == eos::TYPE_BUG && attacker.ability_active(eos::ABILITY_SWARM)) {
        int32_t max_hp = attacker.monster.max_hp_stat + attacker.monster.max_hp_boost;
        if (max_hp > mechanics::MAX_HP_CAP) {
            max_hp = mechanics::MAX_HP_CAP;
        }
        if (attacker.monster.hp <= div4_trunc(max_hp)) {
            dungeon.damage_calc.swarm_boost_activated = true;
            damage_mult_out *= Fx64{2};
        }
    }
    if (attack_type == eos::TYPE_FIRE) {
        if (attacker.ability_active(eos::ABILITY_BLAZE)) {
            int32_t max_hp = attacker.monster.max_hp_stat + attacker.monster.max_hp_boost;
            if (max_hp > mechanics::MAX_HP_CAP) {
                max_hp = mechanics::MAX_HP_CAP;
            }
            if (attacker.monster.hp <= div4_trunc(max_hp)) {
                dungeon.damage_calc.fire_move_ability_boost_activated = true;
                damage_mult_out *= Fx64{2};
            }
        }

        if (defender.ability_active(eos::ABILITY_DRY_SKIN, attacker, true)) {
            dungeon.damage_calc.fire_move_ability_boost_activated = true;
            damage_mult_out *= Fx64::CONST_1_5;
        }
    }

    if (attacker.monster.statuses.burn) {
        damage_mult_out *= Fx64{mechanics::BURN_DAMAGE_MULTIPLIER};
    }

    if (damage_mult_out != 0 && attacker.is_type(attack_type)) {
        dungeon.damage_calc.stab_boost_activated = true;
        if (attacker.ability_active(eos::ABILITY_ADAPTABILITY)) {
            damage_mult_out *= 2;
        } else {
            damage_mult_out *= Fx64::CONST_1_5;
        }
    }

    eos::weather_id weather = attacker.perceived_weather(dungeon);
    if (weather == eos::WEATHER_SUNNY) {
        if (attack_type == eos::TYPE_FIRE) {
            dungeon.damage_calc.sunny_multiplier_activated = true;
            damage_mult_out *= Fx64::CONST_1_5;
        } else if (attack_type == eos::TYPE_WATER) {
            dungeon.damage_calc.sunny_multiplier_activated = true;
            damage_mult_out *= Fx64::CONST_0_5;
        }
    }
    if (weather == eos::WEATHER_RAIN) {
        if (attack_type == eos::TYPE_FIRE) {
            dungeon.damage_calc.rain_multiplier_activated = true;
            damage_mult_out *= Fx64::CONST_0_5;
        } else if (attack_type == eos::TYPE_WATER) {
            dungeon.damage_calc.rain_multiplier_activated = true;
            damage_mult_out *= Fx64::CONST_1_5;
        }
    }
    if (weather == eos::WEATHER_CLOUDY && attack_type != eos::TYPE_NORMAL) {
        damage_mult_out *= mechanics::CLOUDY_DAMAGE_MULTIPLIER;
        dungeon.damage_calc.cloudy_drop_activated = true;
    }
    if ((dungeon.mud_sport_turns != 0 || weather == eos::WEATHER_FOG) &&
        attack_type == eos::TYPE_ELECTRIC) {
        dungeon.damage_calc.electric_move_dampened = true;
        damage_mult_out *= Fx64::CONST_0_5;
    }
    if (dungeon.water_sport_turns != 0 && attack_type == eos::TYPE_FIRE) {
        dungeon.damage_calc.water_sport_drop_activated = true;
        damage_mult_out *= Fx64::CONST_0_5;
    }

    if (attack_type == eos::TYPE_ELECTRIC && attacker.monster.statuses.charge) {
        dungeon.damage_calc.charge_boost_activated = true;
        damage_mult_out *= Fx64{2};
    }

    return super_effective;
}

// pmdsky-debug: CalcDamage ([NA] 0x230BBAC)
void calc_damage(DungeonState& dungeon, const MonsterEntity& attacker, MonsterEntity& defender,
                 eos::type_id attack_type, int32_t attack_power, int32_t crit_chance,
                 DamageData& damage_out, Fx32 damage_mult, eos::move_id move_id, bool full_calc)

{
    damage_out = DamageData{};
    if (attacker.monster.me_first_flag) {
        damage_mult *= mechanics::ME_FIRST_MULTIPLIER;
    }

    if (attacker.ability_active(eos::ABILITY_RECKLESS) && mechanics::is_recoil_move(move_id)) {
        damage_mult = (damage_mult * 3) / 2;
    }

    if (attacker.ability_active(eos::ABILITY_IRON_FIST) && mechanics::is_punch_move(move_id)) {
        damage_mult *= Fx32::CONST_1_5;
    }

    if (attacker.ability_active(eos::ABILITY_NORMALIZE)) {
        attack_type = eos::TYPE_NORMAL;
    }
    if (move_id == eos::MOVE_JUDGMENT) {
        attack_type = attacker.monster.types[0];
    }

    bool not_physical = mechanics::move_not_physical(move_id);
    dungeon.damage_calc = DamageCalcDiag{};

    if ((!attacker.monster.is_team_leader && attacker.monster.belly.ceil() == 0) ||
        (move_id == eos::MOVE_REGULAR_ATTACK &&
         defender.ability_active(eos::ABILITY_WONDER_GUARD, attacker, true))) {
        damage_out.damage = 1;
        damage_out.damage_message = eos::DAMAGE_MESSAGE_MOVE;
        damage_out.type_matchup = eos::MATCHUP_NEUTRAL;
        damage_out.type = attack_type;
        damage_out.category = mechanics::get_move_category(move_id);
        damage_out.critical_hit = false;
        damage_out.full_type_immunity = false;
        damage_out.no_damage = false;
        return;
    }

    damage_out.type = attack_type;
    damage_out.category = mechanics::get_move_category(move_id);

    int32_t atk_stage_boost = 0;

    dungeon.damage_calc.move_type = attack_type;
    eos::move_category move_category =
        (not_physical ? eos::CATEGORY_SPECIAL : eos::CATEGORY_PHYSICAL);
    dungeon.damage_calc.move_category = move_category;

    Fx32 atk_stage_mult = attacker.monster.stat_modifiers.offensive_multipliers[move_category];
    int32_t def_stage = 0;
    Fx32 def_stage_mult = defender.monster.stat_modifiers.defensive_multipliers[move_category];
    if (attacker.ability_active(eos::ABILITY_DOWNLOAD)) {
        if (defender.monster.defensive_stats[0] < defender.monster.defensive_stats[1]) {
            bool is_physical = (move_category == eos::CATEGORY_PHYSICAL);
            atk_stage_boost = static_cast<int32_t>(is_physical);
            if (is_physical) {
                dungeon.damage_calc.ability_offense_modifier += 1;
            }
        } else {
            bool is_special = (move_category == eos::CATEGORY_SPECIAL);
            atk_stage_boost = static_cast<int32_t>(is_special);
            if (is_special) {
                dungeon.damage_calc.ability_offense_modifier += 1;
            }
        }
    }

    if (full_calc && attack_type == eos::TYPE_FIRE) {
        int16_t flash_fire_boost = attacker.monster.stat_modifiers.flash_fire_boost;
        dungeon.damage_calc.flash_fire_boost = flash_fire_boost;
        atk_stage_boost += flash_fire_boost;
    }

    if (attacker.iq_skill_enabled(eos::IQ_AGGRESSOR, dungeon)) {
        atk_stage_boost += 1;
        dungeon.damage_calc.iq_skill_offense_modifier += 1;
    }

    if (attacker.iq_skill_enabled(eos::IQ_DEFENDER, dungeon)) {
        atk_stage_boost -= 1;
        dungeon.damage_calc.iq_skill_offense_modifier -= 1;
    }

    if (attacker.iq_skill_enabled(eos::IQ_PRACTICE_SWINGER, dungeon) &&
        attacker.monster.practice_swinger_flag) {
        atk_stage_boost += 1;
        dungeon.damage_calc.iq_skill_offense_modifier += 1;
    }

    if (attacker.nearby_ally_iq_skill_enabled(eos::IQ_CHEERLEADER, dungeon)) {
        atk_stage_boost += 1;
    }

    if (move_category == eos::CATEGORY_PHYSICAL) {
        if (attacker.ability_active(eos::ABILITY_RIVALRY)) {
            if (genders_equal_not_genderless(attacker.monster.apparent_id,
                                             defender.monster.apparent_id)) {
                atk_stage_boost += 1;
                dungeon.damage_calc.ability_offense_modifier += 1;
            } else if (attacker.gender() != eos::GENDER_GENDERLESS &&
                       defender.gender() != eos::GENDER_GENDERLESS) {
                atk_stage_boost -= 1;
                dungeon.damage_calc.ability_offense_modifier -= 1;
            }
        }

        if (attacker.perceived_weather(dungeon) == eos::WEATHER_SUNNY &&
            (attacker.ability_active(eos::ABILITY_FLOWER_GIFT) ||
             attacker.other_monster_ability_active(eos::ABILITY_FLOWER_GIFT, dungeon))) {
            atk_stage_boost += 1;
            dungeon.damage_calc.ability_offense_modifier += 1;
        }
    } else {
        if (attacker.ability_active(eos::ABILITY_SOLAR_POWER) &&
            attacker.perceived_weather(dungeon) == eos::WEATHER_SUNNY) {
            atk_stage_boost += 2;
            dungeon.damage_calc.ability_offense_modifier += 2;
        }

        if (defender.perceived_weather(dungeon) == eos::WEATHER_SUNNY &&
            (defender.ability_active(eos::ABILITY_FLOWER_GIFT) ||
             defender.other_monster_ability_active(eos::ABILITY_FLOWER_GIFT, dungeon))) {
            def_stage = 1;
            dungeon.damage_calc.ability_defense_modifier += 1;
        }

        if (defender.perceived_weather(dungeon) == eos::WEATHER_SANDSTORM) {
            if (defender.monster.types[0] == eos::TYPE_ROCK ||
                defender.monster.types[1] == eos::TYPE_ROCK) {
                def_stage += 2;
            }
        }
    }

    if (attacker.monster.apparent_id == eos::MONSTER_DEOXYS_ATTACK) {
        atk_stage_boost += 2;
    }
    if (attacker.monster.apparent_id == eos::MONSTER_DEOXYS_DEFENSE) {
        atk_stage_boost -= 2;
    }
    if (attacker.monster.apparent_id == eos::MONSTER_DEOXYS_SPEED) {
        atk_stage_boost -= 2;
    }
    if (attacker.monster.apparent_id == eos::MONSTER_GIRATINA_ALTERED) {
        atk_stage_boost -= 2;
    } else if (attacker.monster.apparent_id == eos::MONSTER_GIRATINA_ORIGIN) {
        atk_stage_boost += 2;
    }

    int32_t atk_stage =
        attacker.monster.stat_modifiers.offensive_stages[move_category] + atk_stage_boost;
    if (attacker.monster.anger_point_flag && attacker.ability_active(eos::ABILITY_ANGER_POINT)) {
        atk_stage = 20;
    }

    if (move_category == eos::CATEGORY_PHYSICAL) {
        if (defender.monster.statuses.skull_bash) {
            dungeon.damage_calc.skull_bash_defense_boost_activated = true;
            def_stage += 1;
        }

        if (defender.iq_skill_enabled(eos::IQ_COUNTER_BASHER, dungeon)) {
            def_stage -= 1;
            dungeon.damage_calc.iq_skill_defense_modifier -= 1;
        }
    }

    if (defender.iq_skill_enabled(eos::IQ_AGGRESSOR, dungeon)) {
        def_stage -= 1;
        dungeon.damage_calc.iq_skill_defense_modifier -= 1;
    }
    if (defender.iq_skill_enabled(eos::IQ_DEFENDER, dungeon)) {
        def_stage += 1;
        dungeon.damage_calc.iq_skill_defense_modifier += 1;
    }

    if (defender.monster.apparent_id == eos::MONSTER_DEOXYS_ATTACK) {
        def_stage -= 2;
    }
    if (defender.monster.apparent_id == eos::MONSTER_DEOXYS_DEFENSE) {
        def_stage += 2;
    }
    if (defender.monster.apparent_id == eos::MONSTER_DEOXYS_SPEED) {
        def_stage -= 2;
    }
    if (defender.monster.apparent_id == eos::MONSTER_GIRATINA_ALTERED) {
        def_stage += 2;
    } else if (defender.monster.apparent_id == eos::MONSTER_GIRATINA_ORIGIN) {
        def_stage -= 2;
    }
    def_stage += defender.monster.stat_modifiers.defensive_stages[move_category];

    if (move_id == eos::MOVE_PUNISHMENT) {
        atk_stage_boost = 0;
        int32_t stage = defender.monster.stat_modifiers.offensive_stages[0];
        if (stage > 10) {
            atk_stage_boost = ((stage - 10) << 16) >> 16;
        }
        stage = defender.monster.stat_modifiers.defensive_stages[0];
        if (stage > 10) {
            atk_stage_boost = ((atk_stage_boost + stage - 10) << 16) >> 16;
        }
        stage = defender.monster.stat_modifiers.offensive_stages[1];
        if (stage > 10) {
            atk_stage_boost = ((atk_stage_boost + stage - 10) << 16) >> 16;
        }
        stage = defender.monster.stat_modifiers.defensive_stages[1];
        if (stage > 10) {
            atk_stage_boost = ((atk_stage_boost + stage - 10) << 16) >> 16;
        }

        atk_stage += atk_stage_boost;
    }

    if (attacker.ability_active(eos::ABILITY_UNAWARE)) {
        def_stage = 10;
        def_stage_mult = Fx32{1};
    } else if (defender.ability_active(eos::ABILITY_UNAWARE)) {
        atk_stage = 10;
        atk_stage_mult = Fx32{1};
    }

    if (atk_stage < 0) {
        atk_stage = 0;
    }
    if (atk_stage > 20) {
        atk_stage = 20;
    }
    dungeon.damage_calc.offensive_stat_stage = atk_stage;
    dungeon.damage_calc.offensive_stat = attacker.monster.offensive_stats[move_category];
    Fx32 atk_stat_stage_mult = mechanics::OFFENSIVE_STAT_STAGE_MULTIPLIERS[atk_stage];
    Fx32 atk_mult = Fx32{attacker.monster.offensive_stats[move_category]} * atk_stat_stage_mult;
    int32_t atk = (atk_mult * atk_stage_mult).trunc();

    if (def_stage < 0) {
        def_stage = 0;
    }
    if (def_stage > 20) {
        def_stage = 20;
    }
    dungeon.damage_calc.defensive_stat_stage = def_stage;
    dungeon.damage_calc.defensive_stat = defender.monster.defensive_stats[move_category];
    Fx32 def_mult = Fx32{defender.monster.defensive_stats[move_category]} *
                    mechanics::DEFENSIVE_STAT_STAGE_MULTIPLIERS[def_stage];
    int32_t def = (def_mult * def_stage_mult).trunc();

    if (!attacker.monster.is_not_team_member) {
        atk += attacker.exclusive_item_offense_boost(move_category);
    }
    if (!defender.monster.is_not_team_member) {
        def += defender.exclusive_item_defense_boost(move_category);
    }

    if (move_category == eos::CATEGORY_PHYSICAL) {
        if (attacker.item_active(eos::ITEM_POWER_BAND)) {
            atk += mechanics::POWER_BAND_STAT_BOOST;
            dungeon.damage_calc.item_atk_modifier += mechanics::POWER_BAND_STAT_BOOST;
        }
        if (attacker.item_active(eos::ITEM_MUNCH_BELT)) {
            atk += mechanics::MUNCH_BELT_STAT_BOOST;
            dungeon.damage_calc.item_atk_modifier += mechanics::MUNCH_BELT_STAT_BOOST;
        }
        if (attacker.aura_bow_active()) {
            atk += mechanics::AURA_BOW_STAT_BOOST;
            // Yes, really
            dungeon.damage_calc.item_sp_atk_modifier += mechanics::AURA_BOW_STAT_BOOST;
        }

        if (full_calc) {
            if (defender.item_active(eos::ITEM_DEF_SCARF)) {
                def += mechanics::DEF_SCARF_STAT_BOOST;
                dungeon.damage_calc.item_def_modifier += mechanics::DEF_SCARF_STAT_BOOST;
            }
            if (defender.aura_bow_active()) {
                def += mechanics::AURA_BOW_STAT_BOOST;
                dungeon.damage_calc.item_def_modifier += mechanics::AURA_BOW_STAT_BOOST;
            }
        }
    } else {
        if (full_calc) {
            if (defender.item_active(eos::ITEM_ZINC_BAND)) {
                def += mechanics::ZINC_BAND_STAT_BOOST;
                dungeon.damage_calc.item_sp_def_modifier += mechanics::ZINC_BAND_STAT_BOOST;
            }
            if (defender.aura_bow_active()) {
                def += mechanics::AURA_BOW_STAT_BOOST;
                // Yes, really
                dungeon.damage_calc.item_def_modifier += mechanics::AURA_BOW_STAT_BOOST;
            }
        }

        if (attacker.item_active(eos::ITEM_SPECIAL_BAND)) {
            atk += mechanics::SPECIAL_BAND_STAT_BOOST;
            dungeon.damage_calc.item_sp_atk_modifier += mechanics::SPECIAL_BAND_STAT_BOOST;
        }
        if (attacker.item_active(eos::ITEM_MUNCH_BELT)) {
            atk += mechanics::MUNCH_BELT_STAT_BOOST;
            dungeon.damage_calc.item_sp_atk_modifier += mechanics::MUNCH_BELT_STAT_BOOST;
        }
        // Yes, really
        if (defender.aura_bow_active()) {
            atk += mechanics::AURA_BOW_STAT_BOOST;
            dungeon.damage_calc.item_sp_atk_modifier += mechanics::AURA_BOW_STAT_BOOST;
        }
    }

    Fx64 power = Fx64{(Fx32{attack_power} * atk_stat_stage_mult) * atk_stage_mult};

    int32_t atk_mult_int = 1;
    int32_t atk_div = 1;
    int32_t def_mult_int = 1;
    int32_t def_div = 1;
    not_physical = mechanics::move_not_physical(move_id);
    if (!not_physical && attacker.ability_active(eos::ABILITY_GUTS) &&
        attacker.has_negative_status(true)) {
        atk_mult_int = 2;
    }

    if (attacker.ability_active(eos::ABILITY_HUGE_POWER) ||
        attacker.ability_active(eos::ABILITY_PURE_POWER)) {
        if (dungeon.rng.roll_huge_pure_power() && !not_physical) {
            atk_mult_int *= 3;
            atk_div = 2;
        }
    }

    if (attacker.ability_active(eos::ABILITY_HUSTLE) && !not_physical) {
        atk_mult_int *= 3;
        atk_div <<= 1;
    }

    int team_idx = attacker.monster.is_not_team_member ? 0 : 1;
    if (attacker.ability_active(eos::ABILITY_PLUS) && not_physical &&
        dungeon.minus_is_active[team_idx]) {
        atk_div *= 10;
        atk_mult_int *= 15;
    }
    if (attacker.ability_active(eos::ABILITY_MINUS) && not_physical &&
        dungeon.plus_is_active[team_idx]) {
        atk_div *= 10;
        atk_mult_int *= 15;
    }

    if (defender.ability_active(eos::ABILITY_INTIMIDATE, attacker, true) && !not_physical) {
        atk_mult_int <<= 2;
        atk_div *= 5;
    }

    if (defender.ability_active(eos::ABILITY_MARVEL_SCALE, attacker, true) && !not_physical) {
        if (defender.has_negative_status(true)) {
            def_mult_int = 3;
            def_div = 2;
        }
    }

    atk *= atk_mult_int;
    def *= def_mult_int;

    if (atk_div != 1) {
        atk /= atk_div;
    }
    if (def_div != 1) {
        def /= def_div;
    }

    dungeon.damage_calc.offense_calc = atk;
    dungeon.damage_calc.defense_calc = def;

    if (atk < 0) {
        atk = 0;
    }
    if (atk >= mechanics::OFFENSE_STAT_MAX) {
        atk = mechanics::OFFENSE_STAT_MAX;
    }

    dungeon.damage_calc.damage_calc_def = def;

    Fx64 def_fx = Fx64{def};
    Fx64 level = Fx64{attacker.monster.level};
    Fx64 flv = Fx64{attacker.monster.level} + (Fx64{atk - def} / Fx64{8});
    Fx64 at = power + Fx64{atk};
    dungeon.damage_calc.damage_calc_at = at.round();
    dungeon.damage_calc.attacker_level = attacker.monster.level;
    dungeon.damage_calc.damage_calc_flv = flv.round();

    Fx64 at_scaled = at * Fx64{Fx32::CONST_153_DIV_256};
    Fx64 def_scaled = def_fx * Fx64{Fx32::CONST_NEG0_5};
    int32_t ln_arg = ((flv + Fx64{Fx32{50}}) * Fx64{Fx32{10}}).round();
    Fx64 ln = clamped_ln(ln_arg);
    Fx64 ln_scaled = ln * Fx64{Fx32{50}};

    Fx64 base = ((def_scaled + at_scaled) + ln_scaled) + Fx64{Fx32{-311}};

    if (dungeon.gen_info.fixed_room_id != eos::FIXED_SUBSTITUTE_ROOM &&
        attacker.monster.is_not_team_member) {
        base /= Fx64{Fx32::CONST_85_DIV_64};
    }
    if (Fx64{999} < base) {
        base = Fx64{999};
    }
    if (base < Fx64{1}) {
        base = Fx64{1};
    }

    Fx64 damage_mult_dynamic;
    bool super_effective = calc_type_based_damage_effects(
        dungeon, damage_mult_dynamic, attacker, defender, attack_power, attack_type, damage_out,
        mechanics::is_regular_attack_or_projectile(move_id));

    if (full_calc &&
        !attacker.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_BYPASS_REFLECT_LIGHT_SCREEN)) {
        if (move_category == eos::CATEGORY_PHYSICAL &&
            ((move_id != eos::MOVE_BRICK_BREAK && defender.monster.statuses.reflect) ||
             defender.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_HALVED_PHYSICAL_DAMAGE))) {
            damage_mult_dynamic *= Fx64::CONST_0_5;
            dungeon.damage_calc.half_physical_damage_activated = true;
        }
        if (move_category == eos::CATEGORY_SPECIAL &&
            (defender.monster.statuses.light_screen ||
             defender.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_HALVED_SPECIAL_DAMAGE))) {
            damage_mult_dynamic *= Fx64::CONST_0_5;
            dungeon.damage_calc.half_special_damage_activated = true;
        }
    }

    if (!defender.monster.statuses.lucky_chant &&
        !defender.ability_active(eos::ABILITY_BATTLE_ARMOR, attacker, true) &&
        !defender.ability_active(eos::ABILITY_SHELL_ARMOR, attacker, true) &&
        !defender.iq_skill_enabled(eos::IQ_CRITICAL_DODGER, dungeon)) {
        if (attacker.gender() != eos::GENDER_FEMALE) {
            crit_chance += crit_chance / 2;
        }

        if (attacker.monster.statuses.focus_energy) {
            dungeon.damage_calc.focus_energy_activated = true;
            crit_chance = mechanics::OFFENSE_STAT_MAX;
        } else {
            if (attacker.item_active(eos::ITEM_SCOPE_LENS) ||
                attacker.iq_skill_enabled(eos::IQ_SHARPSHOOTER, dungeon)) {
                dungeon.damage_calc.scope_lens_or_sharpshooter_activated = true;
                crit_chance += mechanics::SCOPE_LENS_CRIT_RATE_BOOST;
            }
            if (attacker.ability_active(eos::ABILITY_SUPER_LUCK)) {
                dungeon.damage_calc.super_luck_activated = true;
                crit_chance += mechanics::SUPER_LUCK_CRIT_RATE_BOOST;
            }
            if (defender.item_active(eos::ITEM_PATSY_BAND)) {
                dungeon.damage_calc.patsy_band_activated = true;
                // same boost
                crit_chance += mechanics::SCOPE_LENS_CRIT_RATE_BOOST;
            }
            if (super_effective &&
                attacker.iq_skill_enabled(eos::IQ_TYPE_ADVANTAGE_MASTER, dungeon)) {
                // override, not add
                crit_chance = mechanics::TYPE_ADVANTAGE_MASTER_CRIT_RATE;
                dungeon.damage_calc.type_advantage_master_activated = true;
            }
        }

        if (dungeon.rng.roll_critical_hit(crit_chance) &&
            !defender.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_NO_CRITICAL_HITS)) {
            damage_out.critical_hit = true;
            if (attacker.ability_active(eos::ABILITY_SNIPER)) {
                damage_mult_dynamic *= Fx64{2};
                dungeon.damage_calc.sniper_activated = true;
            } else {
                damage_mult_dynamic *= Fx64::CONST_1_5;
            }
        }
    }

    dungeon.damage_calc.damage_calc_base = base.round();
    base *= damage_mult_dynamic;
    dungeon.damage_calc.static_damage_mult = damage_mult;
    base *= Fx64{damage_mult};
    dungeon.damage_calc.damage_calc = base.round();

    Fx64 variance = dungeon.rng.roll_damage_variance();
    base *= variance;
    dungeon.damage_calc.damage_calc_random_mult_pct = (Fx64{100} * variance).round();
    damage_out.damage = base.round();

    if (move_id == eos::MOVE_PROJECTILE) {
        damage_out.damage = (Fx32{damage_out.damage} * Fx32::CONST_0_5).ceil();
    }
    if (move_id == eos::MOVE_PROJECTILE &&
        attacker.iq_skill_enabled(eos::IQ_POWER_PITCHER, dungeon)) {
        damage_out.damage =
            (Fx32{damage_out.damage} * mechanics::POWER_PITCHER_DAMAGE_MULTIPLIER).ceil();
    }

    if (damage_out.damage > 0 &&
        attacker.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_DAMAGE_BOOST_50_PCT)) {
        damage_out.damage =
            (Fx32{damage_out.damage} * mechanics::AIR_BLADE_DAMAGE_MULTIPLIER).ceil();
    }

    damage_out.damage_message = eos::DAMAGE_MESSAGE_MOVE;
    if (damage_out.damage == 0) {
        damage_out.critical_hit = false;
    }
    defender.monster.anger_point_flag = damage_out.critical_hit;
}

// pmdsky-debug: MoveHitCheck ([NA] 0x2323C48)
bool move_hit_check(DungeonState& dungeon, const MonsterEntity& attacker,
                    const MonsterEntity& defender, eos::move_id move_id, bool use_second_accuracy,
                    bool never_miss_self) {
    if (never_miss_self && &attacker == &defender) {
        return true;
    }
    if (move_id == eos::MOVE_REGULAR_ATTACK &&
        attacker.iq_skill_enabled(eos::IQ_SURE_HIT_ATTACKER, dungeon)) {
        return true;
    }
    if (attacker.monster.statuses.sure_shot) {
        return true;
    }
    if (attacker.monster.statuses.whiffer) {
        return false;
    }

    int32_t move_accuracy = mechanics::get_move_accuracy(move_id, use_second_accuracy);
    if (move_accuracy > 100) {
        return true;
    }
    if (defender.item_active(eos::ITEM_DETECT_BAND)) {
        move_accuracy -= mechanics::DETECT_BAND_MOVE_ACCURACY_DROP;
    }
    if (defender.iq_skill_enabled(eos::IQ_QUICK_DODGER, dungeon)) {
        move_accuracy -= mechanics::QUICK_DODGER_MOVE_ACCURACY_DROP;
    }

    int32_t accuracy_boost = 0;

    if (attacker.ability_active(eos::ABILITY_COMPOUNDEYES)) {
        accuracy_boost = 2;
    }

    if (move_id == eos::MOVE_THUNDER) {
        eos::weather_id weather = attacker.perceived_weather(dungeon);
        if (weather == eos::WEATHER_RAIN) {
            return true;
        }
        if (weather == eos::WEATHER_SUNNY) {
            accuracy_boost -= 2;
        }
    }

    if (move_id == eos::MOVE_BLIZZARD && attacker.perceived_weather(dungeon) == eos::WEATHER_HAIL) {
        return true;
    }

    if (attacker.iq_skill_enabled(eos::IQ_CONCENTRATOR, dungeon)) {
        accuracy_boost += 1;
    }

    int32_t evasion_stage = defender.monster.stat_modifiers.hit_chance_stages[1];
    if (defender.monster.statuses.exposed) {
        evasion_stage = 10;
    }

    int32_t evasion_boost = 0;
    if (defender.perceived_weather(dungeon) == eos::WEATHER_SANDSTORM &&
        defender.ability_active(eos::ABILITY_SAND_VEIL, attacker, true)) {
        evasion_boost = 2;
    }

    if (attacker.ability_active(eos::ABILITY_HUSTLE) && !mechanics::move_not_physical(move_id)) {
        evasion_boost += 2;
    }

    if (defender.iq_skill_enabled(eos::IQ_CLUTCH_PERFORMER, dungeon)) {
        int32_t max_hp = defender.monster.max_hp_stat + defender.monster.max_hp_boost;
        if (max_hp > mechanics::MAX_HP_CAP) {
            max_hp = mechanics::MAX_HP_CAP;
        }
        if (defender.monster.hp <= div4_trunc(max_hp)) {
            evasion_boost += 2;
        }
    }

    if (defender.iq_skill_enabled(eos::IQ_CONCENTRATOR, dungeon)) {
        evasion_boost -= 1;
    }

    if (defender.ability_active(eos::ABILITY_TANGLED_FEET, attacker, true) &&
        (defender.monster.statuses.confusion || defender.monster.statuses.cross_eyed)) {
        evasion_boost += 3;
    }

    if (defender.ability_active(eos::ABILITY_SNOW_CLOAK, attacker, true) &&
        (defender.perceived_weather(dungeon) == eos::WEATHER_HAIL ||
         defender.perceived_weather(dungeon) == eos::WEATHER_SNOW)) {
        evasion_boost += 2;
    }

    eos::weather_id weather = defender.perceived_weather(dungeon);
    if (mechanics::EXCL_ITEM_EFFECTS_EVASION_BOOST[weather] != eos::EXCLUSIVE_EFF_STAT_BOOST &&
        defender.exclusive_item_effect_active(
            mechanics::EXCL_ITEM_EFFECTS_EVASION_BOOST[weather])) {
        evasion_boost += 1;
    }

    evasion_stage += evasion_boost;
    int32_t accuracy_stage = attacker.monster.stat_modifiers.hit_chance_stages[0] + accuracy_boost;

    if (attacker.ability_active(eos::ABILITY_NO_GUARD) ||
        defender.ability_active(eos::ABILITY_NO_GUARD, attacker, true)) {
        evasion_stage = 10;
        accuracy_stage = 10;
    }

    if (accuracy_stage < 0) {
        accuracy_stage = 0;
    }
    if (accuracy_stage > 20) {
        accuracy_stage = 20;
    }

    auto ACCURACY_MULTIPLIERS = (attacker.gender() == eos::GENDER_FEMALE)
                                    ? mechanics::FEMALE_ACCURACY_STAGE_MULTIPLIERS
                                    : mechanics::MALE_ACCURACY_STAGE_MULTIPLIERS;
    Fx32 accuracy = ACCURACY_MULTIPLIERS[accuracy_stage];

    if (evasion_stage < 0) {
        evasion_stage = 0;
    }
    if (evasion_stage > 20) {
        evasion_stage = 20;
    }

    if (accuracy < Fx32{0}) {
        accuracy = Fx32{0};
    }
    if (accuracy > Fx32{100}) {
        accuracy = Fx32{100};
    }

    auto EVASION_MULTIPLIERS = (defender.gender() == eos::GENDER_FEMALE)
                                   ? mechanics::FEMALE_EVASION_STAGE_MULTIPLIERS
                                   : mechanics::MALE_EVASION_STAGE_MULTIPLIERS;
    Fx32 evasion = EVASION_MULTIPLIERS[evasion_stage];
    if (evasion < Fx32{0}) {
        evasion = Fx32{0};
    }
    if (evasion > Fx32{100}) {
        evasion = Fx32{100};
    }

    return dungeon.rng.roll_hit_chance(((move_accuracy * accuracy) * evasion).trunc(),
                                       use_second_accuracy);
}

// Based on pmdsky-debug: ApplyDamage ([NA] 0x2308FE0). The only (known) thing that really matters
// for this damage calculator within this function is negating damage due to abilities and exclusive
// item effects.
void apply_ability_and_effect_immunities(const MonsterEntity& attacker,
                                         const MonsterEntity& defender, DamageData& damage_data) {
    if (!defender.is_monster() || !attacker.is_monster()) {
        return;
    }
    if (defender.ability_active(eos::ABILITY_VOLT_ABSORB, attacker, true) &&
        damage_data.type == eos::TYPE_ELECTRIC) {
        damage_data.no_damage = true;
        damage_data.healed = true;
        return;
    }
    if ((defender.ability_active(eos::ABILITY_WATER_ABSORB, attacker, true) ||
         defender.ability_active(eos::ABILITY_DRY_SKIN, attacker, true)) &&
        damage_data.type == eos::TYPE_WATER) {
        damage_data.no_damage = true;
        damage_data.healed = true;
        return;
    }
    if (defender.ability_active(eos::ABILITY_MOTOR_DRIVE, attacker, true) &&
        damage_data.type == eos::TYPE_ELECTRIC) {
        damage_data.no_damage = true;
        return;
    }

    for (const auto* entry = &mechanics::TYPE_DAMAGE_NEGATING_EXCLUSIVE_ITEM_EFFECTS[0];
         entry->type != eos::TYPE_NEUTRAL; ++entry) {
        if (entry->type == damage_data.type &&
            defender.exclusive_item_effect_active(entry->effect)) {
            if (entry->effect < eos::EXCLUSIVE_EFF_ABSORB_FIRE_DAMAGE) {
                damage_data.no_damage = true;
                return;
            } else {
                // Apparently when damage is absorbed, the game still counts it as damage being
                // "dealt" for the purposes of other things, so damage_data.no_damage isn't set
                damage_data.healed = true;
                return;
            }
        }
    }
}

// Based on pmdsky-debug: PerformDamageSequence ([NA] 0x2332D6C), omitting the things that don't
// matter
int32_t run_mock_damage_sequence(DungeonState& dungeon, MonsterEntity& attacker,
                                 const MonsterEntity& defender, eos::move_id move_id,
                                 DamageData& damage_data) {
    if (move_hit_check(dungeon, attacker, defender, move_id, true, true)) {
        apply_ability_and_effect_immunities(attacker, defender, damage_data);
        attacker.monster.practice_swinger_flag = false;
        attacker.monster.anger_point_flag = false;
    } else {
        damage_data.no_damage = true;
        if (move_id != eos::MOVE_REGULAR_ATTACK) {
            attacker.monster.practice_swinger_flag = true;
        }
    }

    if (damage_data.no_damage) {
        return 0;
    }
    return damage_data.damage;
}

// Loosely based on pmdsky-debug: ExecuteMoveEffect ([NA] 0x232E864)
// Includes stuff known to happen before calls to DealDamage and friends that affect the damage
// calculation. This function is likely incomplete and should be considered experimental; still
// need to get a better understanding of the in-game function works... (TODO)
// Returns whether or not the move has passed all the hit checks in this function.
bool execute_move_effect_prechecks(DungeonState& dungeon, MonsterEntity& attacker,
                                   MonsterEntity& defender, eos::move_id move_id) {
    // TODO: actually handle the checks for this flag properly, although it doesn't really matter
    // since we don't care about status moves.
    bool reflected_by_magic_coat_etc = false;

    // I don't actually understand how these checks work, so this is just a made-up implementation
    // that probably approximates what the game does.
    bool lightningrod = (defender.ability_active(eos::ABILITY_LIGHTNINGROD) ||
                         dungeon.other_monsters.abilities[eos::ABILITY_LIGHTNINGROD]) &&
                        attacker.get_move_type(move_id, dungeon) == eos::TYPE_ELECTRIC;
    bool storm_drain = (defender.ability_active(eos::ABILITY_STORM_DRAIN) ||
                        dungeon.other_monsters.abilities[eos::ABILITY_STORM_DRAIN]) &&
                       attacker.get_move_type(move_id, dungeon) == eos::TYPE_WATER;

    bool hit = true;
    if (defender.two_turn_move_forced_miss(move_id)) {
        hit = false;
        // For program reporting purposes; not really in the game
        dungeon.damage_calc.two_turn_move_forced_miss = true;
    }
    if (hit && defender.ability_active(eos::ABILITY_SOUNDPROOF, attacker, true) &&
        mechanics::is_sound_move(move_id)) {
        hit = false;
        dungeon.damage_calc.soundproof_activated =
            true; // For program reporting purposes; not really in the game
    }
    if (hit && defender.ability_active(eos::ABILITY_FOREWARN, attacker, true) &&
        dungeon.rng.roll_forewarn()) {
        hit = false;
    }
    bool never_miss_self = (move_id != eos::MOVE_ENDURE && move_id != eos::MOVE_DETECT &&
                            move_id != eos::MOVE_PROTECT && !reflected_by_magic_coat_etc);
    if (hit && !move_hit_check(dungeon, attacker, defender, move_id, false, never_miss_self)) {
        hit = false;
        dungeon.damage_calc.first_hit_check_failed =
            true; // For program reporting purposes; not really in the game
    }
    if (hit) {
        if (lightningrod) {
            // For program reporting purposes; not really in the game
            dungeon.damage_calc.lightningrod_activated = true;
            hit = false;
        }
        if (storm_drain) {
            // For program reporting purposes; not really in the game
            dungeon.damage_calc.storm_drain_activated = true;
            hit = false;
        }
    }

    if (!hit && move_id != eos::MOVE_REGULAR_ATTACK) {
        attacker.monster.practice_swinger_flag = true;
    }
    return hit;
}

// Based on the shared parts of DealDamage and friends, omitting the things that don't matter
int32_t simulate_damage_calc_shared(DamageData& damage_data, DungeonState& dungeon,
                                    MonsterEntity& attacker, MonsterEntity& defender,
                                    eos::type_id attack_type, int32_t attack_power,
                                    Fx32 damage_mult, eos::move_id move_id) {
    int32_t crit_chance = mechanics::get_move_crit_chance(move_id);
    calc_damage(dungeon, attacker, defender, attack_type, attack_power, crit_chance, damage_data,
                damage_mult, move_id, true);
    return run_mock_damage_sequence(dungeon, attacker, defender, move_id, damage_data);
}

// Based on pmdsky-debug: ExecuteMoveEffect ([NA] 0x232E864) + DealDamage ([NA] 0x2332B20)
int32_t simulate_damage_calc_with_mult(DamageData& damage_data, DungeonState& dungeon,
                                       MonsterEntity& attacker, MonsterEntity& defender, Move move,
                                       Fx32 damage_mult) {
    if (!execute_move_effect_prechecks(dungeon, attacker, defender, move.id)) {
        return 0;
    }

    eos::type_id attack_type = attacker.get_move_type(move.id, dungeon);
    int32_t attack_power = attacker.get_move_power(move);
    return simulate_damage_calc_shared(damage_data, dungeon, attacker, defender, attack_type,
                                       attack_power, damage_mult, move.id);
}

// Generic version of pmdsky-debug: ExecuteMoveEffect ([NA] 0x232E864) + DealDamage ([NA] 0x2332B20)
// that in theory permits custom move definitions
int32_t simulate_damage_calc_generic(DamageData& damage_data, DungeonState& dungeon,
                                     MonsterEntity& attacker, MonsterEntity& defender,
                                     eos::type_id attack_type, int32_t attack_power,
                                     eos::move_id move_id, int32_t crit_chance, Fx32 damage_mult) {
    if (!execute_move_effect_prechecks(dungeon, attacker, defender, move_id)) {
        return 0;
    }

    calc_damage(dungeon, attacker, defender, attack_type, attack_power, crit_chance, damage_data,
                damage_mult, move_id, true);
    return run_mock_damage_sequence(dungeon, attacker, defender, move_id, damage_data);
}

// Based on pmdsky-debug:
// - ExecuteMoveEffect ([NA] 0x232E864)
// - DoMoveWeatherBall ([NA] 0x23266DC)
// - DealDamageWithType ([NA] 0x2332CDC)
int32_t simulate_damage_calc_weather_ball(DamageData& damage_data, DungeonState& dungeon,
                                          MonsterEntity& attacker, MonsterEntity& defender,
                                          uint8_t ginseng = 0) {
    if (!execute_move_effect_prechecks(dungeon, attacker, defender, eos::MOVE_WEATHER_BALL)) {
        return 0;
    }

    eos::weather_id weather = attacker.perceived_weather(dungeon);
    eos::type_id attack_type = mechanics::WEATHER_BALL_TYPE_TABLE[weather];
    Fx32 damage_mult = mechanics::WEATHER_BALL_DAMAGE_MULT_TABLE[weather];
    int32_t attack_power = attacker.get_move_power(Move{eos::MOVE_WEATHER_BALL, ginseng});
    return simulate_damage_calc_shared(damage_data, dungeon, attacker, defender, attack_type,
                                       attack_power, damage_mult, eos::MOVE_WEATHER_BALL);
}

// Based on pmdsky-debug:
// - ExecuteMoveEffect ([NA] 0x232E864)
// - DoMoveNaturalGift ([NA] 0x232D738)
// - DealDamageWithTypeAndPowerBoost ([NA] 0x2332BB8)
int32_t simulate_damage_calc_natural_gift(DamageData& damage_data, DungeonState& dungeon,
                                          MonsterEntity& attacker, MonsterEntity& defender,
                                          uint8_t ginseng = 0) {
    if (!execute_move_effect_prechecks(dungeon, attacker, defender, eos::MOVE_NATURAL_GIFT)) {
        return 0;
    }

    Move move{eos::MOVE_NATURAL_GIFT, ginseng};
    if (attacker.monster.held_item.id != eos::ITEM_NOTHING) {
        auto ng_info = attacker.natural_gift_info();
        if (ng_info) {
            int32_t attack_power =
                static_cast<uint16_t>(attacker.get_move_power(move)) + ng_info->base_power_boost;
            if (attack_power > std::numeric_limits<int16_t>::max()) {
                // Signed integer overflow is implementation-dependent, so we can't rely on it
                attack_power -= (1 << 16);
            }
            return simulate_damage_calc_shared(damage_data, dungeon, attacker, defender,
                                               ng_info->type_id, attack_power, Fx32{1},
                                               eos::MOVE_NATURAL_GIFT);
        }
    }

    return simulate_damage_calc_with_mult(damage_data, dungeon, attacker, defender, move, Fx32{1});
}

int get_hp_dep_mult_table_idx(const MonsterEntity& entity) {
    int32_t hp = entity.monster.hp;
    int32_t max_hp = std::min(entity.monster.max_hp_stat + entity.monster.max_hp_boost, 999);
    if (hp <= div4_trunc(max_hp)) {
        return 0;
    }
    if (hp <= div4_trunc(max_hp * 2)) {
        return 1;
    }
    if (hp <= div4_trunc(max_hp * 3)) {
        return 2;
    }
    return 3;
}

// Dispatches to the correct simulation_damage_calc* function based on the move, figuring out the
// damage multiplier if needed (based on the move effect handler).
// The game doesn't do things this way; the logic in this function is hard-coded into the program
// structure in the actual game.
int32_t simulate_damage_calc(DamageData& damage_data, DungeonState& dungeon,
                             MonsterEntity& attacker, MonsterEntity& defender, Move move) {
    Fx32 damage_mult = 1; // Default, used in most cases

    // Multipliers come from ExecuteMoveEffect itself or from the move effect handler
    switch (move.id) {
    case eos::MOVE_NOTHING:
        damage_mult = 0;
        break;
    case eos::MOVE_ICE_BALL:
    case eos::MOVE_ROLLOUT:
        // Can't hit more than 10 times
        damage_mult = mechanics::ROLLOUT_DAMAGE_MULT_TABLE[std::min(
            static_cast<int>(move.prior_successive_hits), 9)];
        break;
    case eos::MOVE_DIG:
        damage_mult = mechanics::DIG_DAMAGE_MULTIPLIER;
        break;
    case eos::MOVE_WEATHER_BALL:
        return simulate_damage_calc_weather_ball(damage_data, dungeon, attacker, defender,
                                                 move.ginseng);
    case eos::MOVE_WHIRLPOOL:
    case eos::MOVE_SURF:
        if (defender.monster.statuses.diving) {
            damage_mult = 2;
        }
        break;
    case eos::MOVE_GUST:
    case eos::MOVE_TWISTER:
        if (defender.monster.statuses.flying || defender.monster.statuses.bouncing) {
            damage_mult = 2;
        }
        break;
    case eos::MOVE_RAZOR_WIND:
        damage_mult = mechanics::RAZOR_WIND_DAMAGE_MULTIPLIER;
        break;
    case eos::MOVE_FACADE: {
        auto& status = attacker.monster.statuses;
        // Not 100% sure about the Identifying status...
        if (status.burn || status.poison || status.bad_poison || status.paralysis ||
            status.identifying) {
            damage_mult = mechanics::FACADE_DAMAGE_MULTIPLIER;
        }
        break;
    }
    case eos::MOVE_FOCUS_PUNCH:
        damage_mult = mechanics::FOCUS_PUNCH_DAMAGE_MULTIPLIER;
        break;
    case eos::MOVE_REVERSAL:
    case eos::MOVE_FLAIL:
        damage_mult = mechanics::REVERSAL_DAMAGE_MULT_TABLE[get_hp_dep_mult_table_idx(attacker)];
        break;
    case eos::MOVE_SMELLINGSALT:
        if (defender.monster.statuses.paralysis) {
            damage_mult = 2;
        }
        break;
    case eos::MOVE_LOW_KICK:
    case eos::MOVE_GRASS_KNOT:
        damage_mult = mechanics::get_monster_weight(defender.monster.apparent_id);
        break;
    case eos::MOVE_SKY_ATTACK:
        damage_mult = mechanics::SKY_ATTACK_DAMAGE_MULTIPLIER;
        break;
    case eos::MOVE_WATER_SPOUT:
        damage_mult = mechanics::WATER_SPOUT_DAMAGE_MULT_TABLE[get_hp_dep_mult_table_idx(attacker)];
        break;
    case eos::MOVE_EARTHQUAKE:
        if (defender.monster.statuses.digging) {
            damage_mult = 2;
        }
        break;
    case eos::MOVE_SOLARBEAM: {
        eos::weather_id weather = attacker.perceived_weather(dungeon);
        damage_mult = mechanics::SOLARBEAM_DAMAGE_MULTIPLIER;
        if (weather == eos::WEATHER_SANDSTORM || weather == eos::WEATHER_RAIN ||
            weather == eos::WEATHER_HAIL) {
            damage_mult /= 2;
        }
        break;
    }
    case eos::MOVE_FLY:
        damage_mult = mechanics::FLY_DAMAGE_MULTIPLIER;
        break;
    case eos::MOVE_DIVE:
        damage_mult = mechanics::DIVE_DAMAGE_MULTIPLIER;
        break;
    case eos::MOVE_BOUNCE:
        damage_mult = mechanics::BOUNCE_DAMAGE_MULTIPLIER;
        break;
    case eos::MOVE_HI_JUMP_KICK:
    case eos::MOVE_BLAST_BURN:
        damage_mult = 2;
        break;
    case eos::MOVE_TRIPLE_KICK:
        // Can't hit more than 3 times
        damage_mult = std::min(static_cast<int>(move.prior_successive_hits) + 1, 3);
        break;
    case eos::MOVE_SPIT_UP:
        damage_mult = attacker.monster.statuses.stockpile_stage;
        break;
    case eos::MOVE_ERUPTION:
        damage_mult = mechanics::ERUPTION_DAMAGE_MULT_TABLE[get_hp_dep_mult_table_idx(attacker)];
        break;
    case eos::MOVE_DREAM_EATER: {
        auto& status = defender.monster.statuses;
        if (!status.sleep && !status.nightmare && !status.napping) {
            // For program reporting purposes; not really in the game
            dungeon.damage_calc.dream_eater_failed = true;
            return 0;
        }
        break;
    }
    case eos::MOVE_SKULL_BASH:
        damage_mult = mechanics::SKULL_BASH_DAMAGE_MULTIPLIER;
        break;
    case eos::MOVE_REGULAR_ATTACK:
        damage_mult = Fx32::CONST_0_5;
        break;
    case eos::MOVE_NATURAL_GIFT:
        return simulate_damage_calc_natural_gift(damage_data, dungeon, attacker, defender,
                                                 move.ginseng);
    case eos::MOVE_TRUMP_CARD: {
        int32_t max_pp = mechanics::get_move_max_pp(move.id);
        if (max_pp == 0) {
            max_pp = 1;
        }
        int32_t pp_frac = (static_cast<uint32_t>(move.pp) * 100) / max_pp;
        if (pp_frac < 26) {
            damage_mult = Fx32::CONST_1_25;
        } else if (pp_frac < 51) {
            damage_mult = 1;
        } else if (pp_frac < 76) {
            damage_mult = Fx32::CONST_0_75;
        } else {
            damage_mult = Fx32::CONST_0_5;
        }
        break;
    }
    case eos::MOVE_BRINE: {
        int32_t max_hp = defender.monster.max_hp_stat + defender.monster.max_hp_boost;
        if (max_hp > mechanics::MAX_HP_CAP) {
            max_hp = mechanics::MAX_HP_CAP;
        }
        if ((static_cast<uint32_t>(defender.monster.hp) * 2) <= max_hp) {
            damage_mult = 2;
        }
        break;
    }
    case eos::MOVE_WRING_OUT:
    case eos::MOVE_CRUSH_GRIP:
        damage_mult = mechanics::WRING_OUT_DAMAGE_MULT_TABLE[get_hp_dep_mult_table_idx(defender)];
        break;
    case eos::MOVE_GYRO_BALL:
        if (attacker.monster.statuses.speed_stage == 0) {
            damage_mult = 2;
        }
        break;
    case eos::MOVE_SHADOW_FORCE:
        damage_mult = mechanics::SHADOW_FORCE_DAMAGE_MULTIPLIER;
        break;
    case eos::MOVE_LAST_RESORT: {
        int32_t n_moves_out_of_pp = attacker.monster.n_moves_out_of_pp;
        if (n_moves_out_of_pp > 0 && move.pp == 0) {
            n_moves_out_of_pp--;
        }
        if (n_moves_out_of_pp < 1) {
            dungeon.damage_calc.last_resort_failed = true;
            return 0;
        }
        damage_mult = mechanics::LAST_RESORT_DAMAGE_MULT_TABLE[n_moves_out_of_pp - 1];
        break;
    }
    case eos::MOVE_WAKE_UP_SLAP: {
        auto& status = defender.monster.statuses;
        if (status.sleep || status.nightmare || status.napping) {
            damage_mult = 2;
        }
        break;
    }
    // TODO: add support for more moves
    default:
        break;
    }

    return simulate_damage_calc_with_mult(damage_data, dungeon, attacker, defender, move,
                                          damage_mult);
}

// Based on pmdsky-debug: ApplyItemEffect ([NA] 0x231B68C) + DealDamageProjectile ([NA] 0x2332C4C)
int32_t simulate_damage_calc_projectile(DamageData& damage_data, DungeonState& dungeon,
                                        MonsterEntity& attacker, MonsterEntity& defender,
                                        int32_t attack_power) {
    eos::type_id attack_type = attacker.get_move_type(eos::MOVE_PROJECTILE, dungeon);
    return simulate_damage_calc_shared(damage_data, dungeon, attacker, defender, attack_type,
                                       attack_power, Fx32{1}, eos::MOVE_PROJECTILE);
}
