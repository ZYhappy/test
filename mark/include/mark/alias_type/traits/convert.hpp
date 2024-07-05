#ifndef MARK_ALIAS_TYPE_TRAITS_CONVERT_HPP_
#define MARK_ALIAS_TYPE_TRAITS_CONVERT_HPP_

#include "mark/alias_type/traits/traits.hpp"
#include "mark/alias_type/alias_type_trait.hpp"
#include "mark/utility.hpp"

namespace mark {
namespace traits {

template <typename _Ty, typename _T>
using convert_to_t = decltype(std::declval<_Ty&>() = static_cast<_Ty&>(std::declval<_T>()));

template <typename AliasType, typename = void>
struct Convert : public Self<AliasType, Convert> {
  constexpr operator remove_alias_t<AliasType>&() {
    return this->self().value();
  }

  constexpr operator const remove_alias_t<AliasType>&() const {
    return this->self().value();
  }

  template <typename U,
    std::enable_if_t<
      !std::is_same_v<remove_alias_t<AliasType>, U> &&
      is_detected<convert_to_t, U, remove_alias_t<AliasType>>::value,
      int
    > = 0>
  constexpr operator U&() {
    return this->self().value();
  }
};

}  // namespace traits
}  // namespace rich_type

#endif  // !MARK_ALIAS_TYPE_TRAITS_CONVERT_HPP_
