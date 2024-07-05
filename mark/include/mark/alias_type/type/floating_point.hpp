
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

#ifndef MARK_ALIAS_TYPE_TYPE_FLOATING_POINT_HPP_
#define MARK_ALIAS_TYPE_TYPE_FLOATING_POINT_HPP_

#include "mark/alias_type/alias_type.hpp"

namespace mark {

template <auto TypeID, template <typename, typename = void> class... Traits>
using float_alias_type = default_alias_type<float, TypeID, Traits...>;
template <auto TypeID, template <typename, typename = void> class... Traits>
using double_alias_type = default_alias_type<double, TypeID, Traits...>;

}

#endif  // !MARK_ALIAS_TYPE_TYPE_FLOATING_POINT_HPP_
