//
// Created by ktiays on 2022/8/12.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_OPTIONAL_H
#define VPACKCORE_OPTIONAL_H

#include <version>

#if defined(__cpp_lib_optional)

#include <optional>

namespace vpk {
    template<typename T>
    using optional = std::optional<T>;

    static constexpr const std::nullopt_t nullopt = std::nullopt;
}

#else

#include <exception>
#include <functional>
#include <new>
#include <type_traits>
#include <utility>

namespace vpk {

/// Used to represent an optional with no data; essentially a bool
class monostate {};

///  A tag type to tell optional to construct its value in-place
struct in_place_t {
    explicit in_place_t() = default;
};

/// A tag to tell optional to construct its value in-place
static constexpr in_place_t in_place{};

template<typename T>
class optional;

namespace detail {

// C++14-style aliases for brevity
template<typename T>
using remove_reference_t = typename std::remove_reference<T>::type;
template<typename T> using decay_t = typename std::decay<T>::type;
template<bool E, typename T = void>
using enable_if_t = typename std::enable_if<E, T>::type;
template<bool B, typename T, typename F>
using conditional_t = typename std::conditional<B, T, F>::type;

// std::conjunction from C++17
template<typename...>
struct conjunction : std::true_type {};
template<typename B>
struct conjunction<B> : B {};
template<typename B, typename... Bs>
struct conjunction<B, Bs...>
    : std::conditional<bool(B::value), conjunction<Bs...>, B>::type {
};

// std::invoke from C++17
template<
    typename Fn,
    typename... Args,
    typename = enable_if_t<std::is_member_pointer<decay_t<Fn>>::value>,
    int = 0
>
constexpr auto invoke(Fn&& f, Args&& ... args) noexcept(
noexcept(std::mem_fn(f)(std::forward<Args>(args)...)))
-> decltype(std::mem_fn(f)(std::forward<Args>(args)...)) {
    return std::mem_fn(f)(std::forward<Args>(args)...);
}

template<typename Fn, typename... Args,
    typename = enable_if_t<!std::is_member_pointer<decay_t<Fn>>::value>>
constexpr auto invoke(Fn&& f, Args&& ... args) noexcept(
noexcept(std::forward<Fn>(f)(std::forward<Args>(args)...)))
-> decltype(std::forward<Fn>(f)(std::forward<Args>(args)...)) {
    return std::forward<Fn>(f)(std::forward<Args>(args)...);
}

// std::invoke_result from C++17
template<typename F, class, typename... Us>
struct invoke_result_impl;

template<typename F, typename... Us>
struct invoke_result_impl<
    F, decltype(detail::invoke(std::declval<F>(), std::declval<Us>()...), void()),
    Us...> {
    using type = decltype(detail::invoke(std::declval<F>(), std::declval<Us>()...));
};

template<typename F, typename... Us>
using invoke_result = invoke_result_impl<F, void, Us...>;

template<typename F, typename... Us>
using invoke_result_t = typename invoke_result<F, Us...>::type;

namespace swap_adl_tests {
// if swap ADL finds this then it would call std::swap otherwise (same
// signature)
struct tag {};

template<typename T>
tag swap(T&, T&);

template<typename T, std::size_t N>
tag swap(T (& a)[N], T (& b)[N]);

// helper functions to test if an unqualified swap is possible, and if it
// becomes std::swap
template<typename, typename>
std::false_type can_swap(...) noexcept(false);

template<typename T, typename U,
    class = decltype(swap(std::declval<T&>(), std::declval<U&>()))>
std::true_type can_swap(int) noexcept(noexcept(swap(std::declval<T&>(),
                                                    std::declval<U&>())));

template<typename, typename>
std::false_type uses_std(...);

template<typename T, typename U>
std::is_same<decltype(swap(std::declval<T&>(), std::declval<U&>())), tag> uses_std(int);

template<typename T>
struct is_std_swap_noexcept
    : std::integral_constant<bool,
        std::is_nothrow_move_constructible<T>::value &&
        std::is_nothrow_move_assignable<T>::value> {
};

template<typename T, std::size_t N>
struct is_std_swap_noexcept<T[N]> : is_std_swap_noexcept<T> {};

template<typename T, typename U>
struct is_adl_swap_noexcept
    : std::integral_constant<bool, noexcept(can_swap<T, U>(0))> {
};
} // namespace swap_adl_tests

template<typename T, typename U = T>
struct is_swappable
    : std::integral_constant<
        bool,
        decltype(detail::swap_adl_tests::can_swap<T, U>(0))::value &&
        (!decltype(detail::swap_adl_tests::uses_std<T, U>(0))::value ||
         (std::is_move_assignable<T>::value &&
          std::is_move_constructible<T>::value))> {
};

template<typename T, std::size_t N>
struct is_swappable<T[N], T[N]>
    : std::integral_constant<
        bool,
        decltype(detail::swap_adl_tests::can_swap<T[N], T[N]>(0))::value &&
        (!decltype(
        detail::swap_adl_tests::uses_std<T[N], T[N]>(0))::value ||
         is_swappable<T, T>::value)> {
};

template<typename T, typename U = T>
struct is_nothrow_swappable
    : std::integral_constant<
        bool,
        is_swappable<T, U>::value &&
        ((decltype(detail::swap_adl_tests::uses_std<T, U>(0))::value
          && detail::swap_adl_tests::is_std_swap_noexcept<T>::value) ||
         (!decltype(detail::swap_adl_tests::uses_std<T, U>(0))::value &&
          detail::swap_adl_tests::is_adl_swap_noexcept<T,
              U>::value))> {
};

// std::void_t from C++17
template<typename...>
struct voider { using type = void; };
template<typename... Ts> using void_t = typename voider<Ts...>::type;

// Trait for checking if a type is a tl::optional
template<typename T>
struct is_optional_impl : std::false_type {};
template<typename T>
struct is_optional_impl<optional<T>> : std::true_type {};
template<typename T> using is_optional = is_optional_impl<decay_t<T>>;

// Change void to tl::monostate
template<typename U>
using fixup_void = conditional_t<std::is_void<U>::value, monostate, U>;

template<typename F, typename U, typename = invoke_result_t<F, U>>
using get_map_return = optional<fixup_void<invoke_result_t<F, U>>>;

// Check if invoking F for some Us returns void
template<typename F, typename = void, typename... U>
struct returns_void_impl;
template<typename F, typename... U>
struct returns_void_impl<F, void_t<invoke_result_t<F, U...>>, U...>
    : std::is_void<invoke_result_t<F, U...>> {
};
template<typename F, typename... U>
using returns_void = returns_void_impl<F, void, U...>;

template<typename T, typename... U>
using enable_if_ret_void = enable_if_t<returns_void<T&&, U...>::value>;

template<typename T, typename... U>
using disable_if_ret_void = enable_if_t<!returns_void<T&&, U...>::value>;

template<typename T, typename U>
using enable_forward_value =
    detail::enable_if_t<std::is_constructible<T, U&&>::value &&
                        !std::is_same<detail::decay_t<U>, in_place_t>::value &&
                        !std::is_same<optional<T>, detail::decay_t<U>>::value>;

template<typename T, typename U, typename Other>
using enable_from_other = detail::enable_if_t<
    std::is_constructible<T, Other>::value &&
    !std::is_constructible<T, optional<U>&>::value &&
    !std::is_constructible<T, optional<U>&&>::value &&
    !std::is_constructible<T, const optional<U>&>::value &&
    !std::is_constructible<T, const optional<U>&&>::value &&
    !std::is_convertible<optional<U>&, T>::value &&
    !std::is_convertible<optional<U>&&, T>::value &&
    !std::is_convertible<const optional<U>&, T>::value &&
    !std::is_convertible<const optional<U>&&, T>::value>;

template<typename T, typename U>
using enable_assign_forward = detail::enable_if_t<
    !std::is_same<optional<T>, detail::decay_t<U>>::value &&
    !detail::conjunction<std::is_scalar<T>,
        std::is_same<T, detail::decay_t<U>>>::value &&
    std::is_constructible<T, U>::value && std::is_assignable<T&, U>::value>;

template<typename T, typename U, class Other>
using enable_assign_from_other = detail::enable_if_t<
    std::is_constructible<T, Other>::value &&
    std::is_assignable<T&, Other>::value &&
    !std::is_constructible<T, optional<U>&>::value &&
    !std::is_constructible<T, optional<U>&&>::value &&
    !std::is_constructible<T, const optional<U>&>::value &&
    !std::is_constructible<T, const optional<U>&&>::value &&
    !std::is_convertible<optional<U>&, T>::value &&
    !std::is_convertible<optional<U>&&, T>::value &&
    !std::is_convertible<const optional<U>&, T>::value &&
    !std::is_convertible<const optional<U>&&, T>::value &&
    !std::is_assignable<T&, optional<U>&>::value &&
    !std::is_assignable<T&, optional<U>&&>::value &&
    !std::is_assignable<T&, const optional<U>&>::value &&
    !std::is_assignable<T&, const optional<U>&&>::value>;

// The storage base manages the actual storage, and correctly propagates
// trivial destruction from T. This case is for when T is not trivially
// destructible.
template<typename T, bool = ::std::is_trivially_destructible<T>::value>
struct optional_storage_base {
    constexpr optional_storage_base() noexcept
        : m_dummy(), m_has_value(false) {}

    template<typename... U>
    constexpr optional_storage_base(in_place_t, U&& ... u)
        : m_value(std::forward<U>(u)...), m_has_value(true) {}

    ~optional_storage_base() {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
    }

    struct dummy {};
    union {
        dummy m_dummy;
        T m_value;
    };

    bool m_has_value;
};

// This case is for when T is trivially destructible.
template<typename T>
struct optional_storage_base<T, true> {
    constexpr optional_storage_base() noexcept
        : m_dummy(), m_has_value(false) {}

    template<typename... U>
    constexpr optional_storage_base(in_place_t, U&& ... u)
        : m_value(std::forward<U>(u)...), m_has_value(true) {}

    // No destructor, so this class is trivially destructible

    struct dummy {};
    union {
        dummy m_dummy;
        T m_value;
    };

    bool m_has_value = false;
};

// This base class provides some handy member functions which can be used in
// further derived classes
template<typename T>
struct optional_operations_base : optional_storage_base<T> {
    using optional_storage_base<T>::optional_storage_base;

    void hard_reset() noexcept {
        get().~T();
        this->m_has_value = false;
    }

    template<typename... Args>
    void construct(Args&& ... args) noexcept {
        new(std::addressof(this->m_value)) T(std::forward<Args>(args)...);
        this->m_has_value = true;
    }

    template<typename Opt>
    void assign(Opt&& rhs) {
        if (this->has_value()) {
            if (rhs.has_value()) {
                this->m_value = std::forward<Opt>(rhs).get();
            } else {
                this->m_value.~T();
                this->m_has_value = false;
            }
        } else if (rhs.has_value()) {
            construct(std::forward<Opt>(rhs).get());
        }
    }

    virtual bool has_value() const { return this->m_has_value; }

    constexpr T& get()& { return this->m_value; }

    constexpr const T& get() const& { return this->m_value; }

    constexpr T&& get()&& { return std::move(this->m_value); }

    constexpr const T&& get() const&& { return std::move(this->m_value); }

};

// This class manages conditionally having a trivial copy constructor
// This specialization is for when T is trivially copy constructible
template<typename T, bool = std::is_trivially_copy_constructible<T>::value>
struct optional_copy_base : optional_operations_base<T> {
    using optional_operations_base<T>::optional_operations_base;
};

// This specialization is for when T is not trivially copy constructible
template<typename T>
struct optional_copy_base<T, false> : optional_operations_base<T> {
    using optional_operations_base<T>::optional_operations_base;

    optional_copy_base() = default;

    optional_copy_base(const optional_copy_base& rhs)
        : optional_operations_base<T>() {
        if (rhs.has_value()) {
            this->construct(rhs.get());
        } else {
            this->m_has_value = false;
        }
    }

    optional_copy_base(optional_copy_base&& rhs) noexcept = default;

    optional_copy_base& operator =(const optional_copy_base& rhs) = default;

    optional_copy_base& operator =(optional_copy_base&& rhs) noexcept = default;
};

// This class manages conditionally having a trivial move constructor
// Unfortunately there's no way to achieve this in GCC < 5 AFAIK, since it
// doesn't implement an analogue to std::is_trivially_move_constructible. We
// have to make do with a non-trivial move constructor even if T is trivially
// move constructible
template<typename T, bool = std::is_trivially_move_constructible<T>::value>
struct optional_move_base : optional_copy_base<T> {
    using optional_copy_base<T>::optional_copy_base;
};

template<typename T>
struct optional_move_base<T, false> : optional_copy_base<T> {
    using optional_copy_base<T>::optional_copy_base;

    optional_move_base() = default;

    optional_move_base(const optional_move_base& rhs) = default;

    optional_move_base(optional_move_base&& rhs) noexcept(
    std::is_nothrow_move_constructible<T>::value) {
        if (rhs.has_value()) {
            this->construct(std::move(rhs.get()));
        } else {
            this->m_has_value = false;
        }
    }

    optional_move_base& operator =(const optional_move_base& rhs) = default;

    optional_move_base& operator =(optional_move_base&& rhs) noexcept = default;
};

// This class manages conditionally having a trivial copy assignment operator
template<typename T, bool = std::is_trivially_copy_assignable<T>::value &&
                            std::is_trivially_copy_constructible<T>::value &&
                            std::is_trivially_destructible<T>::value>
struct optional_copy_assign_base : optional_move_base<T> {
    using optional_move_base<T>::optional_move_base;
};

template<typename T>
struct optional_copy_assign_base<T, false> : optional_move_base<T> {
    using optional_move_base<T>::optional_move_base;

    optional_copy_assign_base() = default;

    optional_copy_assign_base(const optional_copy_assign_base& rhs) = default;

    optional_copy_assign_base(optional_copy_assign_base&& rhs) noexcept = default;

    optional_copy_assign_base& operator =(const optional_copy_assign_base& rhs) {
        this->assign(rhs);
        return *this;
    }

    optional_copy_assign_base&
    operator =(optional_copy_assign_base&& rhs) noexcept = default;
};

// This class manages conditionally having a trivial move assignment operator
// Unfortunately there's no way to achieve this in GCC < 5 AFAIK, since it
// doesn't implement an analogue to std::is_trivially_move_assignable. We have
// to make do with a non-trivial move assignment operator even if T is trivially
// move assignable
template<typename T, bool = std::is_trivially_destructible<T>::value
                            && std::is_trivially_move_constructible<T>::value
                            && std::is_trivially_move_assignable<T>::value>
struct optional_move_assign_base : optional_copy_assign_base<T> {
    using optional_copy_assign_base<T>::optional_copy_assign_base;
};

template<typename T>
struct optional_move_assign_base<T, false> : optional_copy_assign_base<T> {
    using optional_copy_assign_base<T>::optional_copy_assign_base;

    optional_move_assign_base() = default;

    optional_move_assign_base(const optional_move_assign_base& rhs) = default;

    optional_move_assign_base(optional_move_assign_base&& rhs) noexcept = default;

    optional_move_assign_base&
    operator =(const optional_move_assign_base& rhs) = default;

    optional_move_assign_base&
    operator =(optional_move_assign_base&& rhs) noexcept(
    std::is_nothrow_move_constructible<T>::value
    && std::is_nothrow_move_assignable<T>::value) {
        this->assign(std::move(rhs));
        return *this;
    }
};

// optional_delete_ctor_base will conditionally delete copy and move
// constructors depending on whether T is copy/move constructible
template<typename T, bool EnableCopy = std::is_copy_constructible<T>::value,
    bool EnableMove = std::is_move_constructible<T>::value>
struct optional_delete_ctor_base {
    optional_delete_ctor_base() = default;

    optional_delete_ctor_base(const optional_delete_ctor_base&) = default;

    optional_delete_ctor_base(optional_delete_ctor_base&&) noexcept = default;

    optional_delete_ctor_base&
    operator =(const optional_delete_ctor_base&) = default;

    optional_delete_ctor_base&
    operator =(optional_delete_ctor_base&&) noexcept = default;
};

template<typename T>
struct optional_delete_ctor_base<T, true, false> {
    optional_delete_ctor_base() = default;

    optional_delete_ctor_base(const optional_delete_ctor_base&) = default;

    optional_delete_ctor_base(optional_delete_ctor_base&&) noexcept = delete;

    optional_delete_ctor_base&
    operator =(const optional_delete_ctor_base&) = default;

    optional_delete_ctor_base&
    operator =(optional_delete_ctor_base&&) noexcept = default;
};

template<typename T>
struct optional_delete_ctor_base<T, false, true> {
    optional_delete_ctor_base() = default;

    optional_delete_ctor_base(const optional_delete_ctor_base&) = delete;

    optional_delete_ctor_base(optional_delete_ctor_base&&) noexcept = default;

    optional_delete_ctor_base&
    operator =(const optional_delete_ctor_base&) = default;

    optional_delete_ctor_base&
    operator =(optional_delete_ctor_base&&) noexcept = default;
};

template<typename T>
struct optional_delete_ctor_base<T, false, false> {
    optional_delete_ctor_base() = default;

    optional_delete_ctor_base(const optional_delete_ctor_base&) = delete;

    optional_delete_ctor_base(optional_delete_ctor_base&&) noexcept = delete;

    optional_delete_ctor_base&
    operator =(const optional_delete_ctor_base&) = default;

    optional_delete_ctor_base&
    operator =(optional_delete_ctor_base&&) noexcept = default;
};

// optional_delete_assign_base will conditionally delete copy and move
// constructors depending on whether T is copy/move constructible + assignable
template<typename T,
    bool EnableCopy = (std::is_copy_constructible<T>::value &&
                       std::is_copy_assignable<T>::value),
    bool EnableMove = (std::is_move_constructible<T>::value &&
                       std::is_move_assignable<T>::value)>
struct optional_delete_assign_base {
    optional_delete_assign_base() = default;

    optional_delete_assign_base(const optional_delete_assign_base&) = default;

    optional_delete_assign_base(optional_delete_assign_base&&) noexcept =
    default;

    optional_delete_assign_base&
    operator =(const optional_delete_assign_base&) = default;

    optional_delete_assign_base&
    operator =(optional_delete_assign_base&&) noexcept = default;
};

template<typename T>
struct optional_delete_assign_base<T, true, false> {
    optional_delete_assign_base() = default;

    optional_delete_assign_base(const optional_delete_assign_base&) = default;

    optional_delete_assign_base(optional_delete_assign_base&&) noexcept =
    default;

    optional_delete_assign_base&
    operator =(const optional_delete_assign_base&) = default;

    optional_delete_assign_base&
    operator =(optional_delete_assign_base&&) noexcept = delete;
};

template<typename T>
struct optional_delete_assign_base<T, false, true> {
    optional_delete_assign_base() = default;

    optional_delete_assign_base(const optional_delete_assign_base&) = default;

    optional_delete_assign_base(optional_delete_assign_base&&) noexcept =
    default;

    optional_delete_assign_base&
    operator =(const optional_delete_assign_base&) = delete;

    optional_delete_assign_base&
    operator =(optional_delete_assign_base&&) noexcept = default;
};

template<typename T>
struct optional_delete_assign_base<T, false, false> {
    optional_delete_assign_base() = default;

    optional_delete_assign_base(const optional_delete_assign_base&) = default;

    optional_delete_assign_base(optional_delete_assign_base&&) noexcept = default;

    optional_delete_assign_base&
    operator =(const optional_delete_assign_base&) = delete;

    optional_delete_assign_base&
    operator =(optional_delete_assign_base&&) noexcept = delete;
};

} // namespace detail

/// A tag type to represent an empty optional
struct nullopt_t {
    struct do_not_use {};

    constexpr explicit nullopt_t(do_not_use, do_not_use) noexcept {}
};

/// Represents an empty optional
static constexpr nullopt_t nullopt{ nullopt_t::do_not_use{},
                                    nullopt_t::do_not_use{}};

class bad_optional_access : public std::exception {
public:
    bad_optional_access() = default;

    const char *what() const noexcept override { return "Optional has no value"; }
};

/// An optional object is an object that contains the storage for another
/// object and manages the lifetime of this contained object, if any. The
/// contained object may be initialized after the optional object has been
/// initialized, and may be destroyed before the optional object has been
/// destroyed. The initialization state of the contained object is tracked by
/// the optional object.
template<typename T>
class optional : private detail::optional_move_assign_base<T>,
                 private detail::optional_delete_ctor_base<T>,
                 private detail::optional_delete_assign_base<T> {
    using base = detail::optional_move_assign_base<T>;

    static_assert(!std::is_same<T, in_place_t>::value,
                  "instantiation of optional with in_place_t is ill-formed");
    static_assert(!std::is_same<detail::decay_t<T>, nullopt_t>::value,
                  "instantiation of optional with nullopt_t is ill-formed");

public:
// The different versions for C++14 and 11 are needed because deduced return
// types are not SFINAE-safe. This provides better support for things like
// generic lambdas. C.f.
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0826r0.html

    /// Carries out some operation which returns an optional on the stored
    /// object if there is one.
    template<typename F>
    constexpr auto and_then(F&& f)& {
        using result = detail::invoke_result_t<F, T&>;
        static_assert(detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    template<typename F>
    constexpr auto and_then(F&& f)&& {
        using result = detail::invoke_result_t<F, T&&>;
        static_assert(detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? detail::invoke(std::forward<F>(f), std::move(**this))
                           : result(nullopt);
    }

    template<typename F>
    constexpr auto and_then(F&& f) const& {
        using result = detail::invoke_result_t<F, const T&>;
        static_assert(detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    template<typename F>
    constexpr auto and_then(F&& f) const&& {
        using result = detail::invoke_result_t<F, const T&&>;
        static_assert(detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? detail::invoke(std::forward<F>(f), std::move(**this))
                           : result(nullopt);
    }

    /// Carries out some operation on the stored object if there is one.
    template<typename F>
    constexpr auto map(F&& f)& {
        return optional_map_impl(*this, std::forward<F>(f));
    }

    template<typename F>
    constexpr auto map(F&& f)&& {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template<typename F>
    constexpr auto map(F&& f) const& {
        return optional_map_impl(*this, std::forward<F>(f));
    }

    template<typename F>
    constexpr auto map(F&& f) const&& {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    /// Carries out some operation on the stored object if there is one.
    template<typename F>
    constexpr auto transform(F&& f)& {
        return optional_map_impl(*this, std::forward<F>(f));
    }

    template<typename F>
    constexpr auto transform(F&& f)&& {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template<typename F>
    constexpr auto transform(F&& f) const& {
        return optional_map_impl(*this, std::forward<F>(f));
    }

    template<typename F>
    constexpr auto transform(F&& f) const&& {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    /// Calls `f` if the optional is empty
    template<typename F, detail::enable_if_ret_void<F> * = nullptr>
    optional<T> constexpr or_else(F&& f)& {
        if (has_value())
            return *this;

        std::forward<F>(f)();
        return nullopt;
    }

    template<typename F, detail::disable_if_ret_void<F> * = nullptr>
    optional<T> constexpr or_else(F&& f)& {
        return has_value() ? *this : std::forward<F>(f)();
    }

    template<typename F, detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F&& f)&& {
        if (has_value())
            return std::move(*this);

        std::forward<F>(f)();
        return nullopt;
    }

    template<typename F, detail::disable_if_ret_void<F> * = nullptr>
    optional<T> constexpr or_else(F&& f)&& {
        return has_value() ? std::move(*this) : std::forward<F>(f)();
    }

    template<typename F, detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F&& f) const& {
        if (has_value())
            return *this;

        std::forward<F>(f)();
        return nullopt;
    }

    template<typename F, detail::disable_if_ret_void<F> * = nullptr>
    optional<T> constexpr or_else(F&& f) const& {
        return has_value() ? *this : std::forward<F>(f)();
    }

    template<typename F, detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F&& f) const&& {
        if (has_value()) return std::move(*this);

        std::forward<F>(f)();
        return nullopt;
    }

    template<typename F, detail::disable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F&& f) const&& {
        return has_value() ? std::move(*this) : std::forward<F>(f)();
    }

    /// Maps the stored value with `f` if there is one, otherwise returns `u`.
    template<typename F, typename U>
    U map_or(F&& f, U&& u)& {
        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u);
    }

    template<typename F, typename U>
    U map_or(F&& f, U&& u)&& {
        return has_value() ? detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u);
    }

    template<typename F, typename U>
    U map_or(F&& f, U&& u) const& {
        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u);
    }

    template<typename F, typename U>
    U map_or(F&& f, U&& u) const&& {
        return has_value() ? detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u);
    }

    /// Maps the stored value with `f` if there is one, otherwise calls
    /// `u` and returns the result.
    template<typename F, typename U>
    detail::invoke_result_t<U> map_or_else(F&& f, U&& u)& {
        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u)();
    }

    template<typename F, typename U>
    detail::invoke_result_t<U> map_or_else(F&& f, U&& u)&& {
        return has_value() ? detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u)();
    }

    template<typename F, typename U>
    detail::invoke_result_t<U> map_or_else(F&& f, U&& u) const& {
        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u)();
    }

    template<typename F, typename U>
    detail::invoke_result_t<U> map_or_else(F&& f, U&& u) const&& {
        return has_value() ? detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u)();
    }

    /// Returns `u` if `*this` has a value, otherwise an empty optional.
    template<typename U>
    constexpr optional<typename std::decay<U>::type> conjunction(U&& u) const {
        using result = optional<detail::decay_t<U>>;
        return has_value() ? result{ u } : result{ nullopt };
    }

    /// Returns `rhs` if `*this` is empty, otherwise the current value.
    constexpr optional disjunction(const optional& rhs)& {
        return has_value() ? *this : rhs;
    }

    constexpr optional disjunction(const optional& rhs) const& {
        return has_value() ? *this : rhs;
    }

    constexpr optional disjunction(const optional& rhs)&& {
        return has_value() ? std::move(*this) : rhs;
    }

    constexpr optional disjunction(const optional& rhs) const&& {
        return has_value() ? std::move(*this) : rhs;
    }

    constexpr optional disjunction(optional&& rhs)& {
        return has_value() ? *this : std::move(rhs);
    }

    constexpr optional disjunction(optional&& rhs) const& {
        return has_value() ? *this : std::move(rhs);
    }

    constexpr optional disjunction(optional&& rhs)&& {
        return has_value() ? std::move(*this) : std::move(rhs);
    }

    constexpr optional disjunction(optional&& rhs) const&& {
        return has_value() ? std::move(*this) : std::move(rhs);
    }

    /// Takes the value out of the optional, leaving it empty
    optional take() {
        optional ret = std::move(*this);
        reset();
        return ret;
    }

    using value_type = T;

    /// Constructs an optional that does not contain a value.
    constexpr optional() noexcept = default;

    constexpr optional(nullopt_t) noexcept {}

    /// Copy constructor
    ///
    /// If `rhs` contains a value, the stored value is direct-initialized with
    /// it. Otherwise, the constructed optional is empty.
    constexpr optional(const optional& rhs) = default;

    /// Move constructor
    ///
    /// If `rhs` contains a value, the stored value is direct-initialized with
    /// it. Otherwise, the constructed optional is empty.
    constexpr optional(optional&& rhs) noexcept = default;

    /// Constructs the stored value in-place using the given arguments.
    template<typename... Args>
    constexpr explicit optional(
        detail::enable_if_t<std::is_constructible<T, Args...>::value, in_place_t>,
        Args&& ... args)
        : base(in_place, std::forward<Args>(args)...) {}

    template<typename U, typename... Args>
    constexpr explicit optional(
        detail::enable_if_t<std::is_constructible<T, std::initializer_list<U>&,
            Args&& ...>::value,
            in_place_t>,
        std::initializer_list<U> il, Args&& ... args) {
        this->construct(il, std::forward<Args>(args)...);
    }

    /// Constructs the stored value with `u`.
    template<
        typename U = T,
        detail::enable_if_t<std::is_convertible<U&&, T>::value> * = nullptr,
        detail::enable_forward_value<T, U> * = nullptr
    >
    constexpr optional(U&& u) : base(in_place, std::forward<U>(u)) {}

    template<
        typename U = T,
        detail::enable_if_t<!std::is_convertible<U&&, T>::value> * = nullptr,
        detail::enable_forward_value<T, U> * = nullptr
    >
    constexpr explicit optional(U&& u) : base(in_place, std::forward<U>(u)) {}

    /// Converting copy constructor.
    template<
        typename U, detail::enable_from_other<T, U, const U&> * = nullptr,
        detail::enable_if_t<std::is_convertible<const U&, T>::value> * = nullptr
    >
    optional(const optional<U>& rhs) {
        if (rhs.has_value()) {
            this->construct(*rhs);
        }
    }

    template<typename U, detail::enable_from_other<T, U, const U&> * = nullptr,
        detail::enable_if_t<!std::is_convertible<const U&, T>::value> * =
        nullptr>
    explicit optional(const optional<U>& rhs) {
        if (rhs.has_value()) {
            this->construct(*rhs);
        }
    }

    /// Converting move constructor.
    template<
        typename U, detail::enable_from_other<T, U, U&&> * = nullptr,
        detail::enable_if_t<std::is_convertible<U&&, T>::value> * = nullptr
    >
    optional(optional<U>&& rhs) {
        if (rhs.has_value()) {
            this->construct(std::move(*rhs));
        }
    }

    template<
        typename U, detail::enable_from_other<T, U, U&&> * = nullptr,
        detail::enable_if_t<!std::is_convertible<U&&, T>::value> * = nullptr
    >
    explicit optional(optional<U>&& rhs) {
        if (rhs.has_value()) {
            this->construct(std::move(*rhs));
        }
    }

    /// Destroys the stored value if there is one.
    ~optional() = default;

    /// Assignment to empty.
    ///
    /// Destroys the current value if there is one.
    optional& operator =(nullopt_t) noexcept {
        if (has_value()) {
            this->m_value.~T();
            this->m_has_value = false;
        }

        return *this;
    }

    /// Copy assignment.
    ///
    /// Copies the value from `rhs` if there is one. Otherwise resets the stored
    /// value in `*this`.
    optional& operator =(const optional& rhs) = default;

    /// Move assignment.
    ///
    /// Moves the value from `rhs` if there is one. Otherwise resets the stored
    /// value in `*this`.
    optional& operator =(optional&& rhs) noexcept = default;

    /// Assigns the stored value from `u`, destroying the old value if there was
    /// one.
    template<typename U = T, detail::enable_assign_forward<T, U> * = nullptr>
    optional& operator =(U&& u) {
        if (has_value()) {
            this->m_value = std::forward<U>(u);
        } else {
            this->construct(std::forward<U>(u));
        }

        return *this;
    }

    /// Converting copy assignment operator.
    ///
    /// Copies the value from `rhs` if there is one. Otherwise resets the stored
    /// value in `*this`.
    template<typename U,
        detail::enable_assign_from_other<T, U, const U&> * = nullptr>
    optional& operator =(const optional<U>& rhs) {
        if (has_value()) {
            if (rhs.has_value()) {
                this->m_value = *rhs;
            } else {
                this->hard_reset();
            }
        }

        if (rhs.has_value()) {
            this->construct(*rhs);
        }

        return *this;
    }

    // TODO check exception guarantee
    /// Converting move assignment operator.
    ///
    /// Moves the value from `rhs` if there is one. Otherwise resets the stored
    /// value in `*this`.
    template<typename U, detail::enable_assign_from_other<T, U, U> * = nullptr>
    optional& operator =(optional<U>&& rhs) {
        if (has_value()) {
            if (rhs.has_value()) {
                this->m_value = std::move(*rhs);
            } else {
                this->hard_reset();
            }
        }

        if (rhs.has_value()) {
            this->construct(std::move(*rhs));
        }

        return *this;
    }

    /// Constructs the value in-place, destroying the current one if there is
    /// one.
    template<typename... Args>
    T& emplace(Args&& ... args) {
        static_assert(std::is_constructible<T, Args&& ...>::value,
                      "T must be constructible with Args");

        *this = nullopt;
        this->construct(std::forward<Args>(args)...);
        return value();
    }

    template<typename U, typename... Args>
    detail::enable_if_t<
        std::is_constructible<T, std::initializer_list<U>&, Args&& ...>::value,
        T&>
    emplace(std::initializer_list<U> il, Args&& ... args) {
        *this = nullopt;
        this->construct(il, std::forward<Args>(args)...);
        return value();
    }

    /// Swaps this optional with the other.
    ///
    /// If neither optionals have a value, nothing happens.
    /// If both have a value, the values are swapped.
    /// If one has a value, it is moved to the other and the movee is left
    /// valueless.
    void
    swap(optional& rhs) noexcept(std::is_nothrow_move_constructible<T>::value
                                 && detail::is_nothrow_swappable<T>::value) {
        using std::swap;
        if (has_value()) {
            if (rhs.has_value()) {
                swap(**this, *rhs);
            } else {
                new(std::addressof(rhs.m_value)) T(std::move(this->m_value));
                this->m_value.T::~T();
            }
        } else if (rhs.has_value()) {
            new(std::addressof(this->m_value)) T(std::move(rhs.m_value));
            rhs.m_value.T::~T();
        }
        swap(this->m_has_value, rhs.m_has_value);
    }

    /// Returns a pointer to the stored value
    constexpr const T *operator ->() const {
        return std::addressof(this->m_value);
    }

    constexpr T *operator ->() {
        return std::addressof(this->m_value);
    }

    /// Returns the stored value
    constexpr T& operator *()& { return this->m_value; }

    constexpr const T& operator *() const& { return this->m_value; }

    constexpr T&& operator *()&& {
        return std::move(this->m_value);
    }

    constexpr const T&& operator *() const&& { return std::move(this->m_value); }

    /// Returns whether or not the optional has a value
    constexpr bool has_value() const noexcept override { return this->m_has_value; }

    constexpr explicit operator bool() const noexcept {
        return this->m_has_value;
    }

    /// Returns the contained value if there is one, otherwise throws bad_optional_access
    constexpr T& value()& {
        if (has_value())
            return this->m_value;
        throw bad_optional_access();
    }

    constexpr const T& value() const& {
        if (has_value())
            return this->m_value;
        throw bad_optional_access();
    }

    constexpr T&& value()&& {
        if (has_value())
            return std::move(this->m_value);
        throw bad_optional_access();
    }

    constexpr const T&& value() const&& {
        if (has_value())
            return std::move(this->m_value);
        throw bad_optional_access();
    }

    /// Returns the stored value if there is one, otherwise returns `u`
    template<typename U>
    constexpr T value_or(U&& u) const& {
        static_assert(std::is_copy_constructible<T>::value &&
                      std::is_convertible<U&&, T>::value,
                      "T must be copy constructible and convertible from U");
        return has_value() ? **this : static_cast<T>(std::forward<U>(u));
    }

    template<typename U>
    constexpr T value_or(U&& u)&& {
        static_assert(std::is_move_constructible<T>::value &&
                      std::is_convertible<U&&, T>::value,
                      "T must be move constructible and convertible from U");
        return has_value() ? **this : static_cast<T>(std::forward<U>(u));
    }

    /// Destroys the stored value if one exists, making the optional empty
    void reset() noexcept {
        if (has_value()) {
            this->m_value.~T();
            this->m_has_value = false;
        }
    }
}; // namespace tl

/// Compares two optional objects
template<typename T, typename U>
inline constexpr bool operator ==(const optional<T>& lhs,
                                  const optional<U>& rhs) {
    return lhs.has_value() == rhs.has_value() &&
           (!lhs.has_value() || *lhs == *rhs);
}

template<typename T, typename U>
inline constexpr bool operator !=(const optional<T>& lhs,
                                  const optional<U>& rhs) {
    return lhs.has_value() != rhs.has_value() ||
           (lhs.has_value() && *lhs != *rhs);
}

template<typename T, typename U>
inline constexpr bool operator <(const optional<T>& lhs,
                                 const optional<U>& rhs) {
    return rhs.has_value() && (!lhs.has_value() || *lhs < *rhs);
}

template<typename T, typename U>
inline constexpr bool operator >(const optional<T>& lhs,
                                 const optional<U>& rhs) {
    return lhs.has_value() && (!rhs.has_value() || *lhs > *rhs);
}

template<typename T, typename U>
inline constexpr bool operator <=(const optional<T>& lhs,
                                  const optional<U>& rhs) {
    return !lhs.has_value() || (rhs.has_value() && *lhs <= *rhs);
}

template<typename T, typename U>
inline constexpr bool operator >=(const optional<T>& lhs,
                                  const optional<U>& rhs) {
    return !rhs.has_value() || (lhs.has_value() && *lhs >= *rhs);
}

/// Compares an optional to a `nullopt`
template<typename T>
inline constexpr bool operator ==(const optional<T>& lhs, nullopt_t) noexcept {
    return !lhs.has_value();
}

template<typename T>
inline constexpr bool operator ==(nullopt_t, const optional<T>& rhs) noexcept {
    return !rhs.has_value();
}

template<typename T>
inline constexpr bool operator !=(const optional<T>& lhs, nullopt_t) noexcept {
    return lhs.has_value();
}

template<typename T>
inline constexpr bool operator !=(nullopt_t, const optional<T>& rhs) noexcept {
    return rhs.has_value();
}

template<typename T>
inline constexpr bool operator <(const optional<T>&, nullopt_t) noexcept {
    return false;
}

template<typename T>
inline constexpr bool operator <(nullopt_t, const optional<T>& rhs) noexcept {
    return rhs.has_value();
}

template<typename T>
inline constexpr bool operator <=(const optional<T>& lhs, nullopt_t) noexcept {
    return !lhs.has_value();
}

template<typename T>
inline constexpr bool operator <=(nullopt_t, const optional<T>&) noexcept {
    return true;
}

template<typename T>
inline constexpr bool operator >(const optional<T>& lhs, nullopt_t) noexcept {
    return lhs.has_value();
}

template<typename T>
inline constexpr bool operator >(nullopt_t, const optional<T>&) noexcept {
    return false;
}

template<typename T>
inline constexpr bool operator >=(const optional<T>&, nullopt_t) noexcept {
    return true;
}

template<typename T>
inline constexpr bool operator >=(nullopt_t, const optional<T>& rhs) noexcept {
    return !rhs.has_value();
}

/// Compares the optional with a value.
template<typename T, typename U>
inline constexpr bool operator ==(const optional<T>& lhs, const U& rhs) {
    return lhs.has_value() && *lhs == rhs;
}

template<typename T, typename U>
inline constexpr bool operator ==(const U& lhs, const optional<T>& rhs) {
    return rhs.has_value() && lhs == *rhs;
}

template<typename T, typename U>
inline constexpr bool operator !=(const optional<T>& lhs, const U& rhs) {
    return !lhs.has_value() || *lhs != rhs;
}

template<typename T, typename U>
inline constexpr bool operator !=(const U& lhs, const optional<T>& rhs) {
    return !rhs.has_value() || lhs != *rhs;
}

template<typename T, typename U>
inline constexpr bool operator <(const optional<T>& lhs, const U& rhs) {
    return !lhs.has_value() || *lhs < rhs;
}

template<typename T, typename U>
inline constexpr bool operator <(const U& lhs, const optional<T>& rhs) {
    return rhs.has_value() && lhs < *rhs;
}

template<typename T, typename U>
inline constexpr bool operator <=(const optional<T>& lhs, const U& rhs) {
    return !lhs.has_value() || *lhs <= rhs;
}

template<typename T, typename U>
inline constexpr bool operator <=(const U& lhs, const optional<T>& rhs) {
    return rhs.has_value() && lhs <= *rhs;
}

template<typename T, typename U>
inline constexpr bool operator >(const optional<T>& lhs, const U& rhs) {
    return lhs.has_value() && *lhs > rhs;
}

template<typename T, typename U>
inline constexpr bool operator >(const U& lhs, const optional<T>& rhs) {
    return !rhs.has_value() || lhs > *rhs;
}

template<typename T, typename U>
inline constexpr bool operator >=(const optional<T>& lhs, const U& rhs) {
    return lhs.has_value() && *lhs >= rhs;
}

template<typename T, typename U>
inline constexpr bool operator >=(const U& lhs, const optional<T>& rhs) {
    return !rhs.has_value() || lhs >= *rhs;
}

template<typename T,
    detail::enable_if_t<std::is_move_constructible<T>::value> * = nullptr,
    detail::enable_if_t<detail::is_swappable<T>::value> * = nullptr>
void swap(optional<T>& lhs,
          optional<T>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    return lhs.swap(rhs);
}

namespace detail {
struct i_am_secret {};
} // namespace detail

template<typename T = detail::i_am_secret, typename U,
    typename Ret =
    detail::conditional_t<std::is_same<T, detail::i_am_secret>::value,
        detail::decay_t<U>, T>>
inline constexpr optional<Ret> make_optional(U&& v) {
    return optional<Ret>(std::forward<U>(v));
}

template<typename T, typename... Args>
inline constexpr optional<T> make_optional(Args&& ... args) {
    return optional<T>(in_place, std::forward<Args>(args)...);
}

template<typename T, typename U, typename... Args>
inline constexpr optional<T> make_optional(std::initializer_list<U> il,
                                           Args&& ... args) {
    return optional<T>(in_place, il, std::forward<Args>(args)...);
}

/// \exclude
namespace detail {

template<class Opt, typename F,
    typename Ret = decltype(detail::invoke(std::declval<F>(),
                                           *std::declval<Opt>())),
    detail::enable_if_t<!std::is_void<Ret>::value> * = nullptr>
constexpr auto optional_map_impl(Opt&& opt, F&& f) {
    return opt.has_value()
           ? detail::invoke(std::forward<F>(f), *std::forward<Opt>(opt))
           : optional<Ret>(nullopt);
}

template<class Opt, typename F,
    typename Ret = decltype(detail::invoke(std::declval<F>(),
                                           *std::declval<Opt>())),
    detail::enable_if_t<std::is_void<Ret>::value> * = nullptr>
auto optional_map_impl(Opt&& opt, F&& f) {
    if (opt.has_value()) {
        detail::invoke(std::forward<F>(f), *std::forward<Opt>(opt));
        return make_optional(monostate{});
    }

    return optional<monostate>(nullopt);
}

} // namespace detail

/// Specialization for when `T` is a reference. `optional<T&>` acts similarly
/// to a `T*`, but provides more operations and shows intent more clearly.
template<typename T>
class optional<T&> {
public:
// The different versions for C++14 and 11 are needed because deduced return
// types are not SFINAE-safe. This provides better support for things like
// generic lambdas. C.f.
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0826r0.html

    /// Carries out some operation which returns an optional on the stored
    /// object if there is one.
    template<typename F>
    constexpr auto and_then(F&& f)& {
        using result = detail::invoke_result_t<F, T&>;
        static_assert(detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    template<typename F>
    constexpr auto and_then(F&& f)&& {
        using result = detail::invoke_result_t<F, T&>;
        static_assert(detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    template<typename F>
    constexpr auto and_then(F&& f) const& {
        using result = detail::invoke_result_t<F, const T&>;
        static_assert(detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }


    template<typename F>
    constexpr auto and_then(F&& f) const&& {
        using result = detail::invoke_result_t<F, const T&>;
        static_assert(detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    /// Carries out some operation on the stored object if there is one.
    template<typename F>
    constexpr auto map(F&& f)& {
        return detail::optional_map_impl(*this, std::forward<F>(f));
    }

    template<typename F>
    constexpr auto map(F&& f)&& {
        return detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template<typename F>
    constexpr auto map(F&& f) const& {
        return detail::optional_map_impl(*this, std::forward<F>(f));
    }

    template<typename F>
    constexpr auto map(F&& f) const&& {
        return detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    /// Carries out some operation on the stored object if there is one.
    template<typename F>
    constexpr auto transform(F&& f)& {
        return detail::optional_map_impl(*this, std::forward<F>(f));
    }

    template<typename F>
    constexpr auto transform(F&& f)&& {
        return detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template<typename F>
    constexpr auto transform(F&& f) const& {
        return detail::optional_map_impl(*this, std::forward<F>(f));
    }

    template<typename F>
    constexpr auto transform(F&& f) const&& {
        return detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    /// Calls `f` if the optional is empty
    template<typename F, detail::enable_if_ret_void<F> * = nullptr>
    optional<T> constexpr or_else(F&& f)& {
        if (has_value())
            return *this;

        std::forward<F>(f)();
        return nullopt;
    }

    template<typename F, detail::disable_if_ret_void<F> * = nullptr>
    optional<T> constexpr or_else(F&& f)& {
        return has_value() ? *this : std::forward<F>(f)();
    }

    template<typename F, detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F&& f)&& {
        if (has_value())
            return std::move(*this);

        std::forward<F>(f)();
        return nullopt;
    }

    template<typename F, detail::disable_if_ret_void<F> * = nullptr>
    optional<T> constexpr or_else(F&& f)&& {
        return has_value() ? std::move(*this) : std::forward<F>(f)();
    }

    template<typename F, detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F&& f) const& {
        if (has_value())
            return *this;

        std::forward<F>(f)();
        return nullopt;
    }

    template<typename F, detail::disable_if_ret_void<F> * = nullptr>
    optional<T> constexpr or_else(F&& f) const& {
        return has_value() ? *this : std::forward<F>(f)();
    }

    template<typename F, detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F&& f) const&& {
        if (has_value())
            return std::move(*this);

        std::forward<F>(f)();
        return nullopt;
    }

    template<typename F, detail::disable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F&& f) const&& {
        return has_value() ? std::move(*this) : std::forward<F>(f)();
    }

    /// Maps the stored value with `f` if there is one, otherwise returns `u`
    template<typename F, typename U>
    U map_or(F&& f, U&& u)& {
        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u);
    }

    template<typename F, typename U>
    U map_or(F&& f, U&& u)&& {
        return has_value() ? detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u);
    }

    template<typename F, typename U>
    U map_or(F&& f, U&& u) const& {
        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u);
    }

    template<typename F, typename U>
    U map_or(F&& f, U&& u) const&& {
        return has_value() ? detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u);
    }

    /// Maps the stored value with `f` if there is one, otherwise calls
    /// `u` and returns the result.
    template<typename F, typename U>
    detail::invoke_result_t<U> map_or_else(F&& f, U&& u)& {
        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u)();
    }

    template<typename F, typename U>
    detail::invoke_result_t<U> map_or_else(F&& f, U&& u)&& {
        return has_value() ? detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u)();
    }

    template<typename F, typename U>
    detail::invoke_result_t<U> map_or_else(F&& f, U&& u) const& {
        return has_value() ? detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u)();
    }

    template<typename F, typename U>
    detail::invoke_result_t<U> map_or_else(F&& f, U&& u) const&& {
        return has_value() ? detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u)();
    }

    /// Returns `u` if `*this` has a value, otherwise an empty optional.
    template<typename U>
    constexpr optional<typename std::decay<U>::type> conjunction(U&& u) const {
        using result = optional<detail::decay_t<U>>;
        return has_value() ? result{ u } : result{ nullopt };
    }

    /// Returns `rhs` if `*this` is empty, otherwise the current value.
    constexpr optional disjunction(const optional& rhs)& {
        return has_value() ? *this : rhs;
    }

    constexpr optional disjunction(const optional& rhs) const& {
        return has_value() ? *this : rhs;
    }

    constexpr optional disjunction(const optional& rhs)&& {
        return has_value() ? std::move(*this) : rhs;
    }

    constexpr optional disjunction(const optional& rhs) const&& {
        return has_value() ? std::move(*this) : rhs;
    }

    constexpr optional disjunction(optional&& rhs)& {
        return has_value() ? *this : std::move(rhs);
    }

    constexpr optional disjunction(optional&& rhs) const& {
        return has_value() ? *this : std::move(rhs);
    }

    constexpr optional disjunction(optional&& rhs)&& {
        return has_value() ? std::move(*this) : std::move(rhs);
    }

    constexpr optional disjunction(optional&& rhs) const&& {
        return has_value() ? std::move(*this) : std::move(rhs);
    }

    /// Takes the value out of the optional, leaving it empty
    optional take() {
        optional ret = std::move(*this);
        reset();
        return ret;
    }

    using value_type = T&;

    /// Constructs an optional that does not contain a value.
    constexpr optional() noexcept: m_value(nullptr) {}

    constexpr optional(nullopt_t) noexcept: m_value(nullptr) {}

    /// Copy constructor
    ///
    /// If `rhs` contains a value, the stored value is direct-initialized with
    /// it. Otherwise, the constructed optional is empty.
    constexpr optional(const optional& rhs) noexcept = default;

    /// Move constructor
    ///
    /// If `rhs` contains a value, the stored value is direct-initialized with
    /// it. Otherwise, the constructed optional is empty.
    constexpr optional(optional&& rhs) noexcept = default;

    /// Constructs the stored value with `u`.
    template<typename U = T,
        detail::enable_if_t<!detail::is_optional<detail::decay_t<U>>::value>
        * = nullptr>
    constexpr optional(U&& u) noexcept : m_value(std::addressof(u)) {
        static_assert(std::is_lvalue_reference<U>::value, "U must be an lvalue");
    }

    template<typename U>
    constexpr explicit optional(const optional<U>& rhs) noexcept : optional(*rhs) {}

    /// No-op
    ~optional() = default;

    /// Assignment to empty.
    ///
    /// Destroys the current value if there is one.
    optional& operator =(nullopt_t) noexcept {
        m_value = nullptr;
        return *this;
    }

    /// Copy assignment.
    ///
    /// Rebinds this optional to the referee of `rhs` if there is one. Otherwise
    /// resets the stored value in `*this`.
    optional& operator =(const optional& rhs) = default;

    /// Rebinds this optional to `u`.
    template<typename U = T,
        detail::enable_if_t<!detail::is_optional<detail::decay_t<U>>::value>
        * = nullptr>
    optional& operator =(U&& u) {
        static_assert(std::is_lvalue_reference<U>::value, "U must be an lvalue");
        m_value = std::addressof(u);
        return *this;
    }

    /// Converting copy assignment operator.
    ///
    /// Rebinds this optional to the referee of `rhs` if there is one. Otherwise
    /// resets the stored value in `*this`.
    template<typename U>
    optional& operator =(const optional<U>& rhs) noexcept {
        m_value = std::addressof(rhs.value());
        return *this;
    }

    /// Rebinds this optional to `u`.
    template<typename U = T,
        detail::enable_if_t<!detail::is_optional<detail::decay_t<U>>::value>
        * = nullptr>
    optional& emplace(U&& u) noexcept {
        return *this = std::forward<U>(u);
    }

    void swap(optional& rhs) noexcept { std::swap(m_value, rhs.m_value); }

    /// Returns a pointer to the stored value
    constexpr const T *operator ->() const noexcept { return m_value; }

    constexpr T *operator ->() noexcept { return m_value; }

    /// Returns the stored value
    constexpr T& operator *() noexcept { return *m_value; }

    constexpr const T& operator *() const noexcept { return *m_value; }

    constexpr bool has_value() const noexcept { return m_value != nullptr; }

    constexpr explicit operator bool() const noexcept {
        return m_value != nullptr;
    }

    /// Returns the contained value if there is one, otherwise throws bad_optional_access
    constexpr T& value() {
        if (has_value())
            return *m_value;
        throw bad_optional_access();
    }

    constexpr const T& value() const {
        if (has_value())
            return *m_value;
        throw bad_optional_access();
    }

    /// Returns the stored value if there is one, otherwise returns `u`
    template<typename U>
    constexpr T value_or(U&& u) const& noexcept {
        static_assert(std::is_copy_constructible<T>::value &&
                      std::is_convertible<U&&, T>::value,
                      "T must be copy constructible and convertible from U");
        return has_value() ? **this : static_cast<T>(std::forward<U>(u));
    }

    /// \group value_or
    template<typename U>
    constexpr T value_or(U&& u)&& noexcept {
        static_assert(std::is_move_constructible<T>::value &&
                      std::is_convertible<U&&, T>::value,
                      "T must be move constructible and convertible from U");
        return has_value() ? **this : static_cast<T>(std::forward<U>(u));
    }

    /// Destroys the stored value if one exists, making the optional empty
    void reset() noexcept { m_value = nullptr; }

private:
    T *m_value;
}; // namespace vpk

}

#endif

#endif //VPACKCORE_OPTIONAL_H
