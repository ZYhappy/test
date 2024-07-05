
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 10/13/2020 09:06
//

#ifndef MARK_MACROS_HPP_
#define MARK_MACROS_HPP_

#define MARK_AND(a, b) MARK_CAT3(_MARK_AND_, a, b)
#define MARK_NOT(a) MARK_CAT2(_MARK_NOT_, a)
#define MARK_IF(a, b, ...) MARK_CAT2(_MARK_IF_, a)(b, __VA_ARGS__)

#define MARK_EMPTY()
#define MARK_DEFER(...) __VA_ARGS__ MARK_EMPTY()
#define MARK_OBSTRUCT(...) __VA_ARGS MARK_DEFER(EMPTY)()
#define MARK_EXPAND(...) __VA_ARGS__
#define MARK_HEAD(a, ...) a
#define MARK_TAIL(a, ...) __VA_ARGS__
#define MARK_PAIR(a, ...) a __VA_ARGS__

#define MARK_CAT2(a, ...) MARK_CAT2_IMPL(a, __VA_ARGS__)
#define MARK_CAT3(a, b, ...) MARK_CAT3_IMPL(a, b, __VA_ARGS__)

#define MARK_IS_EMPTY(...) \
  MARK_AND(MARK_IS_LIST(__VA_ARGS__ ()), MARK_NOT(MARK_IS_LIST(__VA_ARGS__ _)))
#define MARK_IS_LIST(...) \
  MARK_PAIR(MARK_HEAD, (MARK_CAT2(MARK_IS_LIST_RET_, MARK_IS_LIST_TST_ __VA_ARGS__)))

#define MARK_IS_LIST_TST_(...) 1
#define MARK_IS_LIST_RET_MARK_IS_LIST_TST_ 0,
#define MARK_IS_LIST_RET_1 1,

#define MARK_CAT2_IMPL(a, ...) MARK_EXPAND(a ## __VA_ARGS__)
#define MARK_CAT3_IMPL(a, b, ...) MARK_EXPAND(a ## b ## __VA_ARGS__)

#define _MARK_AND_00 0
#define _MARK_AND_01 0
#define _MARK_AND_10 0
#define _MARK_AND_11 1

#define _MARK_IF_0(a, ...) __VA_ARGS__
#define _MARK_IF_1(a, ...) a

#define _MARK_NOT_0 1
#define _MARK_NOT_1 0


//------------------------------------------------------------------------------

#define MARK_MACRO_CONCAT(pre, sub) MARK_MACRO_CONCAT_(pre, sub)
#define MARK_MACRO_CONCAT_(pre, sub) pre##_##sub

#define MARK_APPLY_VARIADIC_MACRO(macro,tuple) macro tuple

//------------------------------------------------------------------------------

#define MARK_REPEAT_WITH_0(s,m,...)
#define MARK_REPEAT_WITH_1(s,m,...)                                        m(0,__VA_ARGS__)
#define MARK_REPEAT_WITH_2(s,m,...)  MARK_REPEAT_WITH_1(s,m,__VA_ARGS__) s m(1,__VA_ARGS__)
#define MARK_REPEAT_WITH_3(s,m,...)  MARK_REPEAT_WITH_2(s,m,__VA_ARGS__) s m(2,__VA_ARGS__)
#define MARK_REPEAT_WITH_4(s,m,...)  MARK_REPEAT_WITH_3(s,m,__VA_ARGS__) s m(3,__VA_ARGS__)
#define MARK_REPEAT_WITH_5(s,m,...)  MARK_REPEAT_WITH_4(s,m,__VA_ARGS__) s m(4,__VA_ARGS__)
#define MARK_REPEAT_WITH_6(s,m,...)  MARK_REPEAT_WITH_5(s,m,__VA_ARGS__) s m(5,__VA_ARGS__)
#define MARK_REPEAT_WITH_7(s,m,...)  MARK_REPEAT_WITH_6(s,m,__VA_ARGS__) s m(6,__VA_ARGS__)
#define MARK_REPEAT_WITH_8(s,m,...)  MARK_REPEAT_WITH_7(s,m,__VA_ARGS__) s m(7,__VA_ARGS__)
#define MARK_REPEAT_WITH_9(s,m,...)  MARK_REPEAT_WITH_8(s,m,__VA_ARGS__) s m(8,__VA_ARGS__)
#define MARK_REPEAT_WITH_10(s,m,...) MARK_REPEAT_WITH_9(s,m,__VA_ARGS__) s m(9,__VA_ARGS__)

#define MARK_REPEAT_WITH(s,n,m,...) MARK_REPEAT_WITH_ ## n(s,m,__VA_ARGS__)
#define MARK_REPEAT(n,m,...)        MARK_REPEAT_WITH(,n,m,__VA_ARGS__)

//------------------------------------------------------------------------------

#define MARK_ENUM_0(m,...)
#define MARK_ENUM_1(m,...)                              m(0,__VA_ARGS__)
#define MARK_ENUM_2(m,...) MARK_ENUM_1(m,__VA_ARGS__) , m(1,__VA_ARGS__)
#define MARK_ENUM_3(m,...) MARK_ENUM_2(m,__VA_ARGS__) , m(2,__VA_ARGS__)
#define MARK_ENUM_4(m,...) MARK_ENUM_3(m,__VA_ARGS__) , m(3,__VA_ARGS__)
#define MARK_ENUM_5(m,...) MARK_ENUM_4(m,__VA_ARGS__) , m(4,__VA_ARGS__)
#define MARK_ENUM_6(m,...) MARK_ENUM_5(m,__VA_ARGS__) , m(5,__VA_ARGS__)
#define MARK_ENUM_7(m,...) MARK_ENUM_6(m,__VA_ARGS__) , m(6,__VA_ARGS__)
#define MARK_ENUM_8(m,...) MARK_ENUM_7(m,__VA_ARGS__) , m(7,__VA_ARGS__)
#define MARK_ENUM_9(m,...) MARK_ENUM_8(m,__VA_ARGS__) , m(8,__VA_ARGS__)

#define MARK_ENUM(n,m,...) MARK_ENUM_ ## n(m,__VA_ARGS__)

//------------------------------------------------------------------------------

#define MARK_SELECT_ARG_0(a0, ...) a0
#define MARK_SELECT_ARG_1(a0,a1, ...) a1
#define MARK_SELECT_ARG_2(a0,a1,a2, ...) a2
#define MARK_SELECT_ARG_3(a0,a1,a2,a3, ...) a3
#define MARK_SELECT_ARG_4(a0,a1,a2,a3,a4, ...) a4
#define MARK_SELECT_ARG_5(a0,a1,a2,a3,a4,a5, ...) a5
#define MARK_SELECT_ARG_6(a0,a1,a2,a3,a4,a5,a6, ...) a6
#define MARK_SELECT_ARG_7(a0,a1,a2,a3,a4,a5,a6,a7, ...) a7
#define MARK_SELECT_ARG_8(a0,a1,a2,a3,a4,a5,a6,a7,a8, ...) a8
#define MARK_SELECT_ARG_9(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9, ...) a9

#define MARK_SELECT_ARG(i, ...) MARK_APPLY_VARIADIC_MACRO(MARK_SELECT_ARG_ ## i,(__VA_ARGS__,MARK_EMPTY()))

//------------------------------------------------------------------------------

#define MARK_SELECT_RST_0(a0, ...) __VA_ARGS__
#define MARK_SELECT_RST_1(a0,a1, ...) __VA_ARGS__
#define MARK_SELECT_RST_2(a0,a1,a2, ...) __VA_ARGS__
#define MARK_SELECT_RST_3(a0,a1,a2,a3, ...) __VA_ARGS__
#define MARK_SELECT_RST_4(a0,a1,a2,a3,a4, ...) __VA_ARGS__
#define MARK_SELECT_RST_5(a0,a1,a2,a3,a4,a5, ...) __VA_ARGS__
#define MARK_SELECT_RST_6(a0,a1,a2,a3,a4,a5,a6, ...) __VA_ARGS__
#define MARK_SELECT_RST_7(a0,a1,a2,a3,a4,a5,a6,a7, ...) __VA_ARGS__
#define MARK_SELECT_RST_8(a0,a1,a2,a3,a4,a5,a6,a7,a8, ...) __VA_ARGS__
#define MARK_SELECT_RST_9(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9, ...) __VA_ARGS__

#define MARK_SELECT_RST(i, ...) MARK_APPLY_VARIADIC_MACRO(MARK_SELECT_RST_ ## i,(__VA_ARGS__))

//------------------------------------------------------------------------------

#define MARK_PREV_0  00
#define MARK_PREV_1  0
#define MARK_PREV_2  1
#define MARK_PREV_3  2
#define MARK_PREV_4  3
#define MARK_PREV_5  4
#define MARK_PREV_6  5
#define MARK_PREV_7  6
#define MARK_PREV_8  7
#define MARK_PREV_9  8
#define MARK_PREV_10 9

#define MARK_PREV(i) MARK_PREV_##i

//------------------------------------------------------------------------------

#define MARK_SUCC_0 1
#define MARK_SUCC_1 2
#define MARK_SUCC_2 3
#define MARK_SUCC_3 4
#define MARK_SUCC_4 5
#define MARK_SUCC_5 6
#define MARK_SUCC_6 7
#define MARK_SUCC_7 8
#define MARK_SUCC_8 9
#define MARK_SUCC_9 10
#define MARK_SUCC_10 PLEASE_EXTEND_MARK_SUCC

#define MARK_SUCC(i) MARK_SUCC_##i

#endif  // !MARK_MACROS_HPP_
