//
// Created by ktiays on 2022/8/12.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_LAYOUTABLE_H
#define VPACKCORE_LAYOUTABLE_H

#include "layout_result.h"
#include "types.h"

namespace vpk::core {

template<typename ValueType>
struct SizeProperty {
    optional<ValueType> min_width;
    optional<ValueType> min_height;
    optional<ValueType> max_width;
    optional<ValueType> max_height;
};

template<typename ValueType>
struct LayoutParams {
    SizeProperty<ValueType> size_property;
    EdgeInsets<ValueType> padding;
    Point<ValueType> offset;

    /// The layout priority of the element.
    ///
    /// Container will read this value from elements and use the value when deciding how to assign space to elements.
    /// The default value of this property is 0.
    int priority{};

    LayoutParams() = default;

    LayoutParams(const SizeProperty<ValueType>& size, const EdgeInsets<ValueType>& insets,
                 const Point<ValueType>& offset, int priority = 0)
        : size_property(size), padding(insets), offset(offset), priority(priority) {}

    LayoutParams(SizeProperty<ValueType>&& size, EdgeInsets<ValueType>&& insets,
                 Point<ValueType>&& offset, int priority = 0)
        : size_property(std::move(size)), padding(std::move(insets)), offset(std::move(offset)), priority(priority) {}
};

template<
    typename Identifier,
    typename ValueType,
    typename = std::enable_if<std::numeric_limits<ValueType>::has_infinity>
>
class Layoutable {
public:
    explicit Layoutable(LayoutParams<ValueType> p)
        : params(p) {}

    virtual void layout(const Rect<ValueType>& frame, LayoutResult<Identifier, ValueType>& result) const = 0;

    virtual Size<ValueType> measure(const Size<ValueType>& size) { return {}; }

    /* The minimum or maximum values here indicate the element's own size attribute, excluding padding. */

    ValueType min_width() const { return min_width_; }

    ValueType min_height() const { return min_height_; }

    ValueType max_width() const { return max_width_; }

    ValueType max_height() const { return max_height_; }

    inline EdgeInsets<ValueType> padding() const { return params.padding; }

    inline Point<ValueType> offset() const { return params.offset; }

    virtual ~Layoutable() = default;

    LayoutParams<ValueType> params;

    /// Resizes the specified size to the closest size that the element is suitable for display.
    ///
    /// The preferred size is not smaller than the minimum size of the element
    /// and does not exceed the maximum size of the element.
    Size<ValueType> preferred_size(const Size<ValueType>& size) const {
        return {
            std::max(static_cast<ValueType>(0), std::min(std::max(min_width(), size.width), max_width())),
            std::max(static_cast<ValueType>(0), std::min(std::max(min_height(), size.height), max_height())),
        };
    }

protected:
    ValueType min_width_;
    ValueType min_height_;
    ValueType max_width_;
    ValueType max_height_;
};

template<typename Identifier, typename ValueType>
using LayoutablePointer = std::shared_ptr<Layoutable<Identifier, ValueType>>;

}

#endif //VPACKCORE_LAYOUTABLE_H
