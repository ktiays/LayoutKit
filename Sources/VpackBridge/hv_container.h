//
// Created by ktiays on 2022/8/22.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_HV_CONTAINER_H
#define VPACKCORE_HV_CONTAINER_H

#include "container.h"

namespace vpk::core::detail {

enum class AxisAlignment {
    start,
    center,
    end
};

#define __AXIS_STRUCT_DECL(struct_type) \
template<typename ValueType>            \
struct Axis##struct_type {              \
    ValueType main;                     \
    ValueType cross;                    \
                                        \
    constexpr Axis##struct_type()       \
        : main(0), cross(0) {}          \
                                        \
    constexpr Axis##struct_type(const ValueType& main, const ValueType& cross) \
        : main(main), cross(cross) {}   \
}

__AXIS_STRUCT_DECL(Point);

__AXIS_STRUCT_DECL(Size);

template<typename ValueType>
struct AxisEdgeInsets {
    ValueType main_start;
    ValueType main_end;
    ValueType cross_start;
    ValueType cross_end;

    constexpr AxisEdgeInsets()
        : main_start(0), main_end(0), cross_start(0), cross_end(0) {}

    constexpr AxisEdgeInsets(const ValueType& main_start, const ValueType& main_end, const ValueType& cross_start,
                             const ValueType& cross_end)
        : main_start(main_start), main_end(main_end), cross_start(cross_start), cross_end(cross_end) {}

    /// The total insets in the main axis direction.
    ValueType main() const {
        return main_start + main_end;
    }

    /// The total insets in the cross axis direction.
    ValueType cross() const {
        return cross_start + cross_end;
    }
};

template<typename Identifier, typename ValueType>
struct HVContainer : public vpk::core::Container<Identifier, ValueType> {
    HVContainer(const std::vector<LayoutablePointer<Identifier, ValueType>>& items,
                const LayoutParams<ValueType>& params)
        : vpk::core::Container<Identifier, ValueType>(items, params) {}

protected:
    using Element = LayoutablePointer<Identifier, ValueType>;

    using SizeType = AxisSize<ValueType>;

    /// Subclasses need to implement this method to provide information
    /// about the main and cross axes of the edge insets.
    ///
    /// \brief Returns the corresponding insets instance according to the current layout direction.
    ///
    /// \param element Pointer to a subclass of Layoutable.
    /// \return Instances of AxisEdgeInsets.
    virtual AxisEdgeInsets<ValueType> axis_edge_insets_for_element(Element element) const = 0;

    virtual ValueType min_main_for_element(Element element) const = 0;

    virtual ValueType max_main_for_element(Element element) const = 0;

    virtual ValueType min_cross_for_element(Element element) const = 0;

    virtual ValueType max_cross_for_element(Element element) const = 0;

    /// The alignment of the container on the main axis.
    virtual AxisAlignment axis_alignment() const = 0;

    /// Convert from cross-axis point to regular point.
    ///
    /// This method needs to be implemented by a subclass.
    /// \param point A cross-axis based point.
    /// \return A regular point.
    virtual Point<ValueType> point_from_axis_point(const AxisPoint<ValueType>& point) const = 0;

    /// Convert from regular point to cross-axis point.
    ///
    /// This method needs to be implemented by a subclass.
    /// \param point A regular point.
    /// \return A cross-axis based point.
    virtual AxisPoint<ValueType> axis_point_from_point(const Point<ValueType>& point) const = 0;

    /// Convert from cross-axis size to regular size.
    ///
    /// This method needs to be implemented by a subclass.
    /// \param size A cross-axis based size.
    /// \return A regular size.
    virtual Size<ValueType> size_from_axis_size(const SizeType& size) const = 0;

    /// Convert from regular size to cross-axis size.
    ///
    /// This method needs to be implemented by a subclass.
    /// \param size A regular size.
    /// \return A cross-axis based size.
    virtual SizeType axis_size_from_size(const Size<ValueType>& size) const = 0;

    void layout(const Rect<ValueType>& frame, LayoutResult<Identifier, ValueType>& result) const override;

    Size<ValueType> measure(const Size<ValueType>& size) override;
};

template<typename Identifier, typename ValueType>
Size<ValueType> HVContainer<Identifier, ValueType>::measure(const Size<ValueType>& origin_size) {
    const SizeType container_size = axis_size_from_size(origin_size);
    // The total size of the elements in the container that have been calculated.
    SizeType measured_size;

    using usize = typename decltype(this->children)::size_type;
    for (const std::pair<int, std::vector<std::pair<usize, Element>>>& children_pair: this->children_priority_map) {
        const auto& children = children_pair.second;
        // The total container size minus the calculated size is used as
        // the base for calculating the next priority element.
        const SizeType size = {
            std::max(static_cast<ValueType>(0), container_size.main - measured_size.main),
            container_size.cross
        };
        std::vector<std::pair<usize, ValueType>> maximum_main_list;
        maximum_main_list.reserve(children.size());

        for (auto it: children) {
            maximum_main_list.push_back(std::make_pair(it.first, max_main_for_element(it.second)));
        }

        // Sort the elements in ascending order by the maximum space required.
        std::sort(
            maximum_main_list.begin(), maximum_main_list.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            }
        );
        // The size already occupied at the current priority.
        SizeType current_priority_measured_size;
        const usize list_count = maximum_main_list.size();
        for (auto it: makeIndexed(maximum_main_list)) {
            const usize index = it.index();

            const ValueType rest_main = size.main - current_priority_measured_size.main;
            /// The number of remaining elements that need to be allocated additional space.
            const usize count = list_count - index;
            // The remaining elements share the remaining space equally,
            // i.e., the space occupied by each element is not allowed to exceed this value.
            const ValueType maximum_container_main = rest_main / count;
            /// The position of the element instance in the children list.
            const usize element_idx = it.value().first;
            const auto child = this->children[element_idx];
            const AxisEdgeInsets<ValueType> padding = axis_edge_insets_for_element(child);
            // Make the element size with the maximum space of available containers.
            const SizeType item_size = axis_size_from_size(child->measure(size_from_axis_size(
                {
                    std::min(maximum_container_main - padding.main(),
                             max_main_for_element(child)),
                    std::min(size.cross - padding.cross(),
                             max_cross_for_element(child))
                }
            )));

            // Size limit on the calculation result.
            const ValueType main = std::max(min_main_for_element(child),
                                            std::min(item_size.main, maximum_container_main - padding.main()));
            const ValueType cross = std::max(min_cross_for_element(child),
                                             std::min(item_size.cross, size.cross - padding.cross()));
            // The size of the element after subtracting padding is the actual size of the element.
            this->size_list.at(element_idx) = size_from_axis_size({ main, cross });
            // The padding needs to be taken into account when counting the actual size of the occupancy.
            current_priority_measured_size.main += (main + padding.main());
            current_priority_measured_size.cross = std::max(current_priority_measured_size.cross,
                                                            cross + padding.cross());
        }
        measured_size.main += current_priority_measured_size.main;
        measured_size.cross = std::max(current_priority_measured_size.cross, measured_size.cross);
    }

    this->cached_measured_size = size_from_axis_size(measured_size);
    return this->cached_measured_size;
}

template<typename Identifier, typename ValueType>
void HVContainer<Identifier, ValueType>::layout(const Rect<ValueType>& frame,
                                                LayoutResult<Identifier, ValueType>& result) const {
    // Layout in terms of the actual space occupied by the elements.
    const AxisPoint<ValueType> origin = axis_point_from_point(
        {
            frame.x +
            (frame.width - this->cached_measured_size.width) / 2,
            frame.y +
            (frame.height - this->cached_measured_size.height) / 2
        }
    );
    const AxisSize<ValueType> size = axis_size_from_size(
        {
            std::max(frame.width, this->cached_measured_size.width),
            std::max(frame.height, this->cached_measured_size.height)
        }
    );

    ValueType used_main = 0;
    for (auto it: makeIndexed(this->children)) {
        const auto child_ptr = it.value();

        const auto item_size = axis_size_from_size(this->size_list[it.index()]);
        const auto item_padding = axis_edge_insets_for_element(child_ptr);
        /// The total size of the accommodating elements.
        ///
        /// The actual size of the element plus the element's own padding.
        const SizeType item_container_size = {
            item_size.main + item_padding.main(),
            item_size.cross + item_padding.cross()
        };
        // The container allows to specify its alignment on the cross axis,
        // so there will be an offset on the cross axis.
        const auto cross_offset = [this, &size, &item_container_size]() -> ValueType {
            switch (this->axis_alignment()) {
                case AxisAlignment::start:
                    return 0;
                case AxisAlignment::center:
                    return (size.cross - item_container_size.cross) / 2;
                case AxisAlignment::end:
                    return size.cross - item_container_size.cross;
            }
        }();
        const auto item_offset = axis_point_from_point(child_ptr->offset());
        const auto layout_frame_for_child = Rect<ValueType>(
            point_from_axis_point(
                { origin.main + used_main + item_offset.main + item_padding.main_start,
                  origin.cross + cross_offset + item_offset.cross + item_padding.cross_start }
            ),
            size_from_axis_size(item_size)
        );
        child_ptr->layout(layout_frame_for_child, result);
        used_main += item_container_size.main;
    }
}

#undef __AXIS_STRUCT_DECL

}

#endif //VPACKCORE_HV_CONTAINER_H
