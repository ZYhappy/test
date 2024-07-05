
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/16/2020 13:42
//

#ifndef MARK_ALIAS_TYPE_TRAITS_FROM_HPP_
#define MARK_ALIAS_TYPE_TRAITS_FROM_HPP_

#include "mark/alias_type/traits/traits.hpp"
#include "mark/utility.hpp"

namespace mark::traits {

template <typename AliasType, typename = void>
struct From : Self<AliasType, From> {
  template <typename FromType>
  using __impl_t = decltype(std::declval<AliasType&>() = Impl<From, FromType>::from(std::declval<const FromType&>()));

  template <typename FromType, typename _FromType = remove_cvref_t<FromType>, has_impl_t<__impl_t, _FromType> = 0>
  static AliasType from(FromType&& value) {
    return Impl<From, _FromType>::from(std::forward<FromType>(value));
  }
};

}  // namespace mark::traits

#endif  // !MARK_ALIAS_TYPE_TRAITS_FROM_HPP_
