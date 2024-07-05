
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|

///
/// \file
///
/// 一些对tuple或形如tuple的模板类的元函数
///
/// \author Zhengyu
/// \date 10/17/2020 14:57
///

#ifndef MARK_META_TUPLE_HPP_
#define MARK_META_TUPLE_HPP_

#include <tuple>
#include "mark/utility.hpp"
#include "mark/meta/index_sequence.hpp"

namespace mark {

namespace detail {

template <typename T, typename Tuple, std::size_t... I>
constexpr T make_from_tuple_impl(Tuple&& t, mark::index_sequence<I...>) {
  return T(std::get<I>(std::forward<Tuple>(t))...);
}

}

template <typename T, typename Tuple>
constexpr T make_from_tuple(Tuple&& t) {
  return detail::make_from_tuple_impl(std::forward<Tuple>(t), mark::make_index_sequence<std::tuple_size_v<Tuple>>());
}

//------------------------------------------------------------------------------

template <template<typename, typename> class Op, typename... Ts>
struct _invoke;

template <template<typename, typename> class Op, typename T1, typename T2>
struct _invoke<Op, T1, T2> :std::conditional_t<Op<T1, T2>::value, std::true_type, std::false_type>  {};

template <template<typename, typename> class Op, typename T1, typename T2, typename... Ts>
struct _invoke<Op, T1, T2, Ts...> : _invoke<Op, T2, Ts...> {};

//------------------------------------------------------------------------------

template <typename... Ts>
struct parameter_package_traits;

template <typename Head, typename... Tail>
struct parameter_package_traits<Head, Tail...> {
  typedef Head head_type;
  typedef std::tuple<Tail...> tail_tuple_type;
  enum {
    size = sizeof...(Tail) + 1
  };
};

//------------------------------------------------------------------------------

template <size_t N, typename... Tuple>
decltype(auto) zip_tuple_element(Tuple&&... tuple) {
  return std::make_tuple(std::get<N>(std::forward<Tuple>(tuple))...);
}

template <size_t... Index, typename... Tuple>
decltype(auto) zip_tuple_impl( mark::index_sequence<Index...>, Tuple&&... tuple) {
  return std::make_tuple(zip_tuple_element<Index>(std::forward<Tuple>(tuple)...)...);
}

template <typename... Tuple>
decltype(auto) zip_tuple(Tuple&&... tuple) {
}

//------------------------------------------------------------------------------

template <typename Fn, typename... Tuple, size_t... Index>
decltype(auto) __map_tuple_impl(Fn&& f, index_sequence<Index...>, Tuple&&... tuple) {
  return std::make_tuple(
    std::apply(std::forward<Fn>(f), zip_tuple_element<Index>(std::forward<Tuple>(tuple)...))...);
}

template <typename Fn, typename... Tuple>
decltype(auto) map_tuple(Fn&& f, Tuple&&... tuples) {
  static_assert(sizeof...(tuples) > 0);
  return __map_tuple_impl(std::forward<Fn>(f),
                          make_index_sequence<std::tuple_size_v<remove_cvref_t<typename parameter_package_traits<Tuple...>::head_type>>>(),
                          std::forward<Tuple>(tuples)...);
}

namespace meta {

/**
 * \brief
 *
 * \tparam Tuple
 * \tparam F
 * \tparam Container
 */
template <typename Tuple, template<typename> class F, template <typename...> class Container = std::tuple>
struct map_tuple;

template <typename... Args, template<typename...> class Tuple, template<typename> class F, template <typename...> class Container>
struct map_tuple<Tuple<Args...>, F, Container> {
  typedef Container<typename F<Args>::type...> type;
};

}


//------------------------------------------------------------------------------

template <typename Tuple, typename Acc, template<typename, typename> class F>
struct fold_tuple;

template <typename Head, typename... Tail, template <typename...> class Tuple, typename Acc, template<typename, typename> class F>
struct fold_tuple<Tuple<Head, Tail...>, Acc, F> {
  typedef typename fold_tuple<Tuple<Tail...>, typename F<Acc, Head>::type, F>::type type;
};

template <template <typename...> class Tuple, typename Acc, template<typename, typename> class F>
struct fold_tuple<Tuple<>, Acc, F> {
  typedef Acc type;
};

template <typename Tuple, typename... Tuples>
struct tuple_cat;

template <template<typename...> class Tuple, typename... Args>
struct tuple_cat<Tuple<Args...>> {
  typedef Tuple<Args...> type;
};

template <template<typename...> class Tuple, typename... Args1, typename... Args2, typename... Others>
struct tuple_cat<Tuple<Args1...>, Tuple<Args2...>, Others...> {
  typedef typename tuple_cat<Tuple<Args1..., Args2...>, Others...>::type type;
};

template <typename Tuple, typename T>
struct find;

template <typename Head, typename... Tail, template <typename...> class Tuple, typename T>
struct find<Tuple<Head, Tail...>, T> {
  static constexpr size_t index = find<Tuple<Tail...>, T>::index + 1;
};

template <typename Head, typename... Tail, template <typename...> class Tuple>
struct find<Tuple<Head, Tail...>, Head> {
  static constexpr size_t index = 0;
};

template <typename Tuple, typename T>
struct is_element_of;

template <typename Head, typename... Tail, template <typename...> class Tuple, typename T>
struct is_element_of<Tuple<Head, Tail...>, T> {
  static constexpr bool value = is_element_of<Tuple<Tail...>, T>::value;
};

template <typename Head, typename... Tail, template <typename...> class Tuple>
struct is_element_of<Tuple<Head, Tail...>, Head> {
  static constexpr bool value = true;
};

template <template <typename...> class Tuple, typename T>
struct is_element_of<Tuple<>, T> {
  static constexpr bool value = false;
};

//------------------------------------------------------------------------------

/**
 * \brief 将tuple内数据类型重复N份
 * \tparam N 复制份数
 * \tparam Tuple 输入元组或是符合 `template<typename...> class Tuple` 类型的模板类实例
 *
 * \code{.cpp}
 * typedef std::tuple<int, double> src_tuple_t;
 * // dst_tuple -> std::tuple<int, double, int, double, int, double>
 * typedef mark::repeat_tuple<3, src_tuple_t>::type dst_tuple_t;
 * \endcode
 *
 * \see repeat_tuple_0_Tuple
 */
template <size_t N, typename Tuple>
struct repeat_tuple;

/**
 * \brief repeat_tuple 在N为0时特化
 * \tparam Args
 *
 * \sa repeat_tuple_0_Tuple
 */
template <template<typename...> class Tuple, typename... Args>
struct repeat_tuple<0, Tuple<Args...>> {
  typedef Tuple<> type;
};

template <size_t N, template<typename...> class Tuple, typename... Args>
struct repeat_tuple<N, Tuple<Args...>> {
  typedef typename tuple_cat<Tuple<Args...>, typename repeat_tuple<N-1, Tuple<Args...>>::type>::type type;
};

//------------------------------------------------------------------------------

template <typename STuple, template <typename...> class DTuple>
struct transfer_tuple;

template <typename... Args, template <typename...> class STuple, template <typename...> class DTuple>
struct transfer_tuple<STuple<Args...>, DTuple> {
  typedef DTuple<Args...> type;
};

//------------------------------------------------------------------------------

}

#endif  // !MARK_META_TUPLE_HPP_

