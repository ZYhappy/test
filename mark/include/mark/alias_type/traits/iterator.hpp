
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/22/2020 20:39
//

#ifndef MARK_ALIAS_TYPE_TYPE_TRAITS_ITERATOR_HPP_
#define MARK_ALIAS_TYPE_TYPE_TRAITS_ITERATOR_HPP_

#include "mark/alias_type/traits/traits.hpp"

namespace mark::traits {

template <typename AliasType, typename = void>
struct Iterator : public Self<AliasType, Iterator> {

  using Self<AliasType, ::mark::traits::Iterator>::self;

  decltype(auto) next() & {
    return Impl<Iterator>::next(this->self());
  }
};

}  // namespace mark::traits

#endif  // !MARK_ALIAS_TYPE_TYPE_TRAITS_ITERATOR_HPP_
