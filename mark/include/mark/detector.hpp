
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 07/06/2020 6:48:54
//

#ifndef MARK_DETECTOR_HPP_
#define MARK_DETECTOR_HPP_

#include <type_traits>

// http://en.cppreference.com/w/cpp/experimental/is_detected
//
// Version 2 of the C++ Extensions for Library Fundamentals
// C++ 标准库扩展，版本2
//
// 检测 template-id 是否良式的别名模板组

namespace mark {

// detected_t 在失败时返回的类类型
struct nonesuch {
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
};

template <class Default,
          class AlwaysVoid,
          template <class...> class Op,
          class... Args>
struct detector {
    using value_t = std::false_type;
    using type = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
    using value_t = std::true_type;
    using type = Op<Args...>;
};

template <template <class...> class Op, class... Args>
using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

template <class Default, template <class...> class Op, class... Args>
using detected_or = detector<Default, void, Op, Args...>;

template <class Default, template <class...> class Op, class... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

template <class Expected, template <class...> class Op, class... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

template <class To, template <class...> class Op, class... Args>
using is_detected_convertible =
    std::is_convertible<detected_t<Op, Args...>, To>;

}  // namespace mark


#endif // !MARK_DETECTOR_HPP_
