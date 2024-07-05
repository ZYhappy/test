
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/14/2020 6:48:54
//

#ifndef MARK_ALIAS_TYPE_ALIAS_TYPE_TRAIT_HPP_
#define MARK_ALIAS_TYPE_ALIAS_TYPE_TRAIT_HPP_

#include <tuple>
#include <type_traits>

#include "mark/alias_type/alias_type_define.hpp"
#include "mark/alias_type/alias_type_base.hpp"

namespace mark {

template <typename T>
struct alias_type_base_of;

template <typename T, auto TypeID, template<typename, typename = void> class... Traits>
struct alias_type_base_of<alias_type<T, TypeID, Traits...>> {
  using type = __alias_type_base<T, TypeID>;
};

template <typename T>
using alias_type_base_of_t = typename alias_type_base_of<T>::type;

template <typename T, template<typename, typename = void> class Trait>
struct has_trait;

template <typename T, auto TypeID, template<typename, typename = void> class Trait,
  template<typename, typename = void> class... TailTraits>
struct has_trait<alias_type<T, TypeID, Trait, TailTraits...>, Trait> : std::true_type {};

template <typename T, auto TypeID, template<typename, typename = void> class Trait,
  template<typename, typename = void> class FrontTraits,
  template<typename, typename = void> class... TailTraits>
struct has_trait<alias_type<T, TypeID, FrontTraits, TailTraits...>, Trait> {
  static constexpr bool value = has_trait<alias_type<T, TypeID, TailTraits...>, Trait>::value;
};

template <typename T, auto TypeID, template<typename, typename = void> class Trait>
struct has_trait<alias_type<T, TypeID>, Trait> : std::false_type {};

template <typename T, template<typename, typename = void> class Trait>
constexpr bool has_trait_v = has_trait<T, Trait>::value;

template <typename T, template<typename, typename = void> class Trait, typename = void>
struct append_trait;

template <typename T, template<typename, typename = void> class Trait>
struct append_trait<T, Trait, std::enable_if_t<has_trait_v<T, Trait>>> {
  using type = T;
};

template <typename T, auto TypeID, template<typename, typename = void> class Trait,
  template<typename, typename = void> class... TailTraits>
struct append_trait<alias_type<T, TypeID, TailTraits...>, Trait, std::enable_if_t<!has_trait_v<alias_type<T, TypeID, TailTraits...>, Trait>>> {
  using type = alias_type<T, TypeID, TailTraits..., Trait>;
};

template <typename T, template<typename, typename = void> class Trait, typename = void>
using append_trait_t = typename append_trait<T, Trait, void>::type;

template <typename AliasType>
struct remove_traits;

template <typename BaseType, auto TypeID,
  template <typename, typename = void> class... Traits>
struct remove_traits<alias_type<BaseType, TypeID, Traits...>> {
  typedef alias_type<BaseType, TypeID> type;
};

template <typename AliasType>
using remove_traits_t = typename remove_traits<AliasType>::type;

template <typename AliasType>
struct remove_alias;

template <typename BaseType, auto TypeID,
  template <typename, typename = void> class... Traits>
struct remove_alias<alias_type<BaseType, TypeID, Traits...>> {
  static constexpr auto type_id = TypeID;
  typedef BaseType type;
};

template <typename AliasType>
using remove_alias_t = typename remove_alias<AliasType>::type;

template <typename T>
struct remove_all_alias {
  typedef T type;
};

template <typename BaseType, auto TypeID,
  template <typename, typename = void> class... Traits>
struct remove_all_alias<alias_type<BaseType, TypeID, Traits...>> {
  typedef typename remove_all_alias<BaseType>::type type;
};

template <typename T>
using remove_all_alias_t = typename remove_all_alias<T>::type;

template <typename AliasType>
struct get_type_id;

template <typename BaseType, auto TypeID,
  template <typename, typename = void> class... Traits>
struct get_type_id<alias_type<BaseType, TypeID, Traits...>> {
  static constexpr auto value = TypeID;
};

template <typename AliasType>
constexpr auto get_type_id_v = get_type_id<AliasType>::value;

template <typename AliasType>
struct get_traits;

template <typename BaseType, auto TypeID,
  template <typename, typename = void> class... Traits>
struct get_traits<alias_type<BaseType, TypeID, Traits...>> {
  typedef std::tuple<Traits<alias_type<BaseType, TypeID, Traits...>>...> type;
};

template <typename AliasType>
using get_traits_t = typename get_traits<AliasType>::type;

template <typename AliasType>
struct is_alias_type : std::false_type {};

template <typename BaseType, auto TypeID,
  template <typename, typename = void> class... Traits>
struct is_alias_type<alias_type<BaseType, TypeID, Traits...>> : std::true_type {
};

template <typename AliasType>
constexpr bool is_alias_type_v = is_alias_type<AliasType>::value;

template <typename LeftAliasType, typename RightAliasType>
struct is_same_without_traits : std::false_type {};

template <typename BaseType, auto TypeID,
  template <typename, typename = void> class... LeftTraits,
  template <typename, typename = void> class... RightTraits>
struct is_same_without_traits<
  mark::alias_type<BaseType, TypeID, LeftTraits...>,
  mark::alias_type<BaseType, TypeID, RightTraits...>> : std::true_type {
};

template <typename LeftAliasType, typename RightAliasType>
constexpr bool is_same_without_traits_v =
  is_same_without_traits<LeftAliasType, RightAliasType>::value;

template <typename AliasType, typename BaseType>
struct is_base_type_of : std::false_type {};

template <typename BaseType, auto TypeID,
  template <typename, typename = void> class... Traits>
struct is_base_type_of<mark::alias_type<BaseType, TypeID, Traits...>,
BaseType> : std::true_type {};

template <typename AliasType, typename BaseType>
constexpr bool is_base_type_of_v = is_base_type_of<AliasType, BaseType>::value;

template <typename AliasType, typename BaseType, typename Ty = void>
struct enable_if_is_base_type_of
  : std::enable_if<is_base_type_of_v<AliasType, BaseType>, Ty> {};

template <typename AliasType, typename BaseType, typename Ty = void>
using enable_if_is_base_type_of_t =
typename enable_if_is_base_type_of<AliasType, BaseType, Ty>::type;

template <typename... Args>
struct first_argument;

template <typename Arg, typename... Args>
struct first_argument<Arg, Args...> {
  typedef Arg type;
};

template <typename... Args>
using first_argument_t = typename first_argument<Args...>::type;

template <typename, typename>
struct is_not_same : std::true_type {};

template <typename T>
struct is_not_same<T, T> : std::false_type {};

template <typename T, typename Tuple>
struct has_element_type;

template <typename This, typename... Rest>
struct has_element_type<This, std::tuple<This, Rest...>> : std::true_type {};

template <typename T, typename This, typename... Rest>
struct has_element_type<T, std::tuple<This, Rest...>>
  : has_element_type<T, std::tuple<Rest...>> {};

template <typename T>
struct has_element_type<T, std::tuple<>> : std::false_type {};

template <typename T, typename Tuple>
constexpr bool has_element_type_v = has_element_type<T, Tuple>::value;

template <template<typename, typename = void> class Trait, typename T>
constexpr auto add_traits_cast(T& value) -> std::add_lvalue_reference_t<append_trait_t<remove_cvref_t<T>, Trait>> {
  using __ReturnType = append_trait_t<remove_cvref_t<T>, Trait>;
  using __BaseType = alias_type_base_of_t<remove_cvref_t<T>>;
  return static_cast<__ReturnType&>(static_cast<__BaseType&>(value));
}

template <template<typename, typename = void> class Trait, typename T>
constexpr auto add_traits_cast(const T& value) -> std::add_lvalue_reference_t<std::add_const_t<append_trait_t<remove_cvref_t<T>, Trait>>> {
  using __ReturnType = std::add_lvalue_reference_t<std::add_const_t<append_trait_t<remove_cvref_t<T>, Trait>>>;
  using __BaseType = std::add_lvalue_reference_t<std::add_const_t<alias_type_base_of_t<remove_cvref_t<T>>>>;
  return static_cast<__ReturnType>(static_cast<__BaseType>(value));
}

}  // namespace mark

#endif  // !MARK_ALIAS_TYPE_ALIAS_TYPE_TRAIT_HPP_
