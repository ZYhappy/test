
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

#ifndef MARK_ALIAS_TYPE_TRAITS_AS_HPP_
#define MARK_ALIAS_TYPE_TRAITS_AS_HPP_

#include "mark/utility.hpp"

#include "mark/alias_type/alias_type_trait.hpp"
#include "mark/alias_type/traits/traits.hpp"

namespace mark {
namespace traits {

template <typename AliasType_, typename = void>
struct As : Self<AliasType_, As> {
  template <typename AsType>
  using __impl_t = decltype(Impl<As, AsType>::as(std::declval<AliasType_&>()));

  using Self<AliasType_, ::mark::traits::As>::self;

  template <typename AsType, has_impl_t<__impl_t, AsType> = 0>
  AsType& as() {
    return Impl<As, AsType>::as(this->self());
  }

  template <typename AsType, has_impl_t<__impl_t, AsType> = 0>
  const AsType& as() const {
    return const_cast<As*>(this)->template as<AsType>();
  }
};

template <typename AliasType_, typename AsType>
struct Impl<As<AliasType_,
  std::enable_if_t<
    not std::is_same_v<AliasType_, AsType> and
    std::is_same_v<remove_alias_t<AliasType_>, AsType>>>, AsType> {
  static AsType& as(AliasType_& value) {
    return value.value();
  }
};

template <typename AliasType_, typename AsType>
struct Impl<As<AliasType_,
  std::enable_if_t<
    not std::is_same_v<AliasType_, AsType> and
    not std::is_same_v<remove_alias_t<AliasType_>, AsType> and
    std::is_same_v<remove_all_alias_t<AliasType_>, AsType>>>, AsType> {
  static AsType& as(AliasType_& value) {
    static_assert(has_impl_v<As<remove_alias_t<AliasType_>>::template __impl_t, AsType>);
    return Impl<As<remove_alias_t<AliasType_>>, AsType>::as(value.value());
  }
};

template <typename AliasType, typename AsType>
struct Impl<As<AliasType,
  std::enable_if_t<
    is_alias_type_v<AsType> and
    not std::is_same_v<AliasType, AsType> and
    is_same_without_traits_v<AliasType, AsType>>>, AsType> {
  static AsType& as(AliasType& value) {
    return static_cast<AsType&>(static_cast<alias_type_base_of_t<AliasType>&>(value));
  }
};


}
}

#endif  // !MARK_ALIAS_TYPE_TRAITS_AS_HPP_

