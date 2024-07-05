
//
// Author: Zhengyu
// Date: 10/17/2020 14:57
//

#ifndef MARK_META_FUNCTIONAL_HPP
#define MARK_META_FUNCTIONAL_HPP

#include <type_traits>

#include "mark/utility.hpp"

namespace mark {

template <typename T, bool = std::is_function_v<T>>
struct __is_closure_base;

template <typename T>
struct __is_closure_base<T, true> : std::true_type {};

template <typename T>
struct __is_closure_base<T, false> : std::false_type {};

template <typename T>
struct is_closure : __is_closure_base<remove_cvref_t<T>> {};

}

#endif  // !MARK_META_FUNCTIONAL_HPP
