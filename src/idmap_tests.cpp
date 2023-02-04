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
TEST_CASE("Name-to-ID mapping is case insensitive") {
    REQUIRE(ids::MOVE["HEAT WAVE"] == eos::MOVE_HEAT_WAVE);
    REQUIRE(ids::MONSTER["Latios"] == eos::MONSTER_LATIOS);
    REQUIRE(ids::TYPE["dRaGoN"] == eos::TYPE_DRAGON);
}
TEST_CASE("IDs can be mapped to names") {
    REQUIRE(ids::MOVE[eos::MOVE_HEAT_WAVE] == "Heat Wave");
    REQUIRE(ids::MONSTER[eos::MONSTER_LATIOS] == "Latios");
    REQUIRE(ids::TYPE[eos::TYPE_DRAGON] == "Dragon");
    REQUIRE(ids::ABILITY[eos::ABILITY_SPEED_BOOST] == "Speed Boost");
    REQUIRE(ids::ITEM[eos::ITEM_X_RAY_SPECS] == "X-Ray Specs");
    REQUIRE(ids::GENDER[eos::GENDER_GENDERLESS] == "Genderless");
    REQUIRE(ids::WEATHER[eos::WEATHER_CLEAR] == "Clear");
    REQUIRE(ids::IQ[eos::IQ_ABSOLUTE_MOVER] == "Absolute Mover");
    REQUIRE(ids::STATUS[eos::STATUS_INVISIBLE] == "Invisible");
    REQUIRE(ids::EXCLUSIVE_ITEM_EFFECT[eos::EXCLUSIVE_EFF_HALVED_DAMAGE] == "Halved damage");
    REQUIRE(ids::MOVE_CATEGORY[eos::CATEGORY_SPECIAL] == "Special");
    REQUIRE(ids::TYPE_MATCHUP[eos::MATCHUP_SUPER_EFFECTIVE] == "Super effective");
    REQUIRE(ids::DAMAGE_MESSAGE[eos::DAMAGE_MESSAGE_MOVE] == "Move");
}
TEST_CASE("Alternate names can be mapped to IDs") {
    REQUIRE(ids::STATUS["asleep"] == eos::STATUS_SLEEP);
    REQUIRE(ids::EXCLUSIVE_ITEM_EFFECT["no paralysis"] == eos::EXCLUSIVE_EFF_NO_PARALYSIS);
    REQUIRE(ids::EXCLUSIVE_ITEM_EFFECT["volt bangle"] == eos::EXCLUSIVE_EFF_NO_PARALYSIS);
    REQUIRE(ids::EXCLUSIVE_ITEM_EFFECT["moving scarf"] == eos::EXCLUSIVE_EFF_NO_PARALYSIS);
    REQUIRE(ids::EXCLUSIVE_ITEM_EFFECT["magne-torc"] == eos::EXCLUSIVE_EFF_NO_PARALYSIS);
    REQUIRE(ids::EXCLUSIVE_ITEM_EFFECT["mobile bow"] == eos::EXCLUSIVE_EFF_NO_PARALYSIS);
}
TEST_CASE("IDs can be mapped to alternate names") {
    REQUIRE(ids::STATUS.alternate_names(eos::STATUS_SLEEP) == std::vector<std::string>{"Asleep"});
    REQUIRE(ids::EXCLUSIVE_ITEM_EFFECT.alternate_names(eos::EXCLUSIVE_EFF_NO_PARALYSIS) ==
            std::vector<std::string>{"Volt Bangle", "Moving Scarf", "Magne-Torc", "Mobile Bow"});
}
TEST_CASE("contains() works") {
    REQUIRE(ids::MOVE.contains("heat wave"));
    REQUIRE(!ids::MOVE.contains("latios"));
}
TEST_CASE("all_except() returns an ordered list of names") {
    std::vector<std::string> all_statuses = ids::STATUS.all_except();
    REQUIRE(all_statuses.front() == "None");
    REQUIRE(all_statuses.back() == "Stair Spotter");
    REQUIRE(all_statuses.size() == 102);
}
TEST_CASE("all_except() excludes given entries") {
    std::vector<std::string> all_statuses =
        ids::STATUS.all_except({eos::STATUS_NONE, eos::STATUS_STAIR_SPOTTER});
    REQUIRE(all_statuses.front() == "Sleep");
    REQUIRE(all_statuses.back() == "Doubled Attack");
    REQUIRE(all_statuses.size() == 100);
}

TEST_CASE("all_with_alts_except() returns an ordered list of names") {
    auto all_statuses = ids::STATUS.all_with_alts_except();
    REQUIRE(all_statuses.front().first == "None");
    REQUIRE(all_statuses.front().second == std::vector<std::string>{"-"});
    REQUIRE(all_statuses.back().first == "Stair Spotter");
    REQUIRE(all_statuses.back().second == std::vector<std::string>{"Can locate stairs"});
    REQUIRE(all_statuses.size() == 102);

    auto all_effs = ids::EXCLUSIVE_ITEM_EFFECT.all_with_alts_except();
    REQUIRE(all_effs.front().first == "Stat boost");
    REQUIRE(all_effs.front().second.size() == 0);
    REQUIRE(all_effs.back().first == "last");
    REQUIRE(all_effs.back().second.size() == 0);
    REQUIRE(all_effs.size() == 130);
}
TEST_CASE("all_with_alts_except() excludes given entries") {
    auto all_statuses =
        ids::STATUS.all_with_alts_except({eos::STATUS_NONE, eos::STATUS_STAIR_SPOTTER});
    REQUIRE(all_statuses.front().first == "Sleep");
    REQUIRE(all_statuses.front().second == std::vector<std::string>{"Asleep"});
    REQUIRE(all_statuses.back().first == "Doubled Attack");
    REQUIRE(all_statuses.back().second == std::vector<std::string>{"Has sped-up attacks"});
    REQUIRE(all_statuses.size() == 100);

    auto all_effs = ids::EXCLUSIVE_ITEM_EFFECT.all_with_alts_except(
        {eos::EXCLUSIVE_EFF_STAT_BOOST, eos::EXCLUSIVE_EFF_LAST});
    REQUIRE(all_effs.front().first == "No Paralysis");
    REQUIRE(all_effs.front().second ==
            std::vector<std::string>{"Volt Bangle", "Moving Scarf", "Magne-Torc", "Mobile Bow"});
    REQUIRE(all_effs.back().first == "Absorb Steel damage");
    REQUIRE(all_effs.back().second ==
            std::vector<std::string>{"Fake Torc", "Cradily Bow", "Skull Helmet", "Glacier Cape"});
    REQUIRE(all_effs.size() == 128);
}
