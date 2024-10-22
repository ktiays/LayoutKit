//
// Created by ktiays on 2022/8/13.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_ITEM_H
#define VPACKCORE_ITEM_H

#include <cassert>

#include "layoutable.h"
#include "measurable.h"

namespace vpk::core {

template<typename Identifier, typename ValueType>
class Item : public Layoutable<Identifier, ValueType> {
public:
    Item(Identifier id, LayoutParams<ValueType> p, std::shared_ptr<Measurable<ValueType>> m)
        : Layoutable<Identifier, ValueType>(p), identifier_(id), measurable(m) {
        const SizeProperty<ValueType> size_property = p.size_property;
        assert(
            size_property.min_width.has_value()
            && size_property.min_height.has_value()
            && size_property.max_width.has_value()
            && size_property.max_height.has_value()
        );
        this->min_width_ = *size_property.min_width;
        this->min_height_ = *size_property.min_height;
        this->max_width_ = *size_property.max_width;
        this->max_height_ = *size_property.max_height;
    }

    inline Identifier identifier() const { return identifier_; }

    void layout(const Rect<ValueType>& frame, LayoutResult<Identifier, ValueType>& result) const override;

    Size<ValueType> measure(const Size<ValueType>& size) override;

private:
    Identifier identifier_;
    std::shared_ptr<Measurable<ValueType>> measurable;
};

template<typename Identifier, typename ValueType>
void Item<Identifier, ValueType>::layout(const Rect<ValueType>& frame,
                                         LayoutResult<Identifier, ValueType>& result) const {
    const auto iter = result.map.find(identifier());
    assert(iter == result.map.end());
    result.map[identifier()] = { .frame = frame, .z_idx = result.max_z_idx };
}

template<typename Identifier, typename ValueType>
Size<ValueType> Item<Identifier, ValueType>::measure(const Size<ValueType>& size) {
    return measurable->measure(size);
}

}

#endif //VPACKCORE_ITEM_H
