//
// Created by ktiays on 2022/8/13.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_INDEXED_H
#define VPACKCORE_INDEXED_H

#include <cstddef>
#include <utility>
#include <type_traits>

namespace vpk {

template<typename T, typename IndexType = std::ptrdiff_t>
struct IndexValue {
    IndexValue(const std::pair<IndexType, T>& pair)
        : pair_(pair) {}

    inline IndexType index() const { return pair_.first; }

    inline T value() const { return pair_.second; }

private:
    std::pair<IndexType, T> pair_;
};

template<typename Cont, typename It>
struct Indexed {
    template<typename Iter, typename SizeType>
    struct IndexedIterator {
        using Type = IndexedIterator<Iter, SizeType>;
        using ReferenceType = typename Iter::reference;

        Type& operator ++() {
            ++this->it_;
            ++this->idx_;
            return *this;
        }

        bool operator ==(const Type& rhs) const {
            return this->it_ == rhs.it_;
        }

        bool operator !=(const Type& rhs) const {
            return !(*this == rhs);
        }

        IndexValue<ReferenceType, SizeType> operator *() const {
            return std::pair<SizeType, ReferenceType>(idx_, *it_);
        }

    private:
        friend struct Indexed<Cont, It>;

        Iter it_;
        SizeType idx_;
    };

    using IteratorType = IndexedIterator<
        It,
        typename std::decay<Cont>::type::size_type
    >;

    explicit Indexed(Cont& cont) : cont_(cont) {}

    IteratorType begin() const {
        IteratorType iter;
        iter.idx_ = 0;
        iter.it_ = cont_.begin();
        return iter;
    }

    IteratorType end() const {
        IteratorType iter;
        iter.idx_ = cont_.size();
        iter.it_ = cont_.end();
        return iter;
    }

    Cont cont_;
};

template<typename Cont>
inline Indexed<Cont&, typename Cont::iterator> makeIndexed(Cont& cont) {
    return Indexed<Cont&, typename Cont::iterator>(cont);
}

template<typename Cont>
inline Indexed<const Cont&, typename Cont::const_iterator> makeIndexed(const Cont& cont) {
    return Indexed<const Cont&, typename Cont::const_iterator>(cont);
}

}

#endif //VPACKCORE_INDEXED_H
