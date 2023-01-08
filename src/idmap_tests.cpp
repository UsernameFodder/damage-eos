#include <catch2/catch_test_macros.hpp>
#include "idmap.hpp"

TEST_CASE("Names can be mapped to IDs") {
    REQUIRE(ids::MOVE["heat wave"] == eos::MOVE_HEAT_WAVE);
    REQUIRE(ids::MONSTER["latios"] == eos::MONSTER_LATIOS);
    REQUIRE(ids::TYPE["dragon"] == eos::TYPE_DRAGON);
    REQUIRE(ids::ABILITY["speed boost"] == eos::ABILITY_SPEED_BOOST);
    REQUIRE(ids::ITEM["x-ray specs"] == eos::ITEM_X_RAY_SPECS);
    REQUIRE(ids::GENDER["genderless"] == eos::GENDER_GENDERLESS);
    REQUIRE(ids::WEATHER["clear"] == eos::WEATHER_CLEAR);
    REQUIRE(ids::IQ["absolute mover"] == eos::IQ_ABSOLUTE_MOVER);
    REQUIRE(ids::STATUS["invisible"] == eos::STATUS_INVISIBLE);
    REQUIRE(ids::EXCLUSIVE_ITEM_EFFECT["halved damage"] == eos::EXCLUSIVE_EFF_HALVED_DAMAGE);
    REQUIRE(ids::MOVE_CATEGORY["special"] == eos::CATEGORY_SPECIAL);
    REQUIRE(ids::TYPE_MATCHUP["super effective"] == eos::MATCHUP_SUPER_EFFECTIVE);
    REQUIRE(ids::DAMAGE_MESSAGE["move"] == eos::DAMAGE_MESSAGE_MOVE);
}
TEST_CASE("IDs can be mapped to names") {
    REQUIRE(ids::MOVE[eos::MOVE_HEAT_WAVE] == "heat wave");
    REQUIRE(ids::MONSTER[eos::MONSTER_LATIOS] == "latios");
    REQUIRE(ids::TYPE[eos::TYPE_DRAGON] == "dragon");
    REQUIRE(ids::ABILITY[eos::ABILITY_SPEED_BOOST] == "speed boost");
    REQUIRE(ids::ITEM[eos::ITEM_X_RAY_SPECS] == "x-ray specs");
    REQUIRE(ids::GENDER[eos::GENDER_GENDERLESS] == "genderless");
    REQUIRE(ids::WEATHER[eos::WEATHER_CLEAR] == "clear");
    REQUIRE(ids::IQ[eos::IQ_ABSOLUTE_MOVER] == "absolute mover");
    REQUIRE(ids::STATUS[eos::STATUS_INVISIBLE] == "invisible");
    REQUIRE(ids::EXCLUSIVE_ITEM_EFFECT[eos::EXCLUSIVE_EFF_HALVED_DAMAGE] == "halved damage");
    REQUIRE(ids::MOVE_CATEGORY[eos::CATEGORY_SPECIAL] == "special");
    REQUIRE(ids::TYPE_MATCHUP[eos::MATCHUP_SUPER_EFFECTIVE] == "super effective");
    REQUIRE(ids::DAMAGE_MESSAGE[eos::DAMAGE_MESSAGE_MOVE] == "move");
}
