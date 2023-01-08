#include "cfgparse.hpp"
#include "idmap.hpp"

using nlohmann::json;

// See the top-level sample-config.json for example config

Fx32 json_get_fx32(const json& obj, const std::string name, double default_val = 0) {
    double v = obj.value(name, default_val);
    if (v < -(1 << 23)) {
        throw std::underflow_error(name + ": " + std::to_string(v) + " out of range (min " +
                                   std::to_string(-(1 << 23)) + ")");
    }
    int32_t fx32_max = ((1 << 23) - 1) + ((1 << 8) - 1) / static_cast<double>(1 << 8);
    if (v > fx32_max) {
        throw std::overflow_error(name + ": " + std::to_string(v) + " out of range (max " +
                                  std::to_string(fx32_max) + ")");
    }

    uint32_t ipart = static_cast<uint32_t>(v);
    uint8_t fpart = static_cast<uint8_t>((v - ipart) * (1 << 8));
    return Fx32{ipart, fpart};
}

DungeonState parse_dungeon_cfg(const json& dungeon_obj, const json& rng_obj) {
    DungeonState dungeon = {};

    if (dungeon_obj.find("weather") != dungeon_obj.end()) {
        dungeon.weather = ids::WEATHER[dungeon_obj.at("weather").get<std::string>()];
    }
    dungeon.mud_sport_turns = dungeon_obj.value("mud_sport", false) ? 1 : 0;
    dungeon.water_sport_turns = dungeon_obj.value("water_sport", false) ? 1 : 0;
    dungeon.gravity = dungeon_obj.value("gravity", false);
    dungeon.iq_disabled = dungeon_obj.value("iq_disabled", false);
    dungeon.gen_info.fixed_room_id = eos::fixed_room_id(dungeon_obj.value("fixed_room_id", 0));

    const json& plus = dungeon_obj.value("plus", json({}));
    dungeon.plus_is_active[0] = plus.value("enemy", false);
    dungeon.plus_is_active[1] = plus.value("team", false);
    const json& minus = dungeon_obj.value("minus", json({}));
    dungeon.minus_is_active[0] = minus.value("enemy", false);
    dungeon.minus_is_active[1] = minus.value("team", false);

    const json& other_monsters = dungeon_obj.value("other_monsters", json({}));
    const json& iq_array = other_monsters.value("iq", json::array());
    for (auto& iq : iq_array) {
        dungeon.other_monsters.iq_skill_flags[ids::IQ[iq.get<std::string>()]] = true;
    }
    const json& ability_array = other_monsters.value("abilities", json::array());
    for (auto& ability : ability_array) {
        dungeon.other_monsters.abilities[ids::ABILITY[ability.get<std::string>()]] = true;
    }

    dungeon.rng.huge_pure_power = rng_obj.value("huge_pure_power", false);
    dungeon.rng.critical_hit = rng_obj.value("critical_hit", false);

    return dungeon;
}
MonsterEntity parse_monster_cfg(const json& monster_obj) {
    Monster monster = {};

    const json& species = monster_obj.at("species");
    eos::monster_id monster_id;
    if (species.is_string()) {
        monster_id = ids::MONSTER[species.get<std::string>()];
    } else {
        throw std::runtime_error("custom species not implemented");
    }
    monster.apparent_id = monster_id;
    mechanics::SpeciesSpec mdata(monster_id);
    monster.types[0] = mdata.types[0];
    monster.types[1] = mdata.types[1];
    monster.abilities[0] = mdata.abilities[0];
    monster.abilities[1] = mdata.abilities[1];

    monster.is_not_team_member = !monster_obj.value("is_team_member", false);
    monster.is_team_leader = monster_obj.value("is_team_leader", false);
    monster.level = json_get_int<uint8_t>(monster_obj, "level", 1);
    monster.max_hp_stat = json_get_int<int16_t>(monster_obj, "max_hp", 1);
    monster.hp = json_get_int<int16_t>(monster_obj, "hp", monster.max_hp_stat);
    monster.offensive_stats[0] = json_get_int<uint8_t>(monster_obj, "atk", 1);
    monster.offensive_stats[1] = json_get_int<uint8_t>(monster_obj, "sp_atk", 1);
    monster.defensive_stats[0] = json_get_int<uint8_t>(monster_obj, "def", 1);
    monster.defensive_stats[1] = json_get_int<uint8_t>(monster_obj, "sp_def", 1);
    monster.iq = json_get_int<int16_t>(monster_obj, "iq");
    monster.belly = DecFx16_16(json_get_int<int16_t>(monster_obj, "belly", 100));

    const json& stat_modifiers = monster_obj.value("stat_modifiers", json({}));
    const json& stages = stat_modifiers.value("stages", json({}));
    monster.stat_modifiers.offensive_stages[0] = json_get_int<int16_t>(stages, "atk", 10);
    monster.stat_modifiers.offensive_stages[1] = json_get_int<int16_t>(stages, "sp_atk", 10);
    monster.stat_modifiers.defensive_stages[0] = json_get_int<int16_t>(stages, "def", 10);
    monster.stat_modifiers.defensive_stages[1] = json_get_int<int16_t>(stages, "sp_def", 10);
    monster.stat_modifiers.hit_chance_stages[0] = json_get_int<int16_t>(stages, "accuracy", 10);
    monster.stat_modifiers.hit_chance_stages[1] = json_get_int<int16_t>(stages, "evasion", 10);
    monster.statuses.speed_stage = json_get_int<int32_t>(stages, "speed", 1);
    monster.statuses.stockpile_stage = json_get_int<uint8_t>(stages, "stockpile");
    const json& multipliers = stat_modifiers.value("multipliers", json({}));
    monster.stat_modifiers.offensive_multipliers[0] = json_get_fx32(multipliers, "atk", 1);
    monster.stat_modifiers.offensive_multipliers[1] = json_get_fx32(multipliers, "sp_atk", 1);
    monster.stat_modifiers.defensive_multipliers[0] = json_get_fx32(multipliers, "def", 1);
    monster.stat_modifiers.defensive_multipliers[1] = json_get_fx32(multipliers, "sp_def", 1);
    monster.stat_modifiers.flash_fire_boost =
        json_get_int<int16_t>(stat_modifiers, "flash_fire_boost");
    monster.boosted_attacks = monster_obj.value("boosted_attacks", false);
    monster.practice_swinger_flag = monster_obj.value("practice_swinger", false);
    monster.anger_point_flag = monster_obj.value("anger_point", false);
    monster.n_moves_out_of_pp = json_get_int<uint8_t>(monster_obj, "n_moves_out_of_pp");

    monster.hidden_power_base_power =
        json_get_int<int16_t>(monster_obj, "hidden_power_base_power", 1);
    if (monster_obj.find("hidden_power_type") != monster_obj.end()) {
        monster.hidden_power_type =
            ids::TYPE[monster_obj.at("hidden_power_type").get<std::string>()];
    }

    const json& held_item = monster_obj.value("held_item", json({}));
    if (held_item.find("id") != held_item.end()) {
        monster.held_item.id = ids::ITEM[held_item.at("id").get<std::string>()];
        if (monster.held_item.id != eos::ITEM_NOTHING) {
            monster.held_item.exists = true;
        }
    }
    monster.held_item.sticky = held_item.value("sticky", false);

    const json& iq_skills = monster_obj.value("iq_skills", json::array());
    for (auto& iq : iq_skills) {
        monster.iq_skill_flags[ids::IQ[iq.get<std::string>()]] = true;
    }

    const json& exclusive_items = monster_obj.value("exclusive_items", json({}));
    const json& effects = exclusive_items.value("effects", json::array());
    for (auto& eff : effects) {
        monster.exclusive_item_effect_flags[ids::EXCLUSIVE_ITEM_EFFECT[eff.get<std::string>()]] =
            true;
    }
    const json& stat_boosts = exclusive_items.value("stat_boosts", json({}));
    monster.exclusive_item_offense_boosts[0] = json_get_int<uint8_t>(stat_boosts, "atk");
    monster.exclusive_item_offense_boosts[1] = json_get_int<uint8_t>(stat_boosts, "sp_atk");
    monster.exclusive_item_defense_boosts[0] = json_get_int<uint8_t>(stat_boosts, "def");
    monster.exclusive_item_defense_boosts[1] = json_get_int<uint8_t>(stat_boosts, "sp_def");

    const json& statuses = monster_obj.value("statuses", json::array());
    for (auto& status : statuses) {
        std::string status_name = status.get<std::string>();
        // special case
        if (status_name == "guts/marvel scale") {
            monster.statuses.other_negative_status = true;
            continue;
        }
        switch (ids::STATUS[status_name]) {
        case eos::STATUS_SLEEP:
            monster.statuses.sleep = true;
            break;
        case eos::STATUS_NIGHTMARE:
            monster.statuses.nightmare = true;
            break;
        case eos::STATUS_NAPPING:
            monster.statuses.napping = true;
            break;
        case eos::STATUS_BURN:
            monster.statuses.burn = true;
            break;
        case eos::STATUS_POISONED:
            monster.statuses.poison = true;
            break;
        case eos::STATUS_BADLY_POISONED:
            monster.statuses.bad_poison = true;
            break;
        case eos::STATUS_PARALYSIS:
            monster.statuses.paralysis = true;
            break;
        case eos::STATUS_IDENTIFYING:
            monster.statuses.identifying = true;
            break;
        case eos::STATUS_CONFUSED:
            monster.statuses.confusion = true;
            break;
        case eos::STATUS_SKULL_BASH:
            monster.statuses.skull_bash = true;
            break;
        case eos::STATUS_FLYING:
            monster.statuses.flying = true;
            break;
        case eos::STATUS_BOUNCING:
            monster.statuses.bouncing = true;
            break;
        case eos::STATUS_DIVING:
            monster.statuses.diving = true;
            break;
        case eos::STATUS_DIGGING:
            monster.statuses.digging = true;
            break;
        case eos::STATUS_SHADOW_FORCE:
            monster.statuses.shadow_force = true;
            break;
        case eos::STATUS_CHARGING:
            monster.statuses.charge = true;
            break;
        case eos::STATUS_REFLECT:
            monster.statuses.reflect = true;
            break;
        case eos::STATUS_LIGHT_SCREEN:
            monster.statuses.light_screen = true;
            break;
        case eos::STATUS_LUCKY_CHANT:
            monster.statuses.lucky_chant = true;
            break;
        case eos::STATUS_GASTRO_ACID:
            monster.statuses.gastro_acid = true;
            break;
        case eos::STATUS_SURE_SHOT:
            monster.statuses.sure_shot = true;
            break;
        case eos::STATUS_WHIFFER:
            monster.statuses.whiffer = true;
            break;
        case eos::STATUS_FOCUS_ENERGY:
            monster.statuses.focus_energy = true;
            break;
        case eos::STATUS_CROSS_EYED:
            monster.statuses.cross_eyed = true;
            break;
        case eos::STATUS_MIRACLE_EYE:
            monster.statuses.miracle_eye = true;
            break;
        case eos::STATUS_MAGNET_RISE:
            monster.statuses.magnet_rise = true;
            break;
        case eos::STATUS_EXPOSED:
            monster.statuses.exposed = true;
            break;
        default:
            throw std::invalid_argument("unsupported status '" + status_name + "'");
        }
    }

    return MonsterEntity{monster};
}
std::pair<Move, int32_t> parse_move_cfg(const json& move_obj) {
    const json& id = move_obj.at("id");
    eos::move_id move_id;

    int32_t base_power;
    if (id.is_string()) {
        std::string str = id.get<std::string>();
        if (str == ids::ITEM[eos::ITEM_STICK]) {
            move_id = eos::MOVE_PROJECTILE;
            base_power = mechanics::STICK_POWER;
        } else if (str == ids::ITEM[eos::ITEM_IRON_THORN]) {
            move_id = eos::MOVE_PROJECTILE;
            base_power = mechanics::IRON_THORN_POWER;
        } else if (str == ids::ITEM[eos::ITEM_SILVER_SPIKE]) {
            move_id = eos::MOVE_PROJECTILE;
            base_power = mechanics::SILVER_SPIKE_POWER;
        } else if (str == ids::ITEM[eos::ITEM_GOLD_FANG]) {
            move_id = eos::MOVE_PROJECTILE;
            base_power = mechanics::GOLD_FANG_POWER;
        } else if (str == ids::ITEM[eos::ITEM_CACNEA_SPIKE]) {
            move_id = eos::MOVE_PROJECTILE;
            base_power = mechanics::CACNEA_SPIKE_POWER;
        } else if (str == ids::ITEM[eos::ITEM_CORSOLA_TWIG]) {
            move_id = eos::MOVE_PROJECTILE;
            base_power = mechanics::CORSOLA_TWIG_POWER;
        } else if (str == ids::ITEM[eos::ITEM_GOLD_THORN]) {
            move_id = eos::MOVE_PROJECTILE;
            base_power = mechanics::GOLD_THORN_POWER;
        } else {
            move_id = ids::MOVE[str];
            base_power = mechanics::get_move_base_power(move_id);
        }
    } else {
        throw std::runtime_error("custom moves not implemented");
    }

    return {Move{move_id, json_get_int<uint8_t>(move_obj, "ginseng"),
                 json_get_int<uint8_t>(move_obj, "pp"),
                 json_get_int<uint8_t>(move_obj, "prior_successive_hits")},
            base_power};
}

std::tuple<DungeonState, MonsterEntity, MonsterEntity, Move, int32_t> parse_cfg(const json& cfg) {
    DungeonState dungeon =
        parse_dungeon_cfg(cfg.value("dungeon", json({})), cfg.value("rng", json({})));
    MonsterEntity attacker = parse_monster_cfg(cfg.at("attacker"));
    MonsterEntity defender = parse_monster_cfg(cfg.at("defender"));
    auto [move, power] = parse_move_cfg(cfg.at("move"));

    return {dungeon, attacker, defender, move, power};
}
