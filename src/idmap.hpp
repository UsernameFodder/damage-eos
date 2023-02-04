// Utility for mapping between names and internal IDs for various enums used by the game

#ifndef IDMAPS_HPP_
#define IDMAPS_HPP_

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "pmdsky.hpp"

namespace ids {
template <typename T> class IDMap {
    std::unordered_map<std::string, T> name_to_id;             // contains normalized names
    std::unordered_map<T, std::string> id_to_name;             // contains raw names
    std::unordered_map<T, std::vector<std::string>> alt_names; // contains raw names

    static char ascii_to_lower(char c) {
        if (c >= 'A' && c <= 'Z') {
            return c - ('A' - 'a');
        }
        return c;
    }
    std::vector<std::pair<T, std::string>> sorted_pairs() const {
        std::vector<std::pair<T, std::string>> pairs(id_to_name.begin(), id_to_name.end());
        std::sort(pairs.begin(), pairs.end());
        return pairs;
    }

  public:
    IDMap(std::initializer_list<std::pair<T, std::string>> pairs) {
        name_to_id.reserve(pairs.size());
        id_to_name.reserve(pairs.size());
        for (auto& [id, name] : pairs) {
            std::string normalized = normalize(name);
            if (name_to_id.find(normalized) != name_to_id.end()) {
                throw std::invalid_argument("IDMap: repeated name '" + name + "' for IDs " +
                                            std::to_string(name_to_id.at(normalized)) + ", " +
                                            std::to_string(id));
            }
            if (id_to_name.find(id) != id_to_name.end()) {
                throw std::invalid_argument("IDMap: repeated ID " + std::to_string(id) +
                                            " for names '" + id_to_name.at(id) + "', '" + name +
                                            "'");
            }
            name_to_id[normalized] = id;
            id_to_name[id] = name;
        }
    }
    IDMap(std::initializer_list<std::tuple<T, std::string, std::vector<std::string>>> entries) {
        name_to_id.reserve(entries.size());
        id_to_name.reserve(entries.size());
        alt_names.reserve(entries.size());
        for (auto& [id, name, alts] : entries) {
            std::string normalized = normalize(name);
            if (name_to_id.find(normalized) != name_to_id.end()) {
                throw std::invalid_argument("IDMap: repeated name '" + name + "' for IDs " +
                                            std::to_string(name_to_id.at(normalized)) + ", " +
                                            std::to_string(id));
            }
            if (id_to_name.find(id) != id_to_name.end()) {
                throw std::invalid_argument("IDMap: repeated ID " + std::to_string(id) +
                                            " for names '" + id_to_name.at(id) + "', '" + name +
                                            "'");
            }
            name_to_id[normalized] = id;
            id_to_name[id] = name;

            for (auto& alt : alts) {
                normalized = normalize(alt);
                if (name_to_id.find(normalized) != name_to_id.end()) {
                    throw std::invalid_argument("IDMap: repeated name '" + alt + "' for IDs " +
                                                std::to_string(name_to_id.at(normalized)) + ", " +
                                                std::to_string(id));
                }
                name_to_id[normalized] = id;
            }
            alt_names[id] = std::move(alts);
        }
    }

    const std::string& operator[](const T id) const { return id_to_name.at(id); }
    const T operator[](const std::string& name) const { return name_to_id.at(normalize(name)); }
    const std::vector<std::string> alternate_names(const T id) const { return alt_names.at(id); }

    bool contains(const std::string& name) const {
        return name_to_id.find(normalize(name)) != name_to_id.end();
    }

    std::vector<std::string> all_except(std::unordered_set<T> exclude = {}) const {
        auto pairs = sorted_pairs();
        std::vector<std::string> names;
        names.reserve(pairs.size());
        for (auto& [id, name] : pairs) {
            if (exclude.find(id) == exclude.end()) {
                names.push_back(name);
            }
        }
        return names;
    }
    std::vector<std::pair<std::string, std::vector<std::string>>>
    all_with_alts_except(std::unordered_set<T> exclude = {}) const {
        auto pairs = sorted_pairs();
        std::vector<std::pair<std::string, std::vector<std::string>>> names;
        names.reserve(pairs.size());
        for (auto& [id, name] : pairs) {
            if (exclude.find(id) == exclude.end()) {
                auto alts = alt_names.find(id);
                if (alts != alt_names.end()) {
                    names.push_back({name, alts->second});
                } else {
                    names.push_back({name, {}});
                }
            }
        }
        return names;
    }

    std::string normalize(const std::string& name) const {
        // For case insensitive lookup
        std::string normalized = name;
        std::transform(normalized.begin(), normalized.end(), normalized.begin(), ascii_to_lower);
        return normalized;
    }
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
