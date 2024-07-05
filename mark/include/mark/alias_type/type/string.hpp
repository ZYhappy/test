
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

#ifndef MARK_ALIAS_TYPE_TYPE_STRING_HPP_
#define MARK_ALIAS_TYPE_TYPE_STRING_HPP_

#include <string>
#include <string_view>

#include "mark/alias_type/traits/traits.hpp"
#include "mark/alias_type/alias_type.hpp"
#include "mark/alias_type/alias_type_trait.hpp"

namespace mark {

namespace traits {

template <typename AliasType, typename = void>
struct StrImpl;

}  // namespace traits

template <auto TypeID, template <typename, typename = void> class... Traits>
using char_alias_type = default_alias_type<char, TypeID, Traits...>;


template <typename T, auto TypeID, template <typename, typename = void> class... Traits>
using basic_string_alias_type = default_alias_type<std::basic_string<T, std::char_traits<T>, std::allocator<T> >, TypeID, traits::StrImpl, Traits...>;

template <auto TypeID, template <typename, typename = void> class... Traits>
using string_alias_type = basic_string_alias_type<char, TypeID, Traits...>;

template <auto TypeID, template <typename, typename = void> class... Traits>
using wstring_alias_type = basic_string_alias_type<wchar_t, TypeID, Traits...>;

namespace traits {

template <typename T, auto TypeID, template<typename, typename = void> class... Traits>
struct StrImpl<basic_string_alias_type<T, TypeID, Traits...>>
  : Self<basic_string_alias_type<T, TypeID, Traits...>, StrImpl> {
  using Self<basic_string_alias_type<T, TypeID, Traits...>, ::mark::traits::StrImpl>::self;

  auto c_str() const
  -> decltype(std::declval<remove_alias_t<basic_string_alias_type<T, TypeID, Traits...>>>().c_str()) {
    return this->self().value().c_str();
  }

  bool empty() const {
    return this->self().value().empty();
  }

  size_t size() const {
    return this->self().value().size();
  }

  void shrink_to_fit() {
    this->self().value().shrink_to_fit();
  }

  void reserve(const size_t new_cap = 0) {
    this->self().value().reserve(new_cap);
  }
};

}  // namespace traits

namespace traits {

template <typename AliasType, typename = void>
struct StrViewImpl;

}  // namespace traits

template <typename T, auto TypeID, template <typename, typename = void> class... Traits>
using basic_string_view_alias_type = default_alias_type<std::basic_string_view<T, std::char_traits<T>>, TypeID, traits::StrViewImpl, Traits...>;

template <auto TypeID, template <typename, typename = void> class... Traits>
using string_view_alias_type = basic_string_view_alias_type<char, TypeID, Traits...>;

template <auto TypeID, template <typename, typename = void> class... Traits>
using wstring_view_alias_type = basic_string_view_alias_type<wchar_t, TypeID, Traits...>;

namespace traits {

template <typename T, auto TypeID, template<typename, typename = void> class... Traits>
struct StrViewImpl<basic_string_view_alias_type<T, TypeID, Traits...>>
  : Self<basic_string_view_alias_type<T, TypeID, Traits...>, StrViewImpl> {
  using Self<basic_string_view_alias_type<T, TypeID, Traits...>, ::mark::traits::StrViewImpl>::self;

  constexpr auto data() const
  -> decltype(std::declval<remove_alias_t<basic_string_view_alias_type<T, TypeID, Traits...>>>().data()) {
    return this->self().value().data();
  }

  constexpr bool empty() const {
    return this->self().value().empty();
  }

  constexpr size_t size() const {
    return this->self().value().size();
  }

  constexpr auto begin() -> decltype(std::declval<remove_alias_t<basic_string_view_alias_type<T, TypeID, Traits...>>>().begin()) {
    return this->self().value().begin();
  }

  constexpr auto cbegin() const -> decltype(std::declval<remove_alias_t<basic_string_view_alias_type<T, TypeID, Traits...>>>().cbegin()) {
    return this->self().value().cbegin();
  }

  constexpr auto end() -> decltype(std::declval<remove_alias_t<basic_string_view_alias_type<T, TypeID, Traits...>>>().end()) {
    return this->self().value().end();
  }

  constexpr auto cend() -> decltype(std::declval<remove_alias_t<basic_string_view_alias_type<T, TypeID, Traits...>>>().cend()) {
    return this->self().value().cend();
  }
};

}  // namespace traits

}  // namespace mark

namespace std {
  template <typename T, auto TypeID, template <typename, typename = void> class... Traits>
  struct hash<mark::basic_string_alias_type<T, TypeID, Traits...>> {
    typedef mark::basic_string_alias_type<T, TypeID, Traits...> argument_type;
    typedef std::size_t result_type;
    result_type operator() (const argument_type &s) const {
      return std::hash<std::basic_string<T, std::char_traits<T>, std::allocator<T> >>{}(s.value());
    }
  };

  template <typename T, auto TypeID, template <typename, typename = void> class... Traits>
  struct hash<mark::basic_string_view_alias_type<T, TypeID, Traits...>> {
    typedef mark::basic_string_view_alias_type<T, TypeID, Traits...> argument_type;
    typedef std::size_t result_type;
    result_type operator() (const argument_type &s) const {
      return std::hash<std::basic_string_view<T, std::char_traits<T>>>{}(s.value());
    }
  };
}

#endif  // !MARK_ALIAS_TYPE_TYPE_STRING_HPP_

