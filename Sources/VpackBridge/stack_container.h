//
// Created by ktiays on 2022/8/14.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_STACK_CONTAINER_H
#define VPACKCORE_STACK_CONTAINER_H

#include <algorithm>

#include "container.h"
#include "indexed.h"
#include "types.h"

namespace vpk::core {

template <typename Identifier, typename ValueType>
class StackContainer : public Container<Identifier, ValueType> {
public:
    StackContainer(const std::vector<LayoutablePointer<Identifier, ValueType>>& children, const LayoutParams<ValueType>& params, Alignment align)
        : Container<Identifier, ValueType>(children, params), alignment(align) {
        const SizeProperty<ValueType> size_property = params.size_property;
        __DEAL_MIN_WIDTH_FOR_POLICY(MinMaxPolicy::max);
        __DEAL_MAX_WIDTH_FOR_POLICY(MinMaxPolicy::max);
        __DEAL_MIN_HEIGHT_FOR_POLICY(MinMaxPolicy::max);
        __DEAL_MAX_HEIGHT_FOR_POLICY(MinMaxPolicy::max);
    }

    void layout(const Rect<ValueType>& frame, LayoutResult<Identifier, ValueType>& result) const override;

    Size<ValueType> measure(const Size<ValueType>& size) override;

private:
    Alignment alignment;
};

template <typename Identifier, typename ValueType>
void StackContainer<Identifier, ValueType>::layout(const Rect<ValueType>& frame, LayoutResult<Identifier, ValueType>& result) const {
    // Layout in terms of the actual space occupied by the elements.
    const Point<ValueType> origin = {frame.x + (frame.width - this->cached_measured_size.width) / 2, frame.y + (frame.height - this->cached_measured_size.height) / 2};
    const Size<ValueType> size = {std::max(frame.width, this->cached_measured_size.width), std::max(frame.height, this->cached_measured_size.height)};

    for (const auto it : makeIndexed(this->children)) {
        const auto index = it.index();
        const auto child = it.value();
        const EdgeInsets<ValueType> padding = child->padding();

        const Size<ValueType> item_size = this->size_list.at(index);
        const Size<ValueType> item_container_size = {item_size.width + padding.horizontal(), item_size.height + padding.vertical()};
        const ValueType x_offset = [this, &size, &item_container_size]() -> ValueType {
            switch (this->alignment.horizontal()) {
            case HorizontalAlignment::leading:
                return 0;
            case HorizontalAlignment::center:
                return (size.width - item_container_size.width) / 2;
            case HorizontalAlignment::trailing:
                return size.width - item_container_size.width;
            }
        }();
        const ValueType y_offset = [this, &size, &item_container_size]() -> ValueType {
            switch (this->alignment.vertical()) {
            case VerticalAlignment::top:
                return 0;
            case VerticalAlignment::center:
                return (size.height - item_container_size.height) / 2;
            case VerticalAlignment::bottom:
                return size.height - item_container_size.height;
            }
        }();
        const Point<ValueType> item_offset = child->offset();
        const Rect<ValueType> layout_frame = Rect<ValueType>{
            origin.x + x_offset + padding.left + item_offset.x,
            origin.y + y_offset + padding.top + item_offset.y,
            item_size.width,
            item_size.height,
        };

        // Lift z-index of the child.
        result.max_z_idx += 1;
        child->layout(layout_frame, result);
    }
}

template <typename Identifier, typename ValueType>
Size<ValueType> StackContainer<Identifier, ValueType>::measure(const Size<ValueType>& size) {
    Size<ValueType> measured_size;
    // The element sizes in `StackContainer` are not affected by each other.
    // Therefore, priority map is not used here.
    for (auto it : makeIndexed(this->children)) {
        const auto child = it.value();

        const EdgeInsets<ValueType> padding = child->padding();

        Size<ValueType> item_size = child->preferred_size(child->measure(child->preferred_size({size.width - padding.horizontal(), size.height - padding.vertical()})));
        this->size_list.at(it.index()) = item_size;
        measured_size = Size<ValueType>{std::max(item_size.width + padding.horizontal(), measured_size.width), std::max(item_size.height + padding.vertical(), measured_size.height)};
    }

    this->cached_measured_size = measured_size;
    return measured_size;
}

} // namespace vpk::core

#endif //VPACKCORE_STACK_CONTAINER_H
