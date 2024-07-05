
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/15/2020 14:13
//

#ifndef MARK_ALIAS_TYPE_TRAITS_HPP_
#define MARK_ALIAS_TYPE_TRAITS_HPP_

#include <ciso646>
#include <type_traits>
#include "mark/detector.hpp"
#include "mark/alias_type/alias_type_define.hpp"

namespace mark::traits {

template <typename...>
struct Impl {};

template <template<typename...> class Trait, typename... Args>
using has_impl = is_detected<Trait, Args...>;

template <template<typename...> class Trait, typename... Args>
constexpr bool has_impl_v = has_impl<Trait, Args...>::value;

template <template<typename...> class Trait, typename... Args>
using has_impl_t = std::enable_if_t<has_impl<Trait, Args...>::value, int>;

template <template<typename...> class Trait, typename... Args>
using not_has_impl_t = std::enable_if_t<not has_impl<Trait, Args...>::value, int>;

template <typename T, template <typename...> class Trait>
struct Self {
  constexpr T& self() { return static_cast<T&>(*this); }
  constexpr const T& self() const { return static_cast<const T&>(*this); }
  friend Trait<T>;
};

}  // namespace mark::traits

#endif  // !MARK_ALIAS_TYPE_TRAITS_HPP_
