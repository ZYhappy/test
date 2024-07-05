
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/15/2020 14:37
//

#ifndef MARK_ALIAS_TYPE_TRAITS_INTO_HPP_
#define MARK_ALIAS_TYPE_TRAITS_INTO_HPP_

#include "mark/alias_type/alias_type_trait.hpp"
#include "mark/alias_type/traits/traits.hpp"

namespace mark {
namespace traits {

template <typename AliasType_, typename = void>
struct Into : Self<AliasType_, Into> {
  template<typename IntoType>
  using impl_t = decltype(std::declval<IntoType &>() = Impl<Into<AliasType_>, IntoType>::into(std::declval<AliasType_>()));

  using Self<AliasType_, ::mark::traits::Into>::self;

  template<typename IntoType, has_impl_t<impl_t, IntoType> = 0>
  IntoType into() const {
    return Impl<Into, IntoType>::into(this->self());
  }
};

template <typename AliasType_, typename IntoType>
struct Impl<Into<AliasType_,
  std::enable_if_t<
  std::is_same_v<remove_alias_t<AliasType_>, IntoType>>>, IntoType> {
  static IntoType into(const AliasType_ &value) {
    return value.value();
  }
};

template <typename AliasType_, typename IntoType>
struct Impl<Into<AliasType_,
  std::enable_if_t<
    not std::is_same_v<AliasType_, IntoType> and
    not std::is_same_v<remove_alias_t<AliasType_>, IntoType> and
    std::is_same_v<remove_all_alias_t<AliasType_>, IntoType>>>, IntoType> {
  static IntoType into(const AliasType_ &value) {
    static_assert(has_impl_v<Into<remove_alias_t<AliasType_>>::template impl_t, IntoType>);
    return Impl<Into<remove_alias_t<AliasType_>>, IntoType>::into(value.value());
  }
};

}
}

#endif  // !MARK_ALIAS_TYPE_TRAITS_INTO_HPP_
