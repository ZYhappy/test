
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/16/2020 08:50
//

#ifndef MARK_ALIAS_TYPE_TRAITS_EQUAL_HPP_
#define MARK_ALIAS_TYPE_TRAITS_EQUAL_HPP_

#include "mark/alias_type/traits/traits.hpp"

namespace mark::traits {

template <typename AliasType, typename = void>
struct Eq : public Self<AliasType, Eq> {
  template <typename RightType>
  using __impl_t = decltype(Impl<Eq, RightType>::equal(std::declval<const AliasType&>(), std::declval<const RightType&>()));

  friend bool operator==(const AliasType& left, const AliasType& right) {
    return left.value() == right.value();
  }

  template <typename RightType, has_impl_t<__impl_t, RightType> = 0>
  friend bool operator==(const AliasType& left, const RightType& right) {
    static_assert(has_impl_v<__impl_t, RightType>, "there are no Eq trait for [RightType]");
    return Impl<Eq, RightType>::equal(left, right);
  }

  template <typename LeftType, has_impl_t<__impl_t, LeftType> = 0>
  friend bool operator==(const LeftType& left, const AliasType& right) {
    static_assert(has_impl_v<__impl_t, LeftType>, "there are no Eq trait for [RightType]");
    return Impl<Eq, LeftType>::equal(right, left);
  }

  friend bool operator!=(const AliasType& left, const AliasType& right) {
    static_assert(has_impl_v<__impl_t, AliasType>, "there are no Eq trait for [RightType]");
    return !Impl<Eq, AliasType>::equal(left, right);
  }

  template <typename RightType, has_impl_t<__impl_t, RightType> = 0>
  friend bool operator!=(const AliasType& left, const RightType& right) {
    return !(left == right);
  }

  template <typename LeftType, has_impl_t<__impl_t, LeftType> = 0>
  friend bool operator!=(const LeftType& left, const AliasType& right) {
    return !(right == left);
  }
};

template <typename AliasType, typename RightType>
struct Impl<
  Eq<AliasType,
    std::enable_if_t<std::is_same_v<AliasType, RightType>>>, RightType> {
  static bool equal(const AliasType& left, const RightType& right) {
    return left.value() == right.value();
  }
};

template <typename AliasType, typename RightType>
struct Impl<
  Eq<AliasType,
    std::enable_if_t<
      not std::is_same_v<AliasType, RightType> and
      std::is_convertible_v<RightType, AliasType>>>, RightType> {
  static bool equal(const AliasType& left, const RightType& right) {
    return left.value() == right;
  }
};

}

#endif  // !MARK_ALIAS_TYPE_TRAITS_EQUAL_HPP_
