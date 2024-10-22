//
// Created by ktiays on 2022/8/13.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_HORIZONTAL_CONTAINER_H
#define VPACKCORE_HORIZONTAL_CONTAINER_H

#include <algorithm>

#include "hv_container.h"
#include "types.h"

namespace vpk::core {

template <typename Identifier, typename ValueType>
class HorizontalContainer : public detail::HVContainer<Identifier, ValueType> {
public:
    HorizontalContainer(const std::vector<LayoutablePointer<Identifier, ValueType>>& children, const LayoutParams<ValueType>& params, VerticalAlignment align)
        : detail::HVContainer<Identifier, ValueType>(children, params), alignment(align) {
        const SizeProperty<ValueType> size_property = params.size_property;
        __DEAL_MIN_WIDTH_FOR_POLICY(MinMaxPolicy::sum);
        __DEAL_MAX_WIDTH_FOR_POLICY(MinMaxPolicy::sum);
        __DEAL_MIN_HEIGHT_FOR_POLICY(MinMaxPolicy::max);
        __DEAL_MAX_HEIGHT_FOR_POLICY(MinMaxPolicy::max);
    }

    using Element = typename detail::HVContainer<Identifier, ValueType>::Element;

    detail::AxisEdgeInsets<ValueType> axis_edge_insets_for_element(Element element) const override {
        const EdgeInsets<ValueType> padding = element->padding();
        return {padding.left, padding.right, padding.top, padding.bottom};
    }

    ValueType min_main_for_element(Element element) const override {
        return element->min_width();
    }

    ValueType max_main_for_element(Element element) const override {
        return element->max_width();
    }

    ValueType min_cross_for_element(Element element) const override {
        return element->min_height();
    }

    ValueType max_cross_for_element(Element element) const override {
        return element->max_height();
    }

    Point<ValueType> point_from_axis_point(const detail::AxisPoint<ValueType>& point) const override {
        return {point.main, point.cross};
    }

    detail::AxisPoint<ValueType> axis_point_from_point(const Point<ValueType>& point) const override {
        return {point.x, point.y};
    }

    Size<ValueType> size_from_axis_size(const detail::AxisSize<ValueType>& size) const override {
        return {size.main, size.cross};
    };

    detail::AxisSize<ValueType> axis_size_from_size(const Size<ValueType>& size) const override {
        return {size.width, size.height};
    };

    detail::AxisAlignment axis_alignment() const override {
        switch (alignment) {
        case VerticalAlignment::top:
            return detail::AxisAlignment::start;
        case VerticalAlignment::center:
            return detail::AxisAlignment::center;
        case VerticalAlignment::bottom:
            return detail::AxisAlignment::end;
        }
    }

private:
    VerticalAlignment alignment;
};

} // namespace vpk::core

#endif //VPACKCORE_HORIZONTAL_CONTAINER_H
