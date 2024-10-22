//
// Created by ktiays on 2022/8/13.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_MEASURABLE_H
#define VPACKCORE_MEASURABLE_H

#include <functional>

#include "types.h"

namespace vpk::core {

template<typename ValueType>
class Measurable {
public:
    virtual Size<ValueType> measure(const Size<ValueType>& size) const = 0;

    virtual ~Measurable() = default;
};

template<typename ValueType>
class AnyMeasurable : public Measurable<ValueType> {
public:
    AnyMeasurable()
        : measure_func([](const Size<ValueType>& size) { return size; }) {}

    AnyMeasurable(Size<ValueType> size) {
        measure_func = [size](const Size<ValueType>& s) {
            return size;
        };
    }

    template<typename F>
    AnyMeasurable(F&& f)
        : measure_func(std::forward<F>(f)) {}

    Size<ValueType> measure(const Size<ValueType>& size) const override {
        return measure_func(size);
    }

private:
    std::function<Size<ValueType>(Size<ValueType>)> measure_func;
};

}

#endif //VPACKCORE_MEASURABLE_H
