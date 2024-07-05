
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/23/2020 07:52
//

#ifndef MARK_ALIAS_TYPE_TYPE_CONTAINER_ITER_HPP_
#define MARK_ALIAS_TYPE_TYPE_CONTAINER_ITER_HPP_

#include "mark/alias_type/traits/traits.hpp"
#include "mark/alias_type/traits/iterator.hpp"
#include "mark/optional/optional.hpp"

namespace mark {

template <typename T>
struct Iter : public traits::Iterator<Iter<T>> {
  T ptr;
  const T end;
};

namespace traits {

template <typename T>
struct Iterator<Iter<T>>
  : public Self<Iter<T>, Iterator> {

  using __raw_iterator_traits = std::iterator_traits<T>;
  using __raw_value_type = typename __raw_iterator_traits::value_type;

  using Self<Iter<T>, ::mark::traits::Iterator>::self;

  optional<std::reference_wrapper<__raw_value_type>> next() {
    if (self().ptr != self().end) {
      return std::ref(*(self().ptr++));
    }
    return nullopt;
  }

  optional<std::reference_wrapper<__raw_value_type>> last() {
    using __opt_t = mark::optional<std::reference_wrapper<__raw_value_type>>;
    auto some = [] (__opt_t, __raw_value_type & x)
      -> __opt_t {
      return std::ref(x);
    };

    return this->fold(__opt_t{}, some);
  }

  template <typename B, typename F>
  B fold(B init, F f) {
    auto _f = [&](B acc, __raw_value_type& x) {
      return f(acc, x);
    };
    return try_fold(init, _f).value();
  }

  template <typename B, typename F>
  optional<B> try_fold(B init, F f) {
    auto accm = init;
    auto x = this->next();
    while (x.has_value()) {
      accm = f(accm, x.value());
      x = this->next();
    }
    return accm;
  }
};

}

}

#endif  // !MARK_ALIAS_TYPE_TYPE_CONTAINER_ITER_HPP_
