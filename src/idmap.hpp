// Utility for mapping between names and internal IDs for various enums used by the game

#ifndef IDMAPS_HPP_
#define IDMAPS_HPP_

#include <initializer_list>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "pmdsky.hpp"

namespace ids {
template <typename T> class IDMap {
    std::unordered_map<std::string, T> name_to_id;
    std::unordered_map<T, std::string> id_to_name;

  public:
    IDMap(std::initializer_list<std::pair<T, std::string>> pairs) {
        name_to_id.reserve(pairs.size());
        id_to_name.reserve(pairs.size());
        for (auto& [id, name] : pairs) {
            if (name_to_id.find(name) != name_to_id.end()) {
                throw std::invalid_argument("IDMap: repeated name '" + name + "' for IDs " +
                                            std::to_string(name_to_id.at(name)) + ", " +
                                            std::to_string(id));
            }
            if (id_to_name.find(id) != id_to_name.end()) {
                throw std::invalid_argument("IDMap: repeated ID " + std::to_string(id) +
                                            " for names '" + id_to_name.at(id) + "', '" + name +
                                            "'");
            }
            name_to_id[name] = id;
            id_to_name[id] = name;
        }
    }

    const std::string& operator[](const T id) const { return id_to_name.at(id); }
    const T operator[](const std::string& name) const { return name_to_id.at(name); }
};

extern const IDMap<eos::move_id> MOVE;
extern const IDMap<eos::monster_id> MONSTER;
extern const IDMap<eos::type_id> TYPE;
extern const IDMap<eos::ability_id> ABILITY;
extern const IDMap<eos::item_id> ITEM;
extern const IDMap<eos::monster_gender> GENDER;
extern const IDMap<eos::weather_id> WEATHER;
extern const IDMap<eos::iq_skill_id> IQ;
extern const IDMap<eos::status_id> STATUS;
extern const IDMap<eos::exclusive_item_effect_id> EXCLUSIVE_ITEM_EFFECT;
extern const IDMap<eos::move_category> MOVE_CATEGORY;
extern const IDMap<eos::type_matchup> TYPE_MATCHUP;
extern const IDMap<eos::damage_message> DAMAGE_MESSAGE;
}; // namespace ids

#endif
