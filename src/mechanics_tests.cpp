#include <catch2/catch_test_macros.hpp>
#include "mechanics.hpp"

using namespace mechanics;

TEST_CASE("get_move_type() works", "[moves]") {
    REQUIRE(get_move_type(eos::MOVE_HEAT_WAVE) == eos::TYPE_FIRE);
    REQUIRE(get_move_type(eos::MOVE_SPACIAL_REND) == eos::TYPE_DRAGON);
}
TEST_CASE("get_move_category() works", "[moves]") {
    REQUIRE(get_move_category(eos::MOVE_TACKLE) == eos::CATEGORY_PHYSICAL);
    REQUIRE(get_move_category(eos::MOVE_HEAT_WAVE) == eos::CATEGORY_SPECIAL);
    REQUIRE(get_move_category(eos::MOVE_STUN_SPORE) == eos::CATEGORY_STATUS);
}
TEST_CASE("get_move_base_power() works", "[moves]") {
    REQUIRE(get_move_base_power(eos::MOVE_QUICK_ATTACK) == 4);
    REQUIRE(get_move_base_power(eos::MOVE_FRENZY_PLANT) == 45);
}
TEST_CASE("get_move_accuracy() works", "[moves]") {
    REQUIRE(get_move_accuracy(eos::MOVE_HYDRO_CANNON, false) == 125);
    REQUIRE(get_move_accuracy(eos::MOVE_HYDRO_CANNON, true) == 84);
}
TEST_CASE("get_move_crit_chance() works", "[moves]") {
    REQUIRE(get_move_crit_chance(eos::MOVE_THUNDERSHOCK) == 2);
    REQUIRE(get_move_crit_chance(eos::MOVE_SLASH) == 30);
}
TEST_CASE("get_move_max_pp() works", "[moves]") {
    REQUIRE(get_move_max_pp(eos::MOVE_AERIAL_ACE) == 8);
    REQUIRE(get_move_max_pp(eos::MOVE_LEER) == 27);
}
TEST_CASE("is_recoil_move() works", "[moves]") {
    REQUIRE(!is_recoil_move(eos::MOVE_GIGA_DRAIN));
    REQUIRE(is_recoil_move(eos::MOVE_DOUBLE_EDGE));
}
TEST_CASE("is_punch_move() works", "[moves]") {
    REQUIRE(!is_punch_move(eos::MOVE_SILVER_WIND));
    REQUIRE(is_punch_move(eos::MOVE_THUNDERPUNCH));
}
TEST_CASE("is_sound_move() works", "[moves]") {
    REQUIRE(!is_sound_move(eos::MOVE_CRABHAMMER));
    REQUIRE(is_sound_move(eos::MOVE_HYPER_VOICE));
}
TEST_CASE("move_not_physical() works", "[moves]") {
    REQUIRE(!move_not_physical(eos::MOVE_ROLLOUT));
    REQUIRE(move_not_physical(eos::MOVE_PSYBEAM));
    REQUIRE(move_not_physical(eos::MOVE_HYPNOSIS));
}
TEST_CASE("is_regular_attack_or_projectile() works", "[moves]") {
    REQUIRE(!is_regular_attack_or_projectile(eos::MOVE_PSYCHIC));
    REQUIRE(is_regular_attack_or_projectile(eos::MOVE_REGULAR_ATTACK));
    REQUIRE(is_regular_attack_or_projectile(eos::MOVE_PROJECTILE));
}
TEST_CASE("MoveSpec can be constructed from move ID", "[moves]") {
    auto move = MoveSpec(eos::MOVE_ROAR_OF_TIME);
    REQUIRE(move.id == eos::MOVE_ROAR_OF_TIME);
    REQUIRE(move.type == eos::TYPE_DRAGON);
    REQUIRE(move.power == 24);
    REQUIRE(move.pp == 4);
}

TEST_CASE("is_aura_bow() works", "[items]") {
    REQUIRE(!is_aura_bow(eos::ITEM_POWER_BAND));
    REQUIRE(is_aura_bow(eos::ITEM_SILVER_BOW));
    REQUIRE(is_aura_bow(eos::ITEM_FUCHSIA_BOW));
}

TEST_CASE("get_monster_gender() works", "[monsters]") {
    REQUIRE(get_monster_gender(eos::MONSTER_BULBASAUR) == eos::GENDER_MALE);
    REQUIRE(get_monster_gender(eos::MONSTER_BULBASAUR_SECONDARY) == eos::GENDER_FEMALE);
    REQUIRE(get_monster_gender(eos::MONSTER_KANGASKHAN) == eos::GENDER_FEMALE);
    REQUIRE(get_monster_gender(eos::MONSTER_KANGASKHAN_SECONDARY) == eos::GENDER_INVALID);
    REQUIRE(get_monster_gender(eos::MONSTER_DIALGA) == eos::GENDER_GENDERLESS);
    REQUIRE(get_monster_gender(eos::MONSTER_DIALGA_SECONDARY) == eos::GENDER_INVALID);
}
TEST_CASE("get_monster_weight() works", "[monsters]") {
    REQUIRE(get_monster_weight(eos::MONSTER_BULBASAUR).val() == 0.59765625);
    REQUIRE(get_monster_weight(eos::MONSTER_BULBASAUR_SECONDARY).val() == 0.59765625);
    REQUIRE(get_monster_weight(eos::MONSTER_VENUSAUR).val() == 1.296875);
}
TEST_CASE("SpeciesSpec can be constructed from monster ID", "[monsters]") {
    auto monster = SpeciesSpec(eos::MONSTER_LATIOS);
    REQUIRE(monster.id == eos::MONSTER_LATIOS);
    REQUIRE(monster.types[0] == eos::TYPE_DRAGON);
    REQUIRE(monster.types[1] == eos::TYPE_PSYCHIC);
    REQUIRE(monster.abilities[0] == eos::ABILITY_LEVITATE);
    REQUIRE(monster.abilities[1] == eos::ABILITY_UNKNOWN);
}
