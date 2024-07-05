
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|

///
/// \file
///
/// 一些通用的元编程工具
///
/// \author: Zhengyu
/// \date: 08/01/2020 12:43:34
///

#ifndef MARK_UTILITY_HPP_
#define MARK_UTILITY_HPP_

#include <utility>
#include <type_traits>

#include "mark/config.hpp"

namespace mark {

//------------------------------------------------------------------------------

struct wildcard {
  constexpr wildcard() noexcept {}
  template <typename S> struct accepted_type_for { typedef S type; };
#if defined(__GNUC__)
  template <typename T>
    constexpr bool operator()(const T&) const noexcept { return true; }
#else
  constexpr bool operator()(...)      const noexcept { return true; }
#endif
};

constexpr wildcard _;

//------------------------------------------------------------------------------

using in_place_t = std::in_place_t;

inline constexpr in_place_t in_place{};

//------------------------------------------------------------------------------

template<typename T>
using remove_cvref = std::remove_cv<
  typename std::remove_reference<T>::type>;

template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

//------------------------------------------------------------------------------

template <bool Last, class ...Preds>
struct __lazy_and_impl;

template <class ...Preds>
struct __lazy_and_impl<false, Preds...> : std::false_type {};

template <>
struct __lazy_and_impl<true> : std::true_type {};

template <class Pred>
struct __lazy_and_impl<true, Pred> : std::integral_constant<bool, Pred::type::value> {};

template <class Hp, class ...Tp>
struct __lazy_and_impl<true, Hp, Tp...> : __lazy_and_impl<Hp::type::value, Tp...> {};

template <class P1, class ...Pr>
struct __lazy_and: __lazy_and_impl<P1::type::value, Pr...> {};

//------------------------------------------------------------------------------

// __lazy_or

template <bool List, class ...Preds>
struct __lazy_or_impl;

template <class ...Preds>
struct __lazy_or_impl<true, Preds...> : std::true_type {};

template <>
struct __lazy_or_impl<false> : std::false_type {};

template <class Hp, class ...Tp>
struct __lazy_or_impl<false, Hp, Tp...>
  : __lazy_or_impl<Hp::type::value, Tp...> {};

template <class P1, class ...Pr>
struct __lazy_or : __lazy_or_impl<P1::type::value, Pr...> {};

//------------------------------------------------------------------------------

// __lazy_not

template <class Pred>
struct __lazy_not : std::integral_constant<bool, !Pred::type::value> {};

template<bool __v>
using __bool_constant = __MARK_STD_CXX_11::integral_constant<bool, __v>;

//------------------------------------------------------------------------------

template<typename...>
struct __or;

template<>
struct __or<>
  : public std::true_type {};

template<typename T1>
struct __or<T1>
  : public T1 {};

template<typename T1, typename T2>
struct __or<T1, T2>
  : public std::conditional<T1::value, T1, T2>::type {};

template<typename T1, typename T2, typename T3, typename... Tn>
struct __or<T1, T2, T3, Tn...>
  : public std::conditional<T1::value, T1, __or<T2, T3, Tn...>>::type {};

//------------------------------------------------------------------------------

template<typename...>
struct __and;

template<>
struct __and<>
  : public std::true_type {};

template<typename T1>
struct __and<T1>
  : public T1 {};

template<typename T1, typename T2>
struct __and<T1, T2>
  : public std::conditional<T1::value, T2, T1>::type {};

template<typename T1, typename T2, typename T3, typename... Tn>
struct __and<T1, T2, T3, Tn...>
  : public std::conditional<T1::value, __and<T2, T3, Tn...>, T1>::type {};

//------------------------------------------------------------------------------

template<typename P>
struct __not
  : public __bool_constant<!bool(P::value)> {};

#ifdef HAS_CXX_17

template<typename... Tn>
inline constexpr bool __or_v = __or<Tn...>::value;

template<typename... Tn>
inline constexpr bool __and_v = __and<Tn...>::value;

template<typename P>
inline constexpr bool __not_v = __not<P>::value;

#endif  // !HAS_CXX_17

//------------------------------------------------------------------------------

template <bool ...Preds>
struct __all_dummy;

template <bool ...Pred>
using __all = std::is_same<__all_dummy<Pred...>, __all_dummy<((void)Pred, true)...>>;

//------------------------------------------------------------------------------

template <bool CanCopy, bool CanMove>
struct __sfinae_ctor_base {};

template <>
struct __sfinae_ctor_base<false, false> {
  __sfinae_ctor_base() = default;
  __sfinae_ctor_base(__sfinae_ctor_base const&) = delete;
  __sfinae_ctor_base(__sfinae_ctor_base &&) = delete;
  __sfinae_ctor_base& operator=(__sfinae_ctor_base const&) = default;
  __sfinae_ctor_base& operator=(__sfinae_ctor_base&&) = default;
};
template <>
struct __sfinae_ctor_base<true, false> {
  __sfinae_ctor_base() = default;
  __sfinae_ctor_base(__sfinae_ctor_base const&) = default;
  __sfinae_ctor_base(__sfinae_ctor_base &&) = delete;
  __sfinae_ctor_base& operator=(__sfinae_ctor_base const&) = default;
  __sfinae_ctor_base& operator=(__sfinae_ctor_base&&) = default;
};
template <>
struct __sfinae_ctor_base<false, true> {
  __sfinae_ctor_base() = default;
  __sfinae_ctor_base(__sfinae_ctor_base const&) = delete;
  __sfinae_ctor_base(__sfinae_ctor_base &&) = default;
  __sfinae_ctor_base& operator=(__sfinae_ctor_base const&) = default;
  __sfinae_ctor_base& operator=(__sfinae_ctor_base&&) = default;
};

//------------------------------------------------------------------------------

template <bool CanCopy, bool CanMove>
struct __sfinae_assign_base {};

template <>
struct __sfinae_assign_base<false, false> {
  __sfinae_assign_base() = default;
  __sfinae_assign_base(__sfinae_assign_base const&) = default;
  __sfinae_assign_base(__sfinae_assign_base &&) = default;
  __sfinae_assign_base& operator=(__sfinae_assign_base const&) = delete;
  __sfinae_assign_base& operator=(__sfinae_assign_base&&) = delete;
};
template <>
struct __sfinae_assign_base<true, false> {
  __sfinae_assign_base() = default;
  __sfinae_assign_base(__sfinae_assign_base const&) = default;
  __sfinae_assign_base(__sfinae_assign_base &&) = default;
  __sfinae_assign_base& operator=(__sfinae_assign_base const&) = default;
  __sfinae_assign_base& operator=(__sfinae_assign_base&&) = delete;
};
template <>
struct __sfinae_assign_base<false, true> {
  __sfinae_assign_base() = default;
  __sfinae_assign_base(__sfinae_assign_base const&) = default;
  __sfinae_assign_base(__sfinae_assign_base &&) = default;
  __sfinae_assign_base& operator=(__sfinae_assign_base const&) = delete;
  __sfinae_assign_base& operator=(__sfinae_assign_base&&) = default;
};

//------------------------------------------------------------------------------

// 检测能否使用U构造T
template <typename T>
struct __check_args_constructor {
  template <typename U>
  static constexpr bool __enable_implicit() {
    return std::is_constructible_v<T, U&&> &&
           std::is_convertible_v<U&&, T>;
  }

  template<typename U>
  static constexpr bool __enable_explicit() {
    return std::is_constructible_v<T, U&&> &&
           !std::is_convertible_v<U&&, T>;
  }

  template<typename U>
  struct _enable_implicit {
    static constexpr auto value = __enable_implicit<U>();
  };

  template<typename U>
  struct _enable_explicit {
    static constexpr auto value = __enable_explicit<U>();
  };
};

template <typename T>
struct __check_args_constructor_fail {
  template <typename>
  static constexpr bool __enable_implicit() {
    return false;
  }

  template<typename>
  static constexpr bool __enable_explicit() {
    return false;
  }

  template<typename U>
  static constexpr auto __enable_implicit_v = __enable_implicit<U>();

  template<typename U>
  static constexpr auto __enable_explicit_v = __enable_explicit<U>();

  template<typename U>
  struct _enable_implicit {
    static constexpr auto value = __enable_implicit<U>();
  };

  template<typename U>
  struct _enable_explicit {
    static constexpr auto value = __enable_explicit<U>();
  };
};

template <typename T, typename Ty, typename Up>
using check_args_ctor =
std::conditional_t<
  !std::is_same_v<remove_cvref_t<Up>, in_place_t> &&
  !std::is_same_v<remove_cvref_t<Up>, T>,
  __check_args_constructor<Ty>,
  __check_args_constructor_fail<Ty>
>;

//------------------------------------------------------------------------------

template<template<typename> class T, typename Ty, typename QualUp>
struct __check_like_constructor {
  template<typename Up, typename Target = T<Up>>
  using __check_constructible_from_target = __lazy_or<
    std::is_constructible<Ty, Target&>,
    std::is_constructible<Ty, const Target&>,
    std::is_constructible<Ty, Target&&>,
    std::is_constructible<Ty, const Target&&>,
    std::is_convertible<Target&, Ty>,
    std::is_convertible<const Target&, Ty>,
    std::is_convertible<Target&&, Ty>,
    std::is_convertible<const Target&&, Ty>>;

  template<typename Up, typename Target = T<Up>>
  using __check_assignable_from_target = __lazy_or<
    std::is_assignable<Ty&, Target&>,
    std::is_assignable<Ty&, const Target&>,
    std::is_assignable<Ty&, Target&&>,
    std::is_assignable<Ty&, const Target&&>>;

  template<typename Up, typename QUp = QualUp>
  static constexpr bool __enable_implicit() {
    return std::is_convertible<QUp, Ty>::value &&
           !__check_constructible_from_target<Up>::value;
  }

  template<typename Up, typename QUp = QualUp>
  static constexpr bool __enable_explicit() {
    return !std::is_convertible<QUp, Ty>::value &&
           !__check_constructible_from_target<Up>::value;
  }

  template<typename Up, typename QUp = QualUp>
  static constexpr bool __enable_assign() {
    return !__check_constructible_from_target<Up>::value &&
           !__check_assignable_from_target<Up>::value;
  }

  template<typename Up, typename QUp = QualUp>
  static constexpr auto __enable_implicit_v = __enable_implicit<Up, QUp>();

  template<typename Up, typename QUp = QualUp>
  static constexpr auto __enable_explicit_v = __enable_explicit<Up, QUp>();

  template<typename Up, typename QUp = QualUp>
  static constexpr auto __enable_assign_v = __enable_assign<Up, QUp>();

  template<typename Up, typename QUp = QualUp>
  struct _enable_implicit {
    static constexpr auto value = __enable_implicit<Up, QUp>();
  };

  template<typename Up, typename QUp = QualUp>
  struct _enable_explicit {
    static constexpr auto value = __enable_explicit<Up, QUp>();
  };

  template<typename Up, typename QUp = QualUp>
  struct _enable_assign {
    static constexpr auto value = __enable_assign<Up, QUp>();
  };
};

template<template<typename> class T, typename Ty, typename QualUp>
struct __check_like_constructor_fail {
  template<typename Up, typename QUp = QualUp>
  static constexpr bool __enable_implicit() {
    return false;
  }

  template<typename Up, typename QUp = QualUp>
  static constexpr bool __enable_explicit() {
    return false;
  }

  template<typename Up, typename QUp = QualUp>
  static constexpr bool __enable_assign() {
    return false;
  }

  template<typename Up, typename QUp = QualUp>
  static constexpr auto __enable_implicit_v = __enable_implicit<Up, QUp>();

  template<typename Up, typename QUp = QualUp>
  static constexpr auto __enable_explicit_v = __enable_explicit<Up, QUp>();

  template<typename Up, typename QUp = QualUp>
  static constexpr auto __enable_assign_v = __enable_assign<Up, QUp>();

  template<typename Up, typename QUp = QualUp>
  struct _enable_implicit {
    static constexpr auto value = __enable_implicit<Up, QUp>();
  };

  template<typename Up, typename QUp = QualUp>
  struct _enable_explicit {
    static constexpr auto value = __enable_explicit<Up, QUp>();
  };

  template<typename Up, typename QUp = QualUp>
  struct _enable_assign {
    static constexpr auto value = __enable_assign<Up, QUp>();
  };
};

template<template<typename> class T, typename Ty, typename Up, typename QualUp>
using check_like_ctor = typename std::conditional<
  __lazy_and<
    __lazy_not<std::is_same<Up, Ty>>,
    std::is_constructible<Ty, QualUp>>::value,
  __check_like_constructor<T, Ty, QualUp>,
  __check_like_constructor_fail<T, Ty, QualUp>>::type;

template<template<typename> class T, typename Ty, typename Up, typename QualUp>
using check_like_assign = typename std::conditional<
  __lazy_and<
    __lazy_not<std::is_same<Up, Ty>>,
    std::is_constructible<Ty, QualUp>,
    std::is_assignable<Ty&, QualUp>>::value,
  __check_like_constructor<T, Ty, QualUp>,
  __check_like_constructor_fail<T, Ty, QualUp>>::type;


//------------------------------------------------------------------------------

template <typename T, typename Fn>
struct is_member_function_pointer_of : std::false_type {};

template <typename T, typename U>
struct is_member_function_pointer_of<T, U T::*> : std::true_type {};

template <typename T, typename Fn>
constexpr bool is_member_function_pointer_of_v = is_member_function_pointer_of<T, Fn>::value;

//------------------------------------------------------------------------------

}  // namespace mark

#endif  // !MARK_UTILITY_HPP_
