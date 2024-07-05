
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/16/2020 13:28
//

#ifndef MARK_ALIAS_TYPE_TRAITS_ORDER_HPP_
#define MARK_ALIAS_TYPE_TRAITS_ORDER_HPP_

#include "mark/alias_type/traits/traits.hpp"
#include "mark/utility.hpp"
#include "mark/alias_type/alias_type_trait.hpp"

namespace mark::traits {

template <typename AliasType, typename = void>
struct Ord : public Eq<AliasType, void> {

  template <typename RightType>
  using __impl_t = decltype(Impl<Ord, RightType>::less(std::declval<const AliasType&>(), std::declval<const RightType&>()));

  friend bool operator<(const AliasType& left, const AliasType& right) {
    return Impl<Ord, AliasType>::less(left, right);
  }

  template <typename RightType, has_impl_t<__impl_t, RightType> = 0>
  friend bool operator<(const AliasType& left, const RightType& right) {
    static_assert(has_impl_v<__impl_t, remove_cvref_t<RightType>>, "there are no Ord trait for [RightType]");
    return Impl<Ord, RightType>::less(left, right);
  }

  template <typename LeftType, has_impl_t<__impl_t, LeftType> = 0>
  friend bool operator<(const LeftType& left, const AliasType& right) {
    static_assert(has_impl_v<__impl_t, remove_cvref_t<LeftType>>, "there are no Ord trait for [LeftType]");
    return !Impl<Ord, LeftType>::less(right, left);
  }

  friend bool operator<=(const AliasType& left, const AliasType& right) {
    return (left < right) || (left == right);
  }

  template <typename RightType, has_impl_t<__impl_t, RightType> = 0>
  friend constexpr bool operator<=(const AliasType& left, RightType right) {
    return (left < right) || (left == right);
  }

  template <typename LeftType, has_impl_t<__impl_t, LeftType> = 0>
  friend constexpr bool operator<=(LeftType left, const AliasType& right) {
    return right >= left;
  }

  friend constexpr bool operator>=(const AliasType& left, const AliasType& right) {
    return !(left < right);
  }

  template <typename RightType, has_impl_t<__impl_t, RightType> = 0>
  friend constexpr bool operator>=(const AliasType& left, RightType right) {
    return !(left < right);
  }

  template <typename LeftType, has_impl_t<__impl_t, LeftType> = 0>
  friend constexpr bool operator>=(LeftType left, const AliasType& right) {
    return right <= left;
  }

  friend constexpr bool operator>(const AliasType& left, const AliasType& right) {
    return !(left <= right);
  }

  template <typename RightType, has_impl_t<__impl_t, RightType> = 0>
  friend constexpr bool operator>(const AliasType& left, RightType right) {
    return !(left <= right);
  }

  template <typename LeftType, has_impl_t<__impl_t, LeftType> = 0>
  friend constexpr bool operator>(LeftType left, const AliasType& right) {
    return right < left;
  }
};

template <typename AliasType, typename RightType>
struct Impl<
  Ord<AliasType,
  std::enable_if_t<
    std::is_same_v<AliasType, remove_cvref_t<RightType>>>>, RightType> {
static bool less(const AliasType& left, const RightType& right) {
  return left.value() < right.value();
}
};

template <typename AliasType, typename RightType>
struct Impl<
  Ord<AliasType,
  std::enable_if_t<
    not std::is_same_v<AliasType, remove_cvref_t<RightType>> and
    std::is_convertible_v<RightType, AliasType>>>, RightType> {
static bool less(const AliasType& left, const RightType& right) {
  return left.value() < right;
}
};

}  // namespace mark::traits

#endif  // !MARK_ALIAS_TYPE_TRAITS_ORDER_HPP_
