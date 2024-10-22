//
// Created by ktiays on 2022/8/13.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_CONTAINER_H
#define VPACKCORE_CONTAINER_H

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include "indexed.h"
#include "layoutable.h"
#include "size_extractor.h"

#define __DEAL_MIN_WIDTH_ELSE(property) \
    if (size_property.property.has_value()) \
        this->property##_ = *size_property.property; \
    else \
        this->property##_ =

#define __DEAL_SIZE_FOR_POLICY_WITH_PROPERTY(property, policy) \
    __DEAL_MIN_WIDTH_ELSE(property) calculate_min_max_dimension<SizeExtractor::property, Identifier, ValueType>(children, policy)

#define __DEAL_MIN_WIDTH_FOR_POLICY(policy) __DEAL_SIZE_FOR_POLICY_WITH_PROPERTY(min_width, policy)
#define __DEAL_MAX_WIDTH_FOR_POLICY(policy) __DEAL_SIZE_FOR_POLICY_WITH_PROPERTY(max_width, policy)
#define __DEAL_MIN_HEIGHT_FOR_POLICY(policy) __DEAL_SIZE_FOR_POLICY_WITH_PROPERTY(min_height, policy)
#define __DEAL_MAX_HEIGHT_FOR_POLICY(policy) __DEAL_SIZE_FOR_POLICY_WITH_PROPERTY(max_height, policy)

namespace vpk::core {

template <typename Identifier, typename ValueType>
class Container : public Layoutable<Identifier, ValueType> {
private:
    using ElementPointer = LayoutablePointer<Identifier, ValueType>;
    using ElementListType = std::vector<ElementPointer>;
    using ElementSizeType = typename ElementListType::size_type;

public:
    Container(const std::vector<LayoutablePointer<Identifier, ValueType>>& items, const LayoutParams<ValueType>& params)
        : Layoutable<Identifier, ValueType>(params), children(items) {
        size_list.resize(items.size());

        for (auto it : makeIndexed(items)) {
            const ElementPointer& ptr = it.value();
            children_priority_map[ptr->params.priority].push_back(std::make_pair(it.index(), ptr));
        }
    }

protected:
    ElementListType children;
    /// A map that sorts the child elements by layout priority.
    ///
    /// The map is sorted in descending order of priority.
    std::map<int, std::vector<std::pair<ElementSizeType, ElementPointer>>, std::greater<int>> children_priority_map;
    // The size list of the element calculated by the cache.
    // The size indicates the actual display size of the element, i.e., the size without padding.
    std::vector<Size<ValueType>> size_list;

    /// A cache of the results of the last size calculation.
    ///
    /// It represents the actual total size that all child elements need to occupy.
    /// Default value of this property is 0.
    Size<ValueType> cached_measured_size;
};

} // namespace vpk::core

#endif //VPACKCORE_CONTAINER_H
