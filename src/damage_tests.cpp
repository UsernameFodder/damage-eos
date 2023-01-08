#include <catch2/catch_test_macros.hpp>
#include "damage.hpp"

TEST_CASE("MonsterEntity methods work", "[MonsterEntity]") {
    MonsterEntity bulbasaur;
    bulbasaur.monster.apparent_id = eos::MONSTER_BULBASAUR;
    bulbasaur.monster.is_not_team_member = false;
    bulbasaur.monster.is_team_leader = true;
    bulbasaur.monster.types[0] = eos::TYPE_GRASS;
    bulbasaur.monster.types[1] = eos::TYPE_POISON;
    bulbasaur.monster.abilities[0] = eos::ABILITY_OVERGROW;

    MonsterEntity charmander;
    charmander.monster.apparent_id = eos::MONSTER_CHARMANDER;
    charmander.monster.is_not_team_member = true;
    charmander.monster.is_team_leader = false;
    charmander.monster.types[0] = eos::TYPE_FIRE;
    charmander.monster.abilities[1] = eos::ABILITY_BLAZE;

    DungeonState dungeon;

    REQUIRE(bulbasaur.is_valid());
    REQUIRE(bulbasaur.is_monster());
    REQUIRE(charmander.is_valid());
    REQUIRE(charmander.is_monster());

    SECTION("iq_skill_enabled() works") {
        REQUIRE(!bulbasaur.iq_skill_enabled(eos::IQ_ABSOLUTE_MOVER, dungeon));
        REQUIRE(!charmander.iq_skill_enabled(eos::IQ_ABSOLUTE_MOVER, dungeon));

        bulbasaur.monster.iq_skill_flags[eos::IQ_ABSOLUTE_MOVER] = true;
        charmander.monster.iq_skill_flags[eos::IQ_ABSOLUTE_MOVER] = true;

        REQUIRE(bulbasaur.iq_skill_enabled(eos::IQ_ABSOLUTE_MOVER, dungeon));
        REQUIRE(charmander.iq_skill_enabled(eos::IQ_ABSOLUTE_MOVER, dungeon));

        dungeon.iq_disabled = true;
        REQUIRE(!bulbasaur.iq_skill_enabled(eos::IQ_ABSOLUTE_MOVER, dungeon));
        REQUIRE(charmander.iq_skill_enabled(eos::IQ_ABSOLUTE_MOVER, dungeon));
    }

    SECTION("scrappy_should_activate() works") {
        REQUIRE(!bulbasaur.scrappy_should_activate(charmander, eos::TYPE_NORMAL, dungeon));
        REQUIRE(!dungeon.damage_calc.scrappy_activated);
        bulbasaur.monster.abilities[0] = eos::ABILITY_SCRAPPY;
        charmander.monster.types[0] = eos::TYPE_GHOST;
        REQUIRE(bulbasaur.scrappy_should_activate(charmander, eos::TYPE_NORMAL, dungeon));
        REQUIRE(dungeon.damage_calc.scrappy_activated);
        bulbasaur.monster.statuses.gastro_acid = true;
        REQUIRE(!bulbasaur.scrappy_should_activate(charmander, eos::TYPE_NORMAL, dungeon));
    }

    SECTION("ghost_immunity_active() works") {
        REQUIRE(!charmander.ghost_immunity_active(bulbasaur, 0));
        charmander.monster.types[0] = eos::TYPE_GHOST;
        REQUIRE(charmander.ghost_immunity_active(bulbasaur, 0));
        charmander.monster.statuses.exposed = true;
        REQUIRE(!charmander.ghost_immunity_active(bulbasaur, 0));
        charmander.monster.statuses.exposed = false;
        bulbasaur.monster.exclusive_item_effect_flags[eos::EXCLUSIVE_EFF_SCRAPPY] = true;
        REQUIRE(!charmander.ghost_immunity_active(bulbasaur, 0));
    }

    SECTION("no_gastro_acid_status() works") {
        REQUIRE(bulbasaur.no_gastro_acid_status());
        bulbasaur.monster.statuses.gastro_acid = true;
        REQUIRE(!bulbasaur.no_gastro_acid_status());
    }

    SECTION("ability_active() works") {
        REQUIRE(!bulbasaur.ability_active(eos::ABILITY_UNKNOWN));
        REQUIRE(!bulbasaur.ability_active(eos::ABILITY_UNKNOWN, charmander, true));
        bulbasaur.monster.abilities[0] = eos::ABILITY_OVERGROW;
        bulbasaur.monster.abilities[1] = eos::ABILITY_CHLOROPHYLL;
        REQUIRE(bulbasaur.ability_active(eos::ABILITY_OVERGROW));
        REQUIRE(bulbasaur.ability_active(eos::ABILITY_CHLOROPHYLL));
        REQUIRE(bulbasaur.ability_active(eos::ABILITY_OVERGROW, charmander, true));
        REQUIRE(bulbasaur.ability_active(eos::ABILITY_CHLOROPHYLL, charmander, true));
        bulbasaur.monster.statuses.gastro_acid = true;
        REQUIRE(!bulbasaur.ability_active(eos::ABILITY_OVERGROW));
        REQUIRE(!bulbasaur.ability_active(eos::ABILITY_CHLOROPHYLL));
        REQUIRE(!bulbasaur.ability_active(eos::ABILITY_OVERGROW, charmander, true));
        REQUIRE(!bulbasaur.ability_active(eos::ABILITY_CHLOROPHYLL, charmander, true));

        bulbasaur.monster.statuses.gastro_acid = false;
        charmander.monster.abilities[0] = eos::ABILITY_MOLD_BREAKER;
        REQUIRE(!bulbasaur.ability_active(eos::ABILITY_OVERGROW, charmander, true));
        REQUIRE(!bulbasaur.ability_active(eos::ABILITY_CHLOROPHYLL, charmander, true));
        REQUIRE(bulbasaur.ability_active(eos::ABILITY_OVERGROW, charmander, false));
        REQUIRE(bulbasaur.ability_active(eos::ABILITY_CHLOROPHYLL, charmander, false));
        REQUIRE(charmander.ability_active(eos::ABILITY_MOLD_BREAKER, charmander, true));
    }

    SECTION("exclusive_item_effect_active() works") {
        REQUIRE(!bulbasaur.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_ABSORB_FIRE_DAMAGE));
        REQUIRE(!charmander.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_ABSORB_FIRE_DAMAGE));

        bulbasaur.monster.exclusive_item_effect_flags[eos::EXCLUSIVE_EFF_ABSORB_FIRE_DAMAGE] = true;
        charmander.monster.exclusive_item_effect_flags[eos::EXCLUSIVE_EFF_ABSORB_FIRE_DAMAGE] =
            true;
        REQUIRE(bulbasaur.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_ABSORB_FIRE_DAMAGE));
        REQUIRE(!charmander.exclusive_item_effect_active(eos::EXCLUSIVE_EFF_ABSORB_FIRE_DAMAGE));
    }

    SECTION("levitate_active() works") {
        REQUIRE(!bulbasaur.levitate_active(dungeon));
        bulbasaur.monster.abilities[0] = eos::ABILITY_LEVITATE;
        REQUIRE(bulbasaur.levitate_active(dungeon));
        dungeon.gravity = true;
        REQUIRE(!bulbasaur.levitate_active(dungeon));
    }

    SECTION("has_conditional_ground_immunity() works") {
        REQUIRE(!bulbasaur.has_conditional_ground_immunity(dungeon));
        bulbasaur.monster.statuses.magnet_rise = true;
        REQUIRE(bulbasaur.has_conditional_ground_immunity(dungeon));
        dungeon.gravity = true;
        REQUIRE(!bulbasaur.has_conditional_ground_immunity(dungeon));
    }

    SECTION("is_type() works") {
        REQUIRE(bulbasaur.is_type(eos::TYPE_GRASS));
        REQUIRE(bulbasaur.is_type(eos::TYPE_POISON));
        REQUIRE(!charmander.is_type(eos::TYPE_NONE));
    }

    SECTION("perceived_weather() works") {
        REQUIRE(bulbasaur.perceived_weather(dungeon) == eos::WEATHER_CLEAR);

        dungeon.weather = eos::WEATHER_SUNNY;
        REQUIRE(bulbasaur.perceived_weather(dungeon) == eos::WEATHER_SUNNY);

        bulbasaur.monster.held_item.exists = true;
        bulbasaur.monster.held_item.id = eos::ITEM_WEATHER_BAND;
        REQUIRE(bulbasaur.perceived_weather(dungeon) == eos::WEATHER_CLEAR);

        bulbasaur.monster.held_item.sticky = true;
        REQUIRE(bulbasaur.perceived_weather(dungeon) == eos::WEATHER_SUNNY);
    }

    SECTION("nearby_ally_iq_skill_enabled() works") {
        REQUIRE(!bulbasaur.nearby_ally_iq_skill_enabled(eos::IQ_CHEERLEADER, dungeon));
        dungeon.other_monsters.iq_skill_flags[eos::IQ_CHEERLEADER] = true;
        REQUIRE(bulbasaur.nearby_ally_iq_skill_enabled(eos::IQ_CHEERLEADER, dungeon));
    }

    SECTION("other_monster_ability_active() works") {
        REQUIRE(!bulbasaur.other_monster_ability_active(eos::ABILITY_FLOWER_GIFT, dungeon));
        dungeon.other_monsters.abilities[eos::ABILITY_FLOWER_GIFT] = true;
        REQUIRE(bulbasaur.other_monster_ability_active(eos::ABILITY_FLOWER_GIFT, dungeon));
    }

    SECTION("exclusive_item_offense_boost() works") {
        REQUIRE(bulbasaur.exclusive_item_offense_boost(eos::CATEGORY_PHYSICAL) == 0);
        REQUIRE(bulbasaur.exclusive_item_offense_boost(eos::CATEGORY_SPECIAL) == 0);
        bulbasaur.monster.exclusive_item_offense_boosts[0] = 1;
        bulbasaur.monster.exclusive_item_offense_boosts[1] = 2;
        REQUIRE(bulbasaur.exclusive_item_offense_boost(eos::CATEGORY_PHYSICAL) == 1);
        REQUIRE(bulbasaur.exclusive_item_offense_boost(eos::CATEGORY_SPECIAL) == 2);
    }

    SECTION("exclusive_item_defense_boost() works") {
        REQUIRE(bulbasaur.exclusive_item_defense_boost(eos::CATEGORY_PHYSICAL) == 0);
        REQUIRE(bulbasaur.exclusive_item_defense_boost(eos::CATEGORY_SPECIAL) == 0);
        bulbasaur.monster.exclusive_item_defense_boosts[0] = 1;
        bulbasaur.monster.exclusive_item_defense_boosts[1] = 2;
        REQUIRE(bulbasaur.exclusive_item_defense_boost(eos::CATEGORY_PHYSICAL) == 1);
        REQUIRE(bulbasaur.exclusive_item_defense_boost(eos::CATEGORY_SPECIAL) == 2);
    }

    SECTION("item detection works") {
        REQUIRE(!bulbasaur.has_held_item(eos::ITEM_SILVER_BOW));
        REQUIRE(!bulbasaur.item_active(eos::ITEM_SILVER_BOW));
        REQUIRE(!bulbasaur.aura_bow_active());

        bulbasaur.monster.held_item.id = eos::ITEM_SILVER_BOW;
        REQUIRE(!bulbasaur.has_held_item(eos::ITEM_SILVER_BOW));
        REQUIRE(!bulbasaur.item_active(eos::ITEM_SILVER_BOW));
        REQUIRE(!bulbasaur.aura_bow_active());

        bulbasaur.monster.held_item.exists = true;
        REQUIRE(bulbasaur.has_held_item(eos::ITEM_SILVER_BOW));
        REQUIRE(bulbasaur.item_active(eos::ITEM_SILVER_BOW));
        REQUIRE(bulbasaur.aura_bow_active());

        bulbasaur.monster.held_item.sticky = true;
        REQUIRE(!bulbasaur.has_held_item(eos::ITEM_SILVER_BOW));
        REQUIRE(!bulbasaur.item_active(eos::ITEM_SILVER_BOW));
        REQUIRE(!bulbasaur.aura_bow_active());

        bulbasaur.monster.held_item.sticky = false;
        bulbasaur.monster.abilities[0] = eos::ABILITY_KLUTZ;
        REQUIRE(bulbasaur.has_held_item(eos::ITEM_SILVER_BOW));
        REQUIRE(!bulbasaur.item_active(eos::ITEM_SILVER_BOW));
        REQUIRE(!bulbasaur.aura_bow_active());
    }

    SECTION("has_negative_status() works") {
        REQUIRE(!bulbasaur.has_negative_status(true));

        SECTION("with a negative status") {
            bulbasaur.monster.statuses.sleep = true;
            REQUIRE(bulbasaur.has_negative_status(true));
        }
        SECTION("with a self-inflicted status") {
            bulbasaur.monster.statuses.napping = true;
            REQUIRE(!bulbasaur.has_negative_status(true));
        }
        SECTION("with the catch-all Guts/Marvel Scale status") {
            bulbasaur.monster.statuses.other_negative_status = true;
            REQUIRE(bulbasaur.has_negative_status(true));
        }
        SECTION("with Y-Ray Specs") {
            bulbasaur.monster.held_item.exists = true;
            bulbasaur.monster.held_item.id = eos::ITEM_Y_RAY_SPECS;
            charmander.monster.held_item.exists = true;
            charmander.monster.held_item.id = eos::ITEM_Y_RAY_SPECS;
            REQUIRE(!bulbasaur.has_negative_status(true));
            REQUIRE(!bulbasaur.has_negative_status(false));
            REQUIRE(charmander.has_negative_status(true));
            REQUIRE(!charmander.has_negative_status(false));
        }
    }

    SECTION("gender() works") {
        REQUIRE(bulbasaur.gender() == eos::GENDER_MALE);
        bulbasaur.monster.apparent_id = eos::MONSTER_BULBASAUR_SECONDARY;
        REQUIRE(bulbasaur.gender() == eos::GENDER_FEMALE);
        bulbasaur.monster.apparent_id = eos::MONSTER_KANGASKHAN;
        REQUIRE(bulbasaur.gender() == eos::GENDER_FEMALE);
        bulbasaur.monster.apparent_id = eos::MONSTER_DIALGA;
        REQUIRE(bulbasaur.gender() == eos::GENDER_GENDERLESS);
    }

    SECTION("get_move_type() works") {
        bulbasaur.monster.hidden_power_type = eos::TYPE_ICE;
        REQUIRE(bulbasaur.get_move_type(eos::MOVE_RAZOR_LEAF, dungeon) == eos::TYPE_GRASS);

        REQUIRE(bulbasaur.get_move_type(eos::MOVE_HIDDEN_POWER, dungeon) == eos::TYPE_ICE);

        bulbasaur.monster.held_item.exists = true;
        bulbasaur.monster.held_item.id = eos::ITEM_BLAST_SEED;
        REQUIRE(bulbasaur.get_move_type(eos::MOVE_NATURAL_GIFT, dungeon) == eos::TYPE_DRAGON);

        dungeon.weather = eos::WEATHER_SANDSTORM;
        REQUIRE(bulbasaur.get_move_type(eos::MOVE_WEATHER_BALL, dungeon) == eos::TYPE_ROCK);

        bulbasaur.monster.abilities[0] = eos::ABILITY_NORMALIZE;
        REQUIRE(bulbasaur.get_move_type(eos::MOVE_RAZOR_LEAF, dungeon) == eos::TYPE_NORMAL);
    }

    SECTION("get_move_power() works") {
        REQUIRE(bulbasaur.get_move_power(Move{eos::MOVE_RAZOR_LEAF}) == 2);
        REQUIRE(bulbasaur.get_move_power(Move{eos::MOVE_RAZOR_LEAF, 2}) == 4);

        bulbasaur.monster.held_item.exists = true;
        bulbasaur.monster.held_item.id = eos::ITEM_SPACE_GLOBE;
        REQUIRE(bulbasaur.get_move_power(Move{eos::MOVE_RAZOR_LEAF, 2}) == 8);

        bulbasaur.monster.hidden_power_base_power = 10;
        REQUIRE(bulbasaur.get_move_power(Move{eos::MOVE_HIDDEN_POWER, 1}) == 11);
    }

    SECTION("natural_gift_info() works") {
        REQUIRE(bulbasaur.natural_gift_info() == nullptr);

        bulbasaur.monster.held_item.exists = true;
        bulbasaur.monster.held_item.id = eos::ITEM_X_RAY_SPECS;
        REQUIRE(bulbasaur.natural_gift_info() == nullptr);

        bulbasaur.monster.held_item.id = eos::ITEM_SITRUS_BERRY;
        auto ng_info = bulbasaur.natural_gift_info();
        REQUIRE(ng_info);
        REQUIRE(ng_info->item_id == eos::ITEM_SITRUS_BERRY);
        REQUIRE(ng_info->type_id == eos::TYPE_PSYCHIC);
        REQUIRE(ng_info->base_power_boost == 3);
    }

    SECTION("weather_ball_type() works") {
        REQUIRE(bulbasaur.weather_ball_type(dungeon) == eos::TYPE_NORMAL);
        dungeon.weather = eos::WEATHER_SNOW;
        REQUIRE(bulbasaur.weather_ball_type(dungeon) == eos::TYPE_ICE);
    }

    SECTION("two_turn_move_forced_miss() works") {
        REQUIRE(!bulbasaur.two_turn_move_forced_miss(eos::MOVE_TACKLE));
        REQUIRE(!bulbasaur.two_turn_move_forced_miss(eos::MOVE_GUST));
        REQUIRE(!bulbasaur.two_turn_move_forced_miss(eos::MOVE_SURF));
        REQUIRE(!bulbasaur.two_turn_move_forced_miss(eos::MOVE_EARTHQUAKE));

        SECTION("when the target is flying") {
            bulbasaur.monster.statuses.flying = true;
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_TACKLE));
            REQUIRE(!bulbasaur.two_turn_move_forced_miss(eos::MOVE_GUST));
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_SURF));
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_EARTHQUAKE));
        }
        SECTION("when the target is diving") {
            bulbasaur.monster.statuses.diving = true;
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_TACKLE));
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_GUST));
            REQUIRE(!bulbasaur.two_turn_move_forced_miss(eos::MOVE_SURF));
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_EARTHQUAKE));
        }
        SECTION("when the target is digging") {
            bulbasaur.monster.statuses.digging = true;
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_TACKLE));
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_GUST));
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_SURF));
            REQUIRE(!bulbasaur.two_turn_move_forced_miss(eos::MOVE_EARTHQUAKE));
        }
        SECTION("when the target is using Shadow Force") {
            bulbasaur.monster.statuses.shadow_force = true;
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_TACKLE));
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_GUST));
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_SURF));
            REQUIRE(bulbasaur.two_turn_move_forced_miss(eos::MOVE_EARTHQUAKE));
        }
    }

    SECTION("statuses::is_valid() works") {
        REQUIRE(bulbasaur.monster.statuses.is_valid());
        bulbasaur.monster.statuses.burn = true;
        REQUIRE(bulbasaur.monster.statuses.is_valid());
        bulbasaur.monster.statuses.poison = true;
        REQUIRE(!bulbasaur.monster.statuses.is_valid());
    }
}

eos::type_matchup get_type_matchup(const DungeonState&, const MonsterEntity&, const MonsterEntity&,
                                   int, eos::type_id);
TEST_CASE("get_type_matchup() works", "[helpers]") {
    MonsterEntity a;
    MonsterEntity d;
    d.monster.types[0] = eos::TYPE_STEEL;

    DungeonState dungeon;

    REQUIRE(get_type_matchup(dungeon, a, d, 0, eos::TYPE_FIRE) == eos::MATCHUP_SUPER_EFFECTIVE);
    REQUIRE(get_type_matchup(dungeon, a, d, 0, eos::TYPE_WATER) == eos::MATCHUP_NEUTRAL);
    REQUIRE(get_type_matchup(dungeon, a, d, 0, eos::TYPE_GRASS) == eos::MATCHUP_NOT_VERY_EFFECTIVE);
    REQUIRE(get_type_matchup(dungeon, a, d, 0, eos::TYPE_POISON) == eos::MATCHUP_IMMUNE);

    SECTION("with Miracle Eye") {
        REQUIRE(get_type_matchup(dungeon, a, d, 1, eos::TYPE_PSYCHIC) == eos::MATCHUP_NEUTRAL);

        d.monster.types[1] = eos::TYPE_DARK;
        REQUIRE(get_type_matchup(dungeon, a, d, 1, eos::TYPE_PSYCHIC) == eos::MATCHUP_IMMUNE);

        d.monster.statuses.miracle_eye = true;
        REQUIRE(get_type_matchup(dungeon, a, d, 1, eos::TYPE_PSYCHIC) == eos::MATCHUP_NEUTRAL);

        d.monster.statuses.miracle_eye = false;
        a.monster.exclusive_item_effect_flags[eos::EXCLUSIVE_EFF_MIRACLE_EYE] = true;
        REQUIRE(get_type_matchup(dungeon, a, d, 1, eos::TYPE_PSYCHIC) == eos::MATCHUP_NEUTRAL);
    }

    SECTION("with Ground moves") {
        d.monster.types[1] = eos::TYPE_FLYING;
        REQUIRE(get_type_matchup(dungeon, a, d, 1, eos::TYPE_GROUND) == eos::MATCHUP_IMMUNE);

        dungeon.gravity = true;
        REQUIRE(get_type_matchup(dungeon, a, d, 1, eos::TYPE_GROUND) == eos::MATCHUP_NEUTRAL);

        dungeon.gravity = false;
        d.monster.types[1] = eos::TYPE_NORMAL;
        d.monster.statuses.magnet_rise = true;
        REQUIRE(get_type_matchup(dungeon, a, d, 1, eos::TYPE_GROUND) == eos::MATCHUP_IMMUNE);
    }
}

int16_t flash_fire_should_activate(const MonsterEntity&, const MonsterEntity&);
TEST_CASE("flash_fire_should_activate() works", "[helpers]") {
    MonsterEntity a;
    MonsterEntity d;

    REQUIRE(flash_fire_should_activate(a, d) == 0);

    d.monster.abilities[0] = eos::ABILITY_FLASH_FIRE;
    REQUIRE(flash_fire_should_activate(a, d) == 2);
    d.monster.stat_modifiers.flash_fire_boost = 2;
    REQUIRE(flash_fire_should_activate(a, d) == 1);

    a.monster.abilities[0] = eos::ABILITY_NORMALIZE;
    REQUIRE(flash_fire_should_activate(a, d) == 0);
}

bool genders_equal_not_genderless(eos::monster_id, eos::monster_id);
TEST_CASE("genders_equal_not_genderless() works", "[helpers]") {
    REQUIRE(genders_equal_not_genderless(eos::MONSTER_BULBASAUR, eos::MONSTER_CHARMANDER));
    REQUIRE(
        !genders_equal_not_genderless(eos::MONSTER_BULBASAUR_SECONDARY, eos::MONSTER_CHARMANDER));
    REQUIRE(!genders_equal_not_genderless(eos::MONSTER_DIALGA, eos::MONSTER_CHARMANDER));
    REQUIRE(!genders_equal_not_genderless(eos::MONSTER_DIALGA, eos::MONSTER_DIALGA));
}

TEST_CASE("Damage calcs with simple scenarios", "[damage_calc]") {
    DungeonState dungeon;
    dungeon.rng.variance_dial = 0.5;

    Monster charizard;
    charizard.apparent_id = eos::MONSTER_CHARIZARD;
    charizard.is_not_team_member = false;
    charizard.is_team_leader = true;
    charizard.level = 50;
    charizard.max_hp_stat = 128;
    charizard.hp = charizard.max_hp_stat;
    charizard.offensive_stats[0] = 73;
    charizard.offensive_stats[1] = 80;
    charizard.defensive_stats[0] = 69;
    charizard.defensive_stats[1] = 68;
    charizard.types[0] = eos::TYPE_FIRE;
    charizard.types[1] = eos::TYPE_FLYING;
    charizard.abilities[0] = eos::ABILITY_BLAZE;
    charizard.belly = 100;

    Monster bulbasaur;
    bulbasaur.apparent_id = eos::MONSTER_BULBASAUR;
    bulbasaur.is_not_team_member = true;
    bulbasaur.is_team_leader = false;
    bulbasaur.level = 50;
    bulbasaur.max_hp_stat = 130;
    bulbasaur.hp = bulbasaur.max_hp_stat;
    bulbasaur.offensive_stats[0] = 63;
    bulbasaur.offensive_stats[1] = 69;
    bulbasaur.defensive_stats[0] = 64;
    bulbasaur.defensive_stats[1] = 64;
    bulbasaur.types[0] = eos::TYPE_GRASS;
    bulbasaur.types[1] = eos::TYPE_POISON;
    bulbasaur.abilities[0] = eos::ABILITY_OVERGROW;
    bulbasaur.belly = 100;

    MonsterEntity attacker{charizard};
    MonsterEntity defender{bulbasaur};
    DamageData details;

    SECTION("simulate_damage_calc_generic() works") {
        int32_t damage = simulate_damage_calc_generic(
            details, dungeon, attacker, defender, eos::TYPE_WATER, 20, eos::MOVE_WATER_GUN, 10, 1);

        REQUIRE(details.damage == damage);
        REQUIRE(details.damage_message == eos::DAMAGE_MESSAGE_MOVE);
        REQUIRE(details.type_matchup == eos::MATCHUP_NOT_VERY_EFFECTIVE);
        REQUIRE(details.type == eos::TYPE_WATER);
        REQUIRE(details.category == eos::CATEGORY_SPECIAL);
        REQUIRE(!details.critical_hit);
        REQUIRE(!details.full_type_immunity);
        REQUIRE(!details.no_damage);
        REQUIRE(!details.healed);

        auto diag = dungeon.damage_calc;
        REQUIRE(diag.move_type == eos::TYPE_WATER);
        REQUIRE(diag.move_category == eos::CATEGORY_SPECIAL);
        REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_NOT_VERY_EFFECTIVE);
        REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NEUTRAL);
        REQUIRE(diag.offensive_stat_stage == 10);
        REQUIRE(diag.defensive_stat_stage == 10);
        REQUIRE(diag.offensive_stat == 80);
        REQUIRE(diag.defensive_stat == 64);
        REQUIRE(diag.flash_fire_boost == 0);
        REQUIRE(diag.offense_calc == diag.offensive_stat);
        REQUIRE(diag.defense_calc == diag.defensive_stat);
        REQUIRE(diag.attacker_level == 50);
        REQUIRE(diag.damage_calc_at == 100); // (80 + 20)
        REQUIRE(diag.damage_calc_def == 64);
        REQUIRE(diag.damage_calc_flv == 52);  // 50 + (80 - 64)/8
        REQUIRE(diag.damage_calc_base == 63); // 153/256*100 - 64/2 + 50*ln(10*(52+50)) - 311
        REQUIRE(diag.static_damage_mult.val() == 1);
        REQUIRE(diag.damage_calc == 45); // 0.70703125 * ~63.143520
        REQUIRE(diag.damage_calc_random_mult_pct == 100);

        REQUIRE(damage == 45);
        REQUIRE(dungeon.rng.get_hit_chance1().value() == 102); // trunc(trunc(100 * 1) * 1.02734375)
        REQUIRE(dungeon.rng.get_hit_chance2().value() == 90);  // trunc(trunc(88 * 1) * 1.02734375)
        REQUIRE(dungeon.rng.get_combined_hit_probability() == 0.9); // 1*0.9
    }

    SECTION("simulate_damage_calc() works") {
        int32_t damage = simulate_damage_calc(details, dungeon, attacker, defender,
                                              Move{eos::MOVE_FLAMETHROWER});

        REQUIRE(details.damage == damage);
        REQUIRE(details.damage_message == eos::DAMAGE_MESSAGE_MOVE);
        REQUIRE(details.type_matchup == eos::MATCHUP_SUPER_EFFECTIVE);
        REQUIRE(details.type == eos::TYPE_FIRE);
        REQUIRE(details.category == eos::CATEGORY_SPECIAL);
        REQUIRE(!details.critical_hit);
        REQUIRE(!details.full_type_immunity);
        REQUIRE(!details.no_damage);
        REQUIRE(!details.healed);

        auto diag = dungeon.damage_calc;
        REQUIRE(diag.move_type == eos::TYPE_FIRE);
        REQUIRE(diag.move_category == eos::CATEGORY_SPECIAL);
        REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_SUPER_EFFECTIVE);
        REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NEUTRAL);
        REQUIRE(diag.offensive_stat_stage == 10);
        REQUIRE(diag.defensive_stat_stage == 10);
        REQUIRE(diag.offensive_stat == 80);
        REQUIRE(diag.defensive_stat == 64);
        REQUIRE(diag.flash_fire_boost == 0);
        REQUIRE(diag.offense_calc == diag.offensive_stat);
        REQUIRE(diag.defense_calc == diag.defensive_stat);
        REQUIRE(diag.attacker_level == 50);
        REQUIRE(diag.damage_calc_at == 98); // (80 + 18)
        REQUIRE(diag.damage_calc_def == 64);
        REQUIRE(diag.damage_calc_flv == 52);  // 50 + (80 - 64)/8
        REQUIRE(diag.damage_calc_base == 62); // 153/256*98 - 64/2 + 50*ln(10*(52+50)) - 311
        REQUIRE(diag.static_damage_mult.val() == 1);
        REQUIRE(diag.damage_calc == 130); // 1.3984375 * 1.5 * ~61.948208
        REQUIRE(diag.damage_calc_random_mult_pct == 100);

        REQUIRE(damage == 130);
        REQUIRE(dungeon.rng.get_hit_chance1().value() == 97); // trunc(trunc(95 * 1) * 1.02734375)
        REQUIRE(dungeon.rng.get_hit_chance2().value() == 90); // trunc(trunc(88 * 1) * 1.02734375)
        REQUIRE(dungeon.rng.get_combined_hit_probability() == 0.873); // 0.97*0.9
    }

    SECTION("simulate_damage_calc_projectile() works") {
        int32_t damage = simulate_damage_calc_projectile(details, dungeon, attacker, defender, 10);

        REQUIRE(details.damage == damage);
        REQUIRE(details.damage_message == eos::DAMAGE_MESSAGE_MOVE);
        REQUIRE(details.type_matchup == eos::MATCHUP_NEUTRAL);
        REQUIRE(details.type == eos::TYPE_NONE);
        REQUIRE(details.category == eos::CATEGORY_PHYSICAL);
        REQUIRE(!details.critical_hit);
        REQUIRE(!details.full_type_immunity);
        REQUIRE(!details.no_damage);
        REQUIRE(!details.healed);

        auto diag = dungeon.damage_calc;
        REQUIRE(diag.move_type == eos::TYPE_NONE);
        REQUIRE(diag.move_category == eos::CATEGORY_PHYSICAL);
        REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_NEUTRAL);
        REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NEUTRAL);
        REQUIRE(diag.offensive_stat_stage == 10);
        REQUIRE(diag.defensive_stat_stage == 10);
        REQUIRE(diag.offensive_stat == 73);
        REQUIRE(diag.defensive_stat == 64);
        REQUIRE(diag.flash_fire_boost == 0);
        REQUIRE(diag.offense_calc == diag.offensive_stat);
        REQUIRE(diag.defense_calc == diag.defensive_stat);
        REQUIRE(diag.attacker_level == 50);
        REQUIRE(diag.damage_calc_at == 83); // (73 + 10)
        REQUIRE(diag.damage_calc_def == 64);
        REQUIRE(diag.damage_calc_flv == 51);  // 50 + (73 - 64)/8
        REQUIRE(diag.damage_calc_base == 53); // 153/256*98 - 64/2 + 50*ln(10*(51.125+50)) - 311
        REQUIRE(diag.static_damage_mult.val() == 1);
        REQUIRE(diag.damage_calc == 53);
        REQUIRE(diag.damage_calc_random_mult_pct == 100);

        REQUIRE(damage == 27);                                 // ceil(0.5 * ~52.552592)
        REQUIRE(!dungeon.rng.get_hit_chance1().has_value());   // 125, guaranteed roll
        REQUIRE(dungeon.rng.get_hit_chance2().value() == 102); // trunc(trunc(100 * 1) * 1.02734375)
        REQUIRE(dungeon.rng.get_combined_hit_probability() == 1);
    }
}

// These cases were directly observed in-game in the live dungeon struct
TEST_CASE("Damage calcs with empirically known scenarios (Charizard + Flygon)", "[damage_calc]") {
    DungeonState dungeon;
    dungeon.rng.variance_dial = 12552. / 0x4000;

    Monster charizard;
    charizard.apparent_id = eos::MONSTER_CHARIZARD;
    charizard.is_not_team_member = false;
    charizard.is_team_leader = true;
    charizard.level = 66;
    charizard.max_hp_stat = 199;
    charizard.max_hp_boost = 20;
    charizard.hp = 219;
    charizard.offensive_stats[0] = 160;
    charizard.offensive_stats[1] = 167;
    charizard.defensive_stats[0] = 158;
    charizard.defensive_stats[1] = 154;
    charizard.iq = 970;
    charizard.types[0] = eos::TYPE_FIRE;
    charizard.types[1] = eos::TYPE_FLYING;
    charizard.abilities[0] = eos::ABILITY_BLAZE;
    charizard.held_item = {true, false, eos::ITEM_X_RAY_SPECS};
    charizard.belly = {78, 600};
    charizard.iq_skill_flags[eos::IQ_ESCAPIST] = true;
    charizard.iq_skill_flags[eos::IQ_AGGRESSOR] = true;
    charizard.iq_skill_flags[eos::IQ_INTIMIDATOR] = true;
    charizard.iq_skill_flags[eos::IQ_SURVIVALIST] = true;
    charizard.iq_skill_flags[eos::IQ_EXTRA_STRIKER] = true;
    charizard.iq_skill_flags[eos::IQ_NO_CHARGER] = true;
    charizard.iq_skill_flags[eos::IQ_POWER_PITCHER] = true;
    charizard.iq_skill_flags[eos::IQ_CONCENTRATOR] = true;
    charizard.iq_skill_flags[eos::IQ_BRICK_TOUGH] = true;
    charizard.iq_skill_flags[eos::IQ_PRACTICE_SWINGER] = true;
    charizard.iq_skill_flags[eos::IQ_CLUTCH_PERFORMER] = true;
    charizard.iq_skill_flags[eos::IQ_PIERCE_HURLER] = true;
    charizard.iq_skill_flags[eos::IQ_ITEM_MASTER] = true;
    charizard.iq_skill_flags[eos::IQ_COURSE_CHECKER] = true;
    charizard.iq_skill_flags[eos::IQ_NONTRAITOR] = true;
    charizard.iq_skill_flags[eos::IQ_STATUS_CHECKER] = true;
    charizard.iq_skill_flags[eos::IQ_TYPE_ADVANTAGE_MASTER] = true;
    charizard.iq_skill_flags[eos::IQ_EXCLUSIVE_MOVE_USER] = true;
    charizard.iq_skill_flags[eos::IQ_EXP_GO_GETTER] = true;
    charizard.iq_skill_flags[eos::IQ_ITEM_CATCHER] = true;
    charizard.iq_skill_flags[eos::IQ_NONSLEEPER] = true;
    charizard.iq_skill_flags[eos::IQ_LAVA_EVADER] = true;

    Monster flygon;
    flygon.apparent_id = eos::MONSTER_FLYGON_SECONDARY;
    flygon.is_not_team_member = true;
    flygon.is_team_leader = false;
    flygon.level = 40;
    flygon.iq = 150;
    flygon.max_hp_stat = 96;
    flygon.hp = flygon.max_hp_stat;
    flygon.offensive_stats[0] = 56;
    flygon.offensive_stats[1] = 53;
    flygon.defensive_stats[0] = 43;
    flygon.defensive_stats[1] = 43;
    flygon.hidden_power_base_power = 6;
    flygon.hidden_power_type = eos::TYPE_FLYING;
    flygon.types[0] = eos::TYPE_GROUND;
    flygon.types[1] = eos::TYPE_DRAGON;
    flygon.abilities[0] = eos::ABILITY_LEVITATE;
    flygon.iq_skill_flags[eos::IQ_ITEM_CATCHER] = true;
    flygon.iq_skill_flags[eos::IQ_COURSE_CHECKER] = true;
    flygon.iq_skill_flags[eos::IQ_NONTRAITOR] = true;
    flygon.iq_skill_flags[eos::IQ_STATUS_CHECKER] = true;
    flygon.iq_skill_flags[eos::IQ_SELF_CURER] = true;
    flygon.iq_skill_flags[eos::IQ_ITEM_MASTER] = true;
    flygon.iq_skill_flags[eos::IQ_ESCAPIST] = true;
    flygon.iq_skill_flags[eos::IQ_PP_SAVER] = true;
    flygon.belly = 100;

    MonsterEntity charizard_entity{charizard};
    MonsterEntity flygon_entity{flygon};
    DamageData details;

    SECTION("Charizard -> Flygon") {
        SECTION("Flamethrower+5") {
            int32_t damage = simulate_damage_calc(details, dungeon, charizard_entity, flygon_entity,
                                                  Move{eos::MOVE_FLAMETHROWER, 5, 10});
            REQUIRE(damage == 186);

            auto diag = dungeon.damage_calc;
            REQUIRE(diag.move_type == eos::TYPE_FIRE);
            REQUIRE(diag.move_category == eos::CATEGORY_SPECIAL);
            REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NOT_VERY_EFFECTIVE);
            REQUIRE(diag.offensive_stat_stage == 11);
            REQUIRE(diag.defensive_stat_stage == 10);
            REQUIRE(diag.offensive_stat == 167);
            REQUIRE(diag.defensive_stat == 43);
            REQUIRE(diag.flash_fire_boost == 0);
            REQUIRE(diag.offense_calc == 200);
            REQUIRE(diag.defense_calc == 43);
            REQUIRE(diag.attacker_level == 66);
            REQUIRE(diag.damage_calc_at == 228);
            REQUIRE(diag.damage_calc_def == 43);
            REQUIRE(diag.damage_calc_flv == 86);
            REQUIRE(diag.damage_calc == 174);
            REQUIRE(diag.damage_calc_base == 164);
            REQUIRE(diag.damage_calc_random_mult_pct == 107);
            REQUIRE(diag.static_damage_mult.val() == 1);
            REQUIRE(diag.stab_boost_activated);
        }

        SECTION("Air Slash") {
            int32_t damage = simulate_damage_calc(details, dungeon, charizard_entity, flygon_entity,
                                                  Move{eos::MOVE_AIR_SLASH, 0, 11});
            REQUIRE(damage == 253);

            auto diag = dungeon.damage_calc;
            REQUIRE(diag.move_type == eos::TYPE_FLYING);
            REQUIRE(diag.move_category == eos::CATEGORY_SPECIAL);
            REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.offensive_stat_stage == 11);
            REQUIRE(diag.defensive_stat_stage == 10);
            REQUIRE(diag.offensive_stat == 167);
            REQUIRE(diag.defensive_stat == 43);
            REQUIRE(diag.flash_fire_boost == 0);
            REQUIRE(diag.offense_calc == 200);
            REQUIRE(diag.defense_calc == 43);
            REQUIRE(diag.attacker_level == 66);
            REQUIRE(diag.damage_calc_at == 218);
            REQUIRE(diag.damage_calc_def == 43);
            REQUIRE(diag.damage_calc_flv == 86);
            REQUIRE(diag.damage_calc == 238);
            REQUIRE(diag.damage_calc_base == 158);
            REQUIRE(diag.damage_calc_random_mult_pct == 107);
            REQUIRE(diag.static_damage_mult.val() == 1);
            REQUIRE(diag.stab_boost_activated);
        }

        SECTION("Wide Slash") {
            int32_t damage = simulate_damage_calc(details, dungeon, charizard_entity, flygon_entity,
                                                  Move{eos::MOVE_WIDE_SLASH, 0, 10});
            REQUIRE(damage == 167);

            auto diag = dungeon.damage_calc;
            REQUIRE(diag.move_type == eos::TYPE_NONE);
            REQUIRE(diag.move_category == eos::CATEGORY_PHYSICAL);
            REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.offensive_stat_stage == 11);
            REQUIRE(diag.defensive_stat_stage == 10);
            REQUIRE(diag.offensive_stat == 160);
            REQUIRE(diag.defensive_stat == 43);
            REQUIRE(diag.flash_fire_boost == 0);
            REQUIRE(diag.offense_calc == 191);
            REQUIRE(diag.defense_calc == 43);
            REQUIRE(diag.attacker_level == 66);
            REQUIRE(diag.damage_calc_at == 215);
            REQUIRE(diag.damage_calc_def == 43);
            REQUIRE(diag.damage_calc_flv == 85);
            REQUIRE(diag.damage_calc == 156);
            REQUIRE(diag.damage_calc_base == 156);
            REQUIRE(diag.damage_calc_random_mult_pct == 107);
            REQUIRE(diag.static_damage_mult.val() == 1);
            REQUIRE(!diag.stab_boost_activated);
        }

        SECTION("Heat Wave") {
            int32_t damage = simulate_damage_calc(details, dungeon, charizard_entity, flygon_entity,
                                                  Move{eos::MOVE_HEAT_WAVE, 0, 8});
            REQUIRE(damage == 175);

            auto diag = dungeon.damage_calc;
            REQUIRE(diag.move_type == eos::TYPE_FIRE);
            REQUIRE(diag.move_category == eos::CATEGORY_SPECIAL);
            REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NOT_VERY_EFFECTIVE);
            REQUIRE(diag.offensive_stat_stage == 11);
            REQUIRE(diag.defensive_stat_stage == 10);
            REQUIRE(diag.offensive_stat == 167);
            REQUIRE(diag.defensive_stat == 43);
            REQUIRE(diag.flash_fire_boost == 0);
            REQUIRE(diag.offense_calc == 200);
            REQUIRE(diag.defense_calc == 43);
            REQUIRE(diag.attacker_level == 66);
            REQUIRE(diag.damage_calc_at == 212);
            REQUIRE(diag.damage_calc_def == 43);
            REQUIRE(diag.damage_calc_flv == 86);
            REQUIRE(diag.damage_calc == 164);
            REQUIRE(diag.damage_calc_base == 155);
            REQUIRE(diag.damage_calc_random_mult_pct == 107);
            REQUIRE(diag.static_damage_mult.val() == 1);
            REQUIRE(diag.stab_boost_activated);
        }

        SECTION("regular attack") {
            dungeon.rng.variance_dial = 7691. / 0x4000;
            int32_t damage = simulate_damage_calc(details, dungeon, charizard_entity, flygon_entity,
                                                  Move{eos::MOVE_REGULAR_ATTACK});
            REQUIRE(damage == 71);

            auto diag = dungeon.damage_calc;
            REQUIRE(diag.move_type == eos::TYPE_NONE);
            REQUIRE(diag.move_category == eos::CATEGORY_PHYSICAL);
            REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.offensive_stat_stage == 11);
            REQUIRE(diag.defensive_stat_stage == 10);
            REQUIRE(diag.offensive_stat == 160);
            REQUIRE(diag.defensive_stat == 43);
            REQUIRE(diag.flash_fire_boost == 0);
            REQUIRE(diag.offense_calc == 191);
            REQUIRE(diag.defense_calc == 43);
            REQUIRE(diag.attacker_level == 66);
            REQUIRE(diag.damage_calc_at == 192);
            REQUIRE(diag.damage_calc_def == 43);
            REQUIRE(diag.damage_calc_flv == 85);
            REQUIRE(diag.damage_calc == 71);
            REQUIRE(diag.damage_calc_base == 143);
            REQUIRE(diag.damage_calc_random_mult_pct == 99);
            REQUIRE(diag.static_damage_mult.val() == 0.5);
            REQUIRE(!diag.stab_boost_activated);
        }

        SECTION("Silver Spike") {
            int32_t damage = simulate_damage_calc_projectile(details, dungeon, charizard_entity,
                                                             flygon_entity, 6);
            REQUIRE(damage == 117);

            auto diag = dungeon.damage_calc;
            REQUIRE(diag.move_type == eos::TYPE_NONE);
            REQUIRE(diag.move_category == eos::CATEGORY_PHYSICAL);
            REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.offensive_stat_stage == 11);
            REQUIRE(diag.defensive_stat_stage == 10);
            REQUIRE(diag.offensive_stat == 160);
            REQUIRE(diag.defensive_stat == 43);
            REQUIRE(diag.flash_fire_boost == 0);
            REQUIRE(diag.offense_calc == 191);
            REQUIRE(diag.defense_calc == 43);
            REQUIRE(diag.attacker_level == 66);
            REQUIRE(diag.damage_calc_at == 198);
            REQUIRE(diag.damage_calc_def == 43);
            REQUIRE(diag.damage_calc_flv == 85);
            REQUIRE(diag.damage_calc == 146);
            REQUIRE(diag.damage_calc_base == 146);
            REQUIRE(diag.damage_calc_random_mult_pct == 107);
            REQUIRE(diag.static_damage_mult.val() == 1);
            REQUIRE(!diag.stab_boost_activated);
        }

        auto diag = dungeon.damage_calc;
        REQUIRE(diag.item_atk_modifier == 0);
        REQUIRE(diag.item_sp_atk_modifier == 0);
        REQUIRE(diag.ability_offense_modifier == 0);
        REQUIRE(diag.ability_defense_modifier == 0);
        REQUIRE(diag.iq_skill_offense_modifier == 1);
        REQUIRE(diag.iq_skill_defense_modifier == 0);
        REQUIRE(diag.item_def_modifier == 0);
        REQUIRE(diag.item_sp_def_modifier == 0);
    }

    SECTION("Flygon -> Charizard") {
        SECTION("Sand Tomb") {
            dungeon.rng.variance_dial = 0.86;
            int32_t damage = simulate_damage_calc(details, dungeon, flygon_entity, charizard_entity,
                                                  Move{eos::MOVE_SAND_TOMB, 0, 17});
            REQUIRE(damage == 5);

            auto diag = dungeon.damage_calc;
            REQUIRE(diag.move_type == eos::TYPE_GROUND);
            REQUIRE(diag.move_category == eos::CATEGORY_PHYSICAL);
            REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_SUPER_EFFECTIVE);
            REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_IMMUNE);
            REQUIRE(diag.offensive_stat_stage == 10);
            REQUIRE(diag.defensive_stat_stage == 9);
            REQUIRE(diag.offensive_stat == 56);
            REQUIRE(diag.defensive_stat == 158);
            REQUIRE(diag.flash_fire_boost == 0);
            REQUIRE(diag.offense_calc == 56);
            REQUIRE(diag.defense_calc == 110);
            REQUIRE(diag.attacker_level == 40);
            REQUIRE(diag.damage_calc_at == 60);
            REQUIRE(diag.damage_calc_def == 110);
            REQUIRE(diag.damage_calc_flv == 33);
            REQUIRE(diag.damage_calc == 5);
            REQUIRE(diag.damage_calc_base == 5);
            REQUIRE(diag.damage_calc_random_mult_pct == 109);
            REQUIRE(diag.static_damage_mult.val() == 1);
            REQUIRE(diag.stab_boost_activated);
        }

        SECTION("regular attack") {
            dungeon.rng.variance_dial = 0.42;
            int32_t damage = simulate_damage_calc(details, dungeon, flygon_entity, charizard_entity,
                                                  Move{eos::MOVE_REGULAR_ATTACK});
            REQUIRE(damage == 2);

            auto diag = dungeon.damage_calc;
            REQUIRE(diag.move_type == eos::TYPE_NONE);
            REQUIRE(diag.move_category == eos::CATEGORY_PHYSICAL);
            REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.offensive_stat_stage == 10);
            REQUIRE(diag.defensive_stat_stage == 9);
            REQUIRE(diag.offensive_stat == 56);
            REQUIRE(diag.defensive_stat == 158);
            REQUIRE(diag.flash_fire_boost == 0);
            REQUIRE(diag.offense_calc == 56);
            REQUIRE(diag.defense_calc == 110);
            REQUIRE(diag.attacker_level == 40);
            REQUIRE(diag.damage_calc_at == 57);
            REQUIRE(diag.damage_calc_def == 110);
            REQUIRE(diag.damage_calc_flv == 33);
            REQUIRE(diag.damage_calc == 2);
            REQUIRE(diag.damage_calc_base == 3);
            REQUIRE(diag.damage_calc_random_mult_pct == 98);
            REQUIRE(diag.static_damage_mult.val() == 0.5);
            REQUIRE(!diag.stab_boost_activated);
        }

        SECTION("Silver Spike") {
            dungeon.rng.variance_dial = 13640. / 0x4000;
            int32_t damage = simulate_damage_calc_projectile(details, dungeon, flygon_entity,
                                                             charizard_entity, 6);
            REQUIRE(damage == 3);

            auto diag = dungeon.damage_calc;
            REQUIRE(diag.move_type == eos::TYPE_NONE);
            REQUIRE(diag.move_category == eos::CATEGORY_PHYSICAL);
            REQUIRE(diag.move_indiv_type_matchups[0] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.move_indiv_type_matchups[1] == eos::MATCHUP_NEUTRAL);
            REQUIRE(diag.offensive_stat_stage == 10);
            REQUIRE(diag.defensive_stat_stage == 9);
            REQUIRE(diag.offensive_stat == 56);
            REQUIRE(diag.defensive_stat == 158);
            REQUIRE(diag.flash_fire_boost == 0);
            REQUIRE(diag.offense_calc == 56);
            REQUIRE(diag.defense_calc == 110);
            REQUIRE(diag.attacker_level == 40);
            REQUIRE(diag.damage_calc_at == 62);
            REQUIRE(diag.damage_calc_def == 110);
            REQUIRE(diag.damage_calc_flv == 33);
            REQUIRE(diag.damage_calc == 5);
            REQUIRE(diag.damage_calc_base == 5);
            REQUIRE(diag.damage_calc_random_mult_pct == 108);
            REQUIRE(diag.static_damage_mult.val() == 1);
            REQUIRE(!diag.stab_boost_activated);
        }

        auto diag = dungeon.damage_calc;
        REQUIRE(diag.item_atk_modifier == 0);
        REQUIRE(diag.item_sp_atk_modifier == 0);
        REQUIRE(diag.ability_offense_modifier == 0);
        REQUIRE(diag.ability_defense_modifier == 0);
        REQUIRE(diag.iq_skill_offense_modifier == 0);
        REQUIRE(diag.iq_skill_defense_modifier == -1);
        REQUIRE(diag.item_def_modifier == 0);
        REQUIRE(diag.item_sp_def_modifier == 0);
    }

    auto diag = dungeon.damage_calc;
    REQUIRE(!diag.scope_lens_or_sharpshooter_activated);
    REQUIRE(!diag.patsy_band_activated);
    REQUIRE(!diag.half_physical_damage_activated);
    REQUIRE(!diag.half_special_damage_activated);
    REQUIRE(!diag.focus_energy_activated);
    REQUIRE(!diag.type_advantage_master_activated);
    REQUIRE(!diag.cloudy_drop_activated);
    REQUIRE(!diag.rain_multiplier_activated);
    REQUIRE(!diag.sunny_multiplier_activated);
    REQUIRE(!diag.fire_move_ability_drop_activated);
    REQUIRE(!diag.flash_fire_activated);
    REQUIRE(!diag.levitate_activated);
    REQUIRE(!diag.torrent_boost_activated);
    REQUIRE(!diag.overgrow_boost_activated);
    REQUIRE(!diag.swarm_boost_activated);
    REQUIRE(!diag.fire_move_ability_boost_activated);
    REQUIRE(!diag.scrappy_activated);
    REQUIRE(!diag.super_luck_activated);
    REQUIRE(!diag.sniper_activated);
    REQUIRE(!diag.electric_move_dampened);
    REQUIRE(!diag.water_sport_drop_activated);
    REQUIRE(!diag.charge_boost_activated);
    REQUIRE(!diag.ghost_immunity_activated);
    REQUIRE(!diag.skull_bash_defense_boost_activated);
}
