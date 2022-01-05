#pragma once

#include <map>

/*
| key_type	  | Key                     |
| mapped_type |	T                       |
| value_type  | std::pair<const Key, T> |
*/
template <typename map_t, typename key_t = typename map_t::key_type, typename value_t = typename map_t::mapped_type>
value_t map_get_value(const map_t& map, const key_t& key) {
    auto pos = map.find(key);
    return (pos != map.end()) ? (pos->second) : (value_t{});
}

template <typename map_t, typename key_t = typename map_t::key_type, typename value_t = typename map_t::mapped_type>
value_t map_get_value(const map_t& map, const key_t& key, value_t&& default_value) {
    auto pos = map.find(key);
    return (pos != map.end()) ? (pos->second) : value_t(std::forward<value_t>(default_value));
}