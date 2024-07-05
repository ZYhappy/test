
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

#ifndef MARK_ALIAS_TYPE_ALIAS_TYPE_HPP_
#define MARK_ALIAS_TYPE_ALIAS_TYPE_HPP_

#include <functional>

#include "mark/alias_type/alias_type_base.hpp"
#include "mark/alias_type/traits/as.hpp"
#include "mark/alias_type/traits/into.hpp"
#include "mark/alias_type/traits/debug.hpp"
#include "mark/alias_type/traits/format.hpp"
#include "mark/alias_type/traits/equal.hpp"
#include "mark/alias_type/traits/order.hpp"
#include "mark/alias_type/traits/show.hpp"
#include "mark/alias_type/traits/convert.hpp"

#include "mark/macros.hpp"

namespace mark {

template <typename BaseType, auto TypeID,
  template <typename, typename = void> class... Traits>
using default_alias_type =
alias_type<BaseType, TypeID,
  traits::As, traits::Into, traits::Ord, traits::Convert,
  traits::Format, traits::Debug, traits::Show,
  Traits...>;

}  // namespace mark

#endif  // !MARK_ALIAS_TYPE_ALIAS_TYPE_HPP_
