
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

#ifndef MARK_ALIAS_TYPE_ALIAS_TYPE_DEFINE_HPP_
#define MARK_ALIAS_TYPE_ALIAS_TYPE_DEFINE_HPP_

namespace mark {

template <typename BaseType, auto TypeID,
  template <typename...> class... Traits>
class alias_type;


}  // namespace mark

#endif  // !MARK_ALIAS_TYPE_ALIAS_TYPE_DEFINE_HPP_
