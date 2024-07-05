
//
// Author: Zhengyu
// Date: 10/20/2020 09:22
//

#ifndef MARK_META_INDEX_SEQUENCE_HPP
#define MARK_META_INDEX_SEQUENCE_HPP

namespace mark {

//------------------------------------------------------------------------------

template<size_t... I>
struct index_sequence {};

template<size_t N, size_t ...Tail>
struct __index_sequence_builder
  : public __index_sequence_builder<N - 1, N - 1, Tail...> {};

template<size_t... I>
struct __index_sequence_builder<0, I...> {
  using type = index_sequence<I...>;
};

template <size_t N>
constexpr decltype(auto) make_index_sequence() {
  return typename __index_sequence_builder<N>::type{};
}

//------------------------------------------------------------------------------

template <typename Index, template<size_t> class F, template <typename...> class Container>
struct map_index_sequence;

template <size_t... I, template<size_t> class F, template <typename...> class Container>
struct map_index_sequence<index_sequence<I...>, F, Container> {
  typedef Container<typename F<I>::type...> type;
};

template <typename Index, typename Acc, template<typename, size_t> class F>
struct fold_index_sequence;

template <size_t Head, size_t... Tail, typename Acc, template<typename, size_t> class F>
struct fold_index_sequence<index_sequence<Head, Tail...>, Acc, F> {
  typedef typename fold_index_sequence<index_sequence<Tail...>, typename F<Acc, Head>::type, F>::type type;
};

template <typename Acc, template<typename, size_t> class F>
struct fold_index_sequence<index_sequence<>, Acc, F> {
  typedef Acc type;
};

//------------------------------------------------------------------------------


}

#endif  // !MARK_META_INDEX_SEQUENCE_HPP
