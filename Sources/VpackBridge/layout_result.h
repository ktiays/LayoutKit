//
// Created by ktiays on 2022/8/12.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_LAYOUT_RESULT_H
#define VPACKCORE_LAYOUT_RESULT_H

#include <unordered_map>

#include "types.h"
#include "optional.h"

namespace vpk::core {

template<typename ValueType>
struct LayoutAttributes {
    Rect<ValueType> frame;
    uint16_t z_idx;

    inline bool operator ==(const LayoutAttributes& attr) const {
        return z_idx == attr.z_idx && frame == attr.frame;
    }

    inline bool operator !=(const LayoutAttributes& attr) const {
        return !(*this == attr);
    }
};

template<typename Identifier, typename ValueType>
struct LayoutResult {
    std::unordered_map<Identifier, LayoutAttributes<ValueType>> map;
    uint16_t max_z_idx;

    LayoutResult()
        : max_z_idx(0) {}

    LayoutResult(const std::unordered_map<Identifier, LayoutAttributes<ValueType>>& m, uint16_t z_idx)
        : map(m), max_z_idx(z_idx) {}

    LayoutResult(std::unordered_map<Identifier, LayoutAttributes<ValueType>>&& m, uint16_t z_idx)
        : map(std::move(m)), max_z_idx(z_idx) {}
};

template<typename Identifier, typename ValueType>
bool
operator ==(const LayoutResult<Identifier, ValueType>& r1, const LayoutResult<Identifier, ValueType>& r2) noexcept {
    if (r1.max_z_idx != r2.max_z_idx) return false;
    if (r1.map.size() != r2.map.size()) return false;
    return std::all_of(r1.map.begin(), r2.map.end(), [&r2](const auto& it) {
        const auto iter = r2.map.find(it.first);
        if (iter == r2.map.end()) return false;
        if (iter->second != it.second) return false;
        return true;
    });
}

}

#endif //VPACKCORE_LAYOUT_RESULT_H
