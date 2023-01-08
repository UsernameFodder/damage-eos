#include <catch2/catch_test_macros.hpp>
#include "cfgparse.hpp"

using nlohmann::json;

TEST_CASE("Fixed-width integers are validated") {
    REQUIRE(json_get_int<int8_t>({{"int8", 1}}, "int8") == 1);
    REQUIRE(json_get_int<int8_t>({{"int8", -1}}, "int8") == -1);
    REQUIRE_THROWS(json_get_int<int8_t>({{"int8", 200}}, "int8"));
    REQUIRE_THROWS(json_get_int<int8_t>({{"int8", -200}}, "int8"));
    REQUIRE_THROWS(json_get_int<uint8_t>({{"uint8", -1}}, "uint8"));
}

Fx32 json_get_fx32(const json&, const std::string, double default_val = 0);
TEST_CASE("Doubles can be parsed as Fx32") {
    REQUIRE(json_get_fx32({{"fx32", 0.5}}, "fx32").get_raw() == Fx32::CONST_0_5.get_raw());
    REQUIRE_THROWS(json_get_fx32({{"fx32", -1e7}}, "fx32"));
    REQUIRE_THROWS(json_get_fx32({{"fx32", 1e7}}, "fx32"));
}

DungeonState parse_dungeon_cfg(const json&, const json&);
TEST_CASE("Dungeon object is parsed correctly") {
    auto dungeon = parse_dungeon_cfg(
        {
            {"weather", "sunny"},
            {"mud_sport", true},
            {"water_sport", true},
            {"gravity", true},
            {"plus",
             {
                 {"team", true},
                 {"enemy", true},
             }},
            {"minus",
             {
                 {"team", true},
                 {"enemy", true},
             }},
            {"iq_disabled", true},
            {"fixed_room_id", 1},
            {"other_monsters",
             {
                 {"iq", {"cheerleader", "bodyguard"}},
                 {"abilities", {"flower gift", "mold breaker"}},
             }},
        },
        {
            {"huge_pure_power", true},
            {"critical_hit", true},
        });

    REQUIRE(dungeon.weather == eos::WEATHER_SUNNY);
    REQUIRE(dungeon.mud_sport_turns > 0);
    REQUIRE(dungeon.water_sport_turns > 0);
    REQUIRE(dungeon.gravity);
    REQUIRE(dungeon.plus_is_active[0]);
    REQUIRE(dungeon.plus_is_active[1]);
    REQUIRE(dungeon.minus_is_active[0]);
    REQUIRE(dungeon.minus_is_active[1]);
    REQUIRE(dungeon.iq_disabled);
    REQUIRE(dungeon.gen_info.fixed_room_id == eos::FIXED_BEACH_CAVE_PIT);
    REQUIRE(dungeon.other_monsters.iq_skill_flags[eos::IQ_CHEERLEADER]);
    REQUIRE(dungeon.other_monsters.iq_skill_flags[eos::IQ_BODYGUARD]);
    REQUIRE(dungeon.other_monsters.abilities[eos::ABILITY_FLOWER_GIFT]);
    REQUIRE(dungeon.other_monsters.abilities[eos::ABILITY_MOLD_BREAKER]);
    REQUIRE(dungeon.rng.huge_pure_power);
    REQUIRE(dungeon.rng.critical_hit);
}

MonsterEntity parse_monster_cfg(const json&);
TEST_CASE("Monster object is parsed correctly") {
    auto monster =
        parse_monster_cfg({
                              {"species", "bronzong"},
                              {"is_team_member", true},
                              {"is_team_leader", true},
                              {"level", 100},
                              {"hp", 5},
                              {"max_hp", 10},
                              {"atk", 15},
                              {"sp_atk", 20},
                              {"def", 25},
                              {"sp_def", 30},
                              {"stat_modifiers",
                               {
                                   {"stages",
                                    {
                                        {"atk", 1},
                                        {"sp_atk", 2},
                                        {"def", 3},
                                        {"sp_def", 4},
                                        {"accuracy", 5},
                                        {"evasion", 6},
                                        {"speed", 0},
                                        {"stockpile", 1},
                                    }},
                                   {"multipliers",
                                    {
                                        {"atk", 0.5},
                                        {"sp_atk", 1.4},
                                        {"def", 1.5},
                                        {"sp_def", 1.7},
                                    }},
                                   {"flash_fire_boost", 2},
                               }},
                              {"iq", 200},
                              {"iq_skills", {"item master", "exclusive move-user"}},
                              {"belly", 90},
                              {"hidden_power_type", "fire"},
                              {"hidden_power_base_power", 42},
                              {"held_item",
                               {
                                   {"id", "stick"},
                                   {"sticky", true},
                               }},
                              {"exclusive_items",
                               {
                                   {"effects", {"absorb fire damage", "no flying damage"}},
                                   {"stat_boosts",
                                    {
                                        {"atk", 11},
                                        {"sp_atk", 12},
                                        {"def", 13},
                                        {"sp_def", 14},
                                    }},
                               }},
                              {"statuses", {"burn", "guts/marvel scale"}},
                              {"boosted_attacks", true},
                              {"practice_swinger", true},
                              {"anger_point", true},
                              {"n_moves_out_of_pp", 4},
                          })
            .monster;

    REQUIRE(monster.apparent_id == eos::MONSTER_BRONZONG);
    REQUIRE(monster.types[0] == eos::TYPE_STEEL);
    REQUIRE(monster.types[1] == eos::TYPE_PSYCHIC);
    REQUIRE(monster.abilities[0] == eos::ABILITY_LEVITATE);
    REQUIRE(monster.abilities[1] == eos::ABILITY_HEATPROOF);
    REQUIRE(!monster.is_not_team_member);
    REQUIRE(monster.is_team_leader);
    REQUIRE(monster.level == 100);
    REQUIRE(monster.hp == 5);
    REQUIRE(monster.max_hp_stat == 10);
    REQUIRE(monster.offensive_stats[0] == 15);
    REQUIRE(monster.offensive_stats[1] == 20);
    REQUIRE(monster.defensive_stats[0] == 25);
    REQUIRE(monster.defensive_stats[1] == 30);
    REQUIRE(monster.stat_modifiers.offensive_stages[0] == 1);
    REQUIRE(monster.stat_modifiers.offensive_stages[1] == 2);
    REQUIRE(monster.stat_modifiers.defensive_stages[0] == 3);
    REQUIRE(monster.stat_modifiers.defensive_stages[1] == 4);
    REQUIRE(monster.stat_modifiers.hit_chance_stages[0] == 5);
    REQUIRE(monster.stat_modifiers.hit_chance_stages[1] == 6);
    REQUIRE(monster.statuses.speed_stage == 0);
    REQUIRE(monster.statuses.stockpile_stage == 1);
    REQUIRE(monster.stat_modifiers.offensive_multipliers[0].get_raw() == Fx32::CONST_0_5.get_raw());
    REQUIRE(monster.stat_modifiers.offensive_multipliers[1].get_raw() == Fx32::CONST_1_4.get_raw());
    REQUIRE(monster.stat_modifiers.defensive_multipliers[0].get_raw() == Fx32::CONST_1_5.get_raw());
    REQUIRE(monster.stat_modifiers.defensive_multipliers[1].get_raw() == Fx32::CONST_1_7.get_raw());
    REQUIRE(monster.stat_modifiers.flash_fire_boost == 2);
    REQUIRE(monster.iq == 200);
    REQUIRE(monster.iq_skill_flags[eos::IQ_ITEM_MASTER]);
    REQUIRE(monster.iq_skill_flags[eos::IQ_EXCLUSIVE_MOVE_USER]);
    REQUIRE(monster.belly.ceil() == 90);
    REQUIRE(monster.hidden_power_type == eos::TYPE_FIRE);
    REQUIRE(monster.hidden_power_base_power == 42);
    REQUIRE(monster.held_item.exists);
    REQUIRE(monster.held_item.id == eos::ITEM_STICK);
    REQUIRE(monster.held_item.sticky);
    REQUIRE(monster.exclusive_item_effect_flags[eos::EXCLUSIVE_EFF_ABSORB_FIRE_DAMAGE]);
    REQUIRE(monster.exclusive_item_effect_flags[eos::EXCLUSIVE_EFF_NO_FLYING_DAMAGE]);
    REQUIRE(monster.exclusive_item_offense_boosts[0] == 11);
    REQUIRE(monster.exclusive_item_offense_boosts[1] == 12);
    REQUIRE(monster.exclusive_item_defense_boosts[0] == 13);
    REQUIRE(monster.exclusive_item_defense_boosts[1] == 14);
    REQUIRE(monster.statuses.burn);
    REQUIRE(monster.statuses.other_negative_status);
    REQUIRE(monster.boosted_attacks);
    REQUIRE(monster.practice_swinger_flag);
    REQUIRE(monster.anger_point_flag);
    REQUIRE(monster.n_moves_out_of_pp == 4);
}

std::pair<Move, int32_t> parse_move_cfg(const json&);
TEST_CASE("Move object is parsed correctly") {
    auto [move, power] = parse_move_cfg({
        {"id", "heat wave"},
        {"ginseng", 1},
        {"pp", 2},
        {"prior_successive_hits", 3},
    });

    REQUIRE(move.id == eos::MOVE_HEAT_WAVE);
    REQUIRE(move.ginseng == 1);
    REQUIRE(move.pp == 2);
    REQUIRE(move.prior_successive_hits == 3);
    REQUIRE(power == mechanics::get_move_base_power(eos::MOVE_HEAT_WAVE));
}
TEST_CASE("Move object is parsed correctly with a thrown item") {
    auto [move, power] = parse_move_cfg({{"id", "corsola twig"}});

    REQUIRE(move.id == eos::MOVE_PROJECTILE);
    REQUIRE(move.ginseng == 0);
    REQUIRE(move.pp == 0);
    REQUIRE(power == 15);
}

std::tuple<DungeonState, MonsterEntity, MonsterEntity, Move, int32_t> parse_cfg(const json&);
TEST_CASE("Top-level config is parsed correctly") {
    auto [dungeon, attacker, defender, move, power] = parse_cfg({
        {"move", {{"id", "tackle"}}},
        {"attacker", {{"species", "charmander"}}},
        {"defender", {{"species", "bulbasaur"}}},
        {"dungeon", {{"mud_sport", true}}},
        {"rng", {{"critical_hit", true}}},
    });

    CHECK(move.id == eos::MOVE_TACKLE);
    CHECK(attacker.monster.apparent_id == eos::MONSTER_CHARMANDER);
    CHECK(defender.monster.apparent_id == eos::MONSTER_BULBASAUR);
    CHECK(dungeon.mud_sport_turns > 0);
    CHECK(dungeon.rng.critical_hit);
    CHECK(power == mechanics::get_move_base_power(eos::MOVE_TACKLE));
}
