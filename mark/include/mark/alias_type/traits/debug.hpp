
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/15/2020 21:46
//

#ifndef MARK_ALIAS_TYPE_TRAITS_DEBUG_HPP_
#define MARK_ALIAS_TYPE_TRAITS_DEBUG_HPP_

#include <string>

#include "mark/alias_type/traits/traits.hpp"

namespace mark {
namespace traits {

template <typename AliasType_, typename = void>
struct Debug : Self<AliasType_, Debug> {
  template<typename IntoType>
  using __impl_t = decltype(std::declval<IntoType &>() = Impl<Debug<AliasType_>, IntoType>::debug(std::declval<AliasType_>()));

  using Self<AliasType_, ::mark::traits::Debug>::Self;

  template <typename DebugResult = std::string, has_impl_t<__impl_t, DebugResult> = 0>
  DebugResult debug() const {
    return Impl<Debug, DebugResult>::debug(this->self());
  }
};

}
}

#endif  // !MARK_ALIAS_TYPE_TRAITS_DEBUG_HPP_
