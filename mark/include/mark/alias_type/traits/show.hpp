#ifndef MARK_ALIAS_TYPE_TRAITS_SHOW_HPP_
#define MARK_ALIAS_TYPE_TRAITS_SHOW_HPP_

#include <ostream>
#include "mark/alias_type/traits/traits.hpp"

namespace mark::traits {

template <typename AliasType, typename = void>
struct Show : Self<AliasType, Show> {
  template <typename OStream>
  using __impl_t = decltype(Impl<Show, OStream>::show(std::declval<OStream&>(), std::declval<const AliasType&>()));

  template <typename OStream = std::ostream , has_impl_t<__impl_t, OStream> = 0>
  OStream& operator<<(OStream& out) const {
    return Impl<Show, OStream>::show(out, this->self());
  }
};

template <typename OStream, typename BaseType>
using _member_show_function_t = decltype(std::declval<OStream&>().operator<<(std::declval<const BaseType&>()));

template <typename OStream, typename BaseType>
using _friend_show_function_t = decltype(operator<<(std::declval<OStream&>(), std::declval<const BaseType&>()));

template <typename AliasType, typename OStream>
struct Impl<Show<AliasType,
  std::enable_if_t<
    has_impl_v<_member_show_function_t, OStream, remove_alias_t<AliasType>> or
    has_impl_v<_friend_show_function_t, OStream, remove_alias_t<AliasType>>>>, OStream> {
  static OStream& show(OStream& out, const AliasType& self) {
    out << self.value();
    return out;
  }
};

}  // namespace mark::traits


#endif  // !MARK_ALIAS_TYPE_TRAITS_SHOW_HPP_
