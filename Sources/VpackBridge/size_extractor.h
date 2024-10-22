//
// Created by ktiays on 2022/8/13.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_SIZE_EXTRACTOR_H
#define VPACKCORE_SIZE_EXTRACTOR_H

#include <version>
#include <numeric>
#include <algorithm>
#include <functional>

#include "layoutable.h"

namespace vpk::core {

enum class MinMaxPolicy {
    sum,
    max
};

enum class SizeExtractor {
    min_width,
    max_width,
    min_height,
    max_height,
};

template<SizeExtractor se, typename Identifier, typename ValueType>
ValueType extract(const LayoutablePointer<Identifier, ValueType>& ptr) {
    const EdgeInsets<ValueType> padding = ptr->padding();
    switch (se) {
        case SizeExtractor::min_width:
            return ptr->min_width() + padding.horizontal();
        case SizeExtractor::max_width:
            return ptr->max_width() + padding.horizontal();
        case SizeExtractor::min_height:
            return ptr->min_height() + padding.vertical();
        case SizeExtractor::max_height:
            return ptr->max_height() + padding.vertical();
    }
}

template<SizeExtractor se, typename Identifier, typename ValueType>
ValueType calculate_min_max_dimension(const std::vector<LayoutablePointer<Identifier, ValueType>>& items,
                                      const MinMaxPolicy& policy) {
    std::vector<ValueType> dimensions;
    std::transform(items.begin(), items.end(), std::back_inserter(dimensions),
                   [](const auto& it) -> ValueType {
                       return extract<se>(it);
                   });
    switch (policy) {
        case MinMaxPolicy::sum:
            return std::accumulate(dimensions.begin(), dimensions.end(), static_cast<ValueType>(0), std::plus<>());
        case MinMaxPolicy::max: {
            auto iter = std::max_element(dimensions.begin(), dimensions.end());
            if (iter != dimensions.end()) return *iter;
            else return static_cast<ValueType>(0);
        }
    }
}

} // vpk

#endif //VPACKCORE_SIZE_EXTRACTOR_H
