//
// Created by ktiays on 2022/8/14.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_COMPUTER_H
#define VPACKCORE_COMPUTER_H

#include <algorithm>

#include "layoutable.h"

namespace vpk::core {

template <typename Identifier, typename ValueType>
class LayoutComputer {
public:
    LayoutComputer(LayoutablePointer<Identifier, ValueType> it) : item(it) {}

    LayoutResult<Identifier, ValueType> compute(const Rect<ValueType>& frame) const;

    inline Size<ValueType> compute_dry_layout(const Rect<ValueType>& frame) const {
        return item->measure(frame.size());
    }

private:
    LayoutablePointer<Identifier, ValueType> item;
};

template <typename Identifier, typename ValueType>
LayoutResult<Identifier, ValueType> LayoutComputer<Identifier, ValueType>::compute(const Rect<ValueType>& frame) const {
    LayoutResult<Identifier, ValueType> result;
    const EdgeInsets<ValueType> padding = item->padding();
    const Point<ValueType> offset = item->offset();

    Size<ValueType> size = item->measure(item->preferred_size({frame.width - padding.horizontal(), frame.height - padding.vertical()}));
    size = item->preferred_size(size);

    item->layout(
        Rect<ValueType>{
            (frame.width - size.width) / 2 + padding.left + offset.x,
            (frame.height - size.height) / 2 + padding.top + offset.y,
            size.width - padding.horizontal(),
            size.height - padding.vertical(),
        },
        result);
    return result;
}

} // namespace vpk::core

#endif //VPACKCORE_COMPUTER_H
