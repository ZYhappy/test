
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/15/2020 21:43
//

#ifndef MARK_ALIAS_TYPE_TRAITS_FORMAT_HPP_
#define MARK_ALIAS_TYPE_TRAITS_FORMAT_HPP_

#include <string>

#include "mark/alias_type/traits/traits.hpp"

namespace mark {
namespace traits {

template <typename AliasType, typename = void>
struct Format : Self<AliasType, Format> {

  template <typename FormatResult>
  using __impl_t = decltype(std::declval<FormatResult&>() = Impl<Format, FormatResult>::format(std::declval<const AliasType&>()));

  template <typename FormatResult = std::string, has_impl_t<__impl_t, FormatResult> = 0>
  FormatResult format() const {
    return Impl<Format, FormatResult>::format(this->self());
  }
};

}  // namespace traits
}  // namespace mark

#endif  // !MARK_ALIAS_TYPE_TRAITS_FORMAT_HPP_
