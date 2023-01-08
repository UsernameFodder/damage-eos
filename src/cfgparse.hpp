// Config parsing utilities to convert between JSON and the internal data representations used by
// the damage calculator reimplementation

#ifndef CFGPARSE_HPP_
#define CFGPARSE_HPP_

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <nlohmann/json.hpp>
#include "damage.hpp"

template <typename T>
T json_get_int(const nlohmann::json& obj, const std::string name, T default_val = 0) {
    std::int64_t v = obj.value<int64_t>(name, default_val);
    if (v < std::numeric_limits<T>::min()) {
        throw std::underflow_error(name + ": " + std::to_string(v) + " out of range (min " +
                                   std::to_string(std::numeric_limits<T>::min()) + ")");
    }
    if (v > std::numeric_limits<T>::max()) {
        throw std::overflow_error(name + ": " + std::to_string(v) + " out of range (max " +
                                  std::to_string(std::numeric_limits<T>::max()) + ")");
    }
    return static_cast<T>(v);
}

std::tuple<DungeonState, MonsterEntity, MonsterEntity, Move, int32_t>
parse_cfg(const nlohmann::json& cfg);

#endif
