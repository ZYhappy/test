
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

#ifndef MARK_ALIAS_TYPE_TYPE_INTEGER_HPP_
#define MARK_ALIAS_TYPE_TYPE_INTEGER_HPP_

#include <cstdint>
#include "mark/alias_type/alias_type.hpp"

namespace mark {

template <auto TypeID, template <typename, typename = void> class... Traits>
using int8_alias_type = default_alias_type<int8_t, TypeID, Traits...>;
template <auto TypeID, template <typename, typename = void> class... Traits>
using uint8_alias_type = default_alias_type<uint8_t, TypeID, Traits...>;

template <auto TypeID, template <typename, typename = void> class... Traits>
using int16_alias_type = default_alias_type<int16_t, TypeID, Traits...>;
template <auto TypeID, template <typename, typename = void> class... Traits>
using uint16_alias_type = default_alias_type<uint16_t, TypeID, Traits...>;

template <auto TypeID, template <typename, typename = void> class... Traits>
using int32_alias_type = default_alias_type<int32_t, TypeID, Traits...>;
template <auto TypeID, template <typename, typename = void> class... Traits>
using uint32_alias_type = default_alias_type<uint32_t, TypeID, Traits...>;

template <auto TypeID, template <typename, typename = void> class... Traits>
using int64_alias_type = default_alias_type<int64_t, TypeID, Traits...>;
template <auto TypeID, template <typename, typename = void> class... Traits>
using uint64_alias_type = default_alias_type<uint64_t, TypeID, Traits...>;

}

#endif  // !MARK_ALIAS_TYPE_TYPE_INTEGER_HPP_
