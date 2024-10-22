//
// Created by ktiays on 2022/8/12.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_TYPES_H
#define VPACKCORE_TYPES_H

#include <algorithm>
#include <limits>
#include <cmath>

namespace vpk::core::detail {

template<typename T>
T almost_equal(T x, T y) {
    const auto abs = std::abs(x - y);
    // The machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return abs <= std::numeric_limits<T>::epsilon() * std::abs(x + y)
           // unless the result is subnormal
           || abs < std::numeric_limits<T>::min();
}

}

//////////////////////////////// Point ////////////////////////////////

namespace vpk::core {

template<typename ValueType>
class Point {
public:
    ValueType x;
    ValueType y;

    Point();

    Point(ValueType x, ValueType y);
};

template<typename ValueType>
inline Point<ValueType>::Point()
    : x(0), y(0) {}

template<typename ValueType>
inline Point<ValueType>::Point(ValueType x, ValueType y)
    : x(x), y(y) {}
}

//////////////////////////////// Size ////////////////////////////////

namespace vpk::core {

template<typename ValueType>
class Size {
public:
    ValueType width;
    ValueType height;

    constexpr Size();

    constexpr Size(ValueType _width, ValueType _height);

    bool empty() const;
};

template<typename ValueType>
constexpr Size<ValueType>::Size()
    : width(0), height(0) {}

template<typename ValueType>
constexpr
Size<ValueType>::Size(ValueType _width, ValueType _height)
    : width(_width), height(_height) {}

template<typename ValueType>
bool Size<ValueType>::empty() const {
    return width <= 0 || height <= 0;
}

template<typename ValueType>
static inline
Size<ValueType>& operator *=(Size<ValueType>& a, ValueType b) {
    a.width *= b;
    a.height *= b;
    return a;
}

template<typename ValueType>
static inline
Size<ValueType> operator *(const Size<ValueType>& a, ValueType b) {
    Size<ValueType> tmp(a);
    tmp *= b;
    return tmp;
}

template<typename ValueType>
static inline
Size<ValueType>& operator /=(Size<ValueType>& a, ValueType b) {
    a.width /= b;
    a.height /= b;
    return a;
}

template<typename ValueType>
static inline
Size<ValueType> operator /(const Size<ValueType>& a, ValueType b) {
    Size<ValueType> tmp(a);
    tmp /= b;
    return tmp;
}

template<typename ValueType>
static inline
Size<ValueType>& operator +=(Size<ValueType>& a, const Size<ValueType>& b) {
    a.width += b.width;
    a.height += b.height;
    return a;
}

template<typename ValueType>
static inline
Size<ValueType> operator +(const Size<ValueType>& a, const Size<ValueType>& b) {
    Size<ValueType> tmp(a);
    tmp += b;
    return tmp;
}

template<typename ValueType>
static inline
Size<ValueType>& operator -=(Size<ValueType>& a, const Size<ValueType>& b) {
    a.width -= b.width;
    a.height -= b.height;
    return a;
}

template<typename ValueType>
static inline
Size<ValueType> operator -(const Size<ValueType>& a, const Size<ValueType>& b) {
    Size<ValueType> tmp(a);
    tmp -= b;
    return tmp;
}

template<typename ValueType>
static inline
bool operator ==(const Size<ValueType>& a, const Size<ValueType>& b) {
    return a.width == b.width && a.height == b.height;
}

template<typename ValueType>
static inline
bool operator !=(const Size<ValueType>& a, const Size<ValueType>& b) {
    return !(a == b);
}

}

//////////////////////////////// Rect ////////////////////////////////

namespace vpk::core {

template<typename ValueType>
class Rect {
public:
    ValueType x;
    ValueType y;
    ValueType width;
    ValueType height;

    Rect();

    Rect(ValueType x, ValueType y, ValueType width, ValueType height);

    Rect(const Point<ValueType>& org, const Size<ValueType>& sz);

    Rect(const Point<ValueType>& pt1, const Point<ValueType>& pt2);

    inline ValueType min_x() const { return x; }

    inline ValueType max_x() const;

    inline ValueType min_y() const { return y; }

    inline ValueType max_y() const;

    inline ValueType mid_x() const;

    inline ValueType mid_y() const;

    inline Point<ValueType> center() const;

    inline Point<ValueType> origin() const;

    inline Size<ValueType> size() const;

    bool contains(const Point<ValueType>& pt) const;
};

template<typename ValueType>
inline
Rect<ValueType>::Rect()
    : x(0), y(0), width(0), height(0) {}

template<typename ValueType>
inline
Rect<ValueType>::Rect(ValueType x, ValueType y, ValueType width, ValueType height)
    : x(x), y(y), width(width), height(height) {}

template<typename ValueType>
inline
Rect<ValueType>::Rect(const Point<ValueType>& org, const Size<ValueType>& sz)
    : x(org.x), y(org.y), width(sz.width), height(sz.height) {}

template<typename ValueType>
inline
Rect<ValueType>::Rect(const Point<ValueType>& pt1, const Point<ValueType>& pt2) {
    x = std::min(pt1.x, pt2.x);
    y = std::min(pt1.y, pt2.y);
    width = std::max(pt1.x, pt2.x) - x;
    height = std::max(pt1.y, pt2.y) - y;
}

template<typename ValueType>
inline ValueType Rect<ValueType>::max_x() const {
    return x + width;
}

template<typename ValueType>
inline ValueType Rect<ValueType>::max_y() const {
    return y + height;
}

template<typename ValueType>
inline ValueType Rect<ValueType>::mid_x() const {
    return x + width / 2;
}

template<typename ValueType>
inline ValueType Rect<ValueType>::mid_y() const {
    return y + height / 2;
}

template<typename ValueType>
inline Point<ValueType> Rect<ValueType>::center() const {
    return { mid_x(), mid_y() };
}

template<typename ValueType>
inline Point<ValueType> Rect<ValueType>::origin() const {
    return { x, y };
}

template<typename ValueType>
inline Size<ValueType> Rect<ValueType>::size() const {
    return { width, height };
}

template<typename ValueType>
bool Rect<ValueType>::contains(const Point<ValueType>& pt) const {
    return x <= pt.x && pt.x < x + width && y <= pt.y && pt.y < y + height;
}

template<typename ValueType>
static inline
Rect<ValueType>& operator +=(Rect<ValueType>& a, const Point<ValueType>& b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

template<typename ValueType>
static inline
Rect<ValueType>& operator -=(Rect<ValueType>& a, const Point<ValueType>& b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

template<typename ValueType>
static inline
Rect<ValueType>& operator +=(Rect<ValueType>& a, const Size<ValueType>& b) {
    a.width += b.width;
    a.height += b.height;
    return a;
}

template<typename ValueType>
static inline
Rect<ValueType>& operator -=(Rect<ValueType>& a, const Size<ValueType>& b) {
    const ValueType width = a.width - b.width;
    const ValueType height = a.height - b.height;
    a.width = width;
    a.height = height;
    return a;
}

template<typename ValueType>
static inline
bool operator ==(const Rect<ValueType>& a, const Rect<ValueType>& b) {
    return detail::almost_equal(a.x, b.x) &&
           detail::almost_equal(a.y, b.y) &&
           detail::almost_equal(a.width, b.width) &&
           detail::almost_equal(a.height, b.height);

}

template<typename ValueType>
static inline
bool operator !=(const Rect<ValueType>& a, const Rect<ValueType>& b) {
    return !(a == b);
}

template<typename ValueType>
static inline
Rect<ValueType> operator +(const Rect<ValueType>& a, const Point<ValueType>& b) {
    return Rect<ValueType>(a.x + b.x, a.y + b.y, a.width, a.height);
}

template<typename ValueType>
static inline
Rect<ValueType> operator -(const Rect<ValueType>& a, const Point<ValueType>& b) {
    return Rect<ValueType>(a.x - b.x, a.y - b.y, a.width, a.height);
}

template<typename ValueType>
static inline
Rect<ValueType> operator +(const Rect<ValueType>& a, const Size<ValueType>& b) {
    return Rect<ValueType>(a.x, a.y, a.width + b.width, a.height + b.height);
}

template<typename ValueType>
static inline
Rect<ValueType> operator -(const Rect<ValueType>& a, const Size<ValueType>& b) {
    const ValueType width = a.width - b.width;
    const ValueType height = a.height - b.height;
    return Rect<ValueType>(a.x, a.y, width, height);
}

}

//////////////////////////////// Alignment ////////////////////////////////

namespace vpk::core {

enum class HorizontalAlignment {
    center,
    leading,
    trailing,
};

enum class VerticalAlignment {
    center,
    top,
    bottom,
};

struct Alignment {
public:
    static Alignment top_leading;
    static Alignment top_center;
    static Alignment top_trailing;

    static Alignment leading;
    static Alignment center;
    static Alignment trailing;

    static Alignment bottom_leading;
    static Alignment bottom_center;
    static Alignment bottom_trailing;

    Alignment(VerticalAlignment v, HorizontalAlignment h)
        : vertical_alignment(v), horizontal_alignment(h) {}

    VerticalAlignment vertical() const { return vertical_alignment; }

    HorizontalAlignment horizontal() const { return horizontal_alignment; }

private:
    VerticalAlignment vertical_alignment;
    HorizontalAlignment horizontal_alignment;
};

static inline Alignment operator |(const VerticalAlignment& v, const HorizontalAlignment& h) {
    return { v, h };
}

static inline Alignment operator |(const HorizontalAlignment& h, const VerticalAlignment& v) {
    return { v, h };
}

}

//////////////////////////////// Edge Insets ////////////////////////////////

namespace vpk::core {

template<typename ValueType>
class EdgeInsets {
public:
    ValueType left;
    ValueType top;
    ValueType right;
    ValueType bottom;

    EdgeInsets()
        : left(0), top(0), right(0), bottom(0) {}

    EdgeInsets(const ValueType& left, const ValueType& top, const ValueType& right, const ValueType& bottom)
        : left(left), top(top), right(right), bottom(bottom) {}

    EdgeInsets(ValueType&& left, ValueType&& top, ValueType&& right, ValueType&& bottom)
        : left(std::move(left)), top(std::move(top)), right(std::move(right)), bottom(std::move(bottom)) {}

    inline ValueType vertical() const {
        return top + bottom;
    }

    inline ValueType horizontal() const {
        return left + right;
    }
};

}

#endif //VPACKCORE_TYPES_H
