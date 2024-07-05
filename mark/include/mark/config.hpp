
//
// Author: Zhengyu
// Date: 10/13/2020 14:47
//

#ifndef MARK_CONFIG_HPP_
#define MARK_CONFIG_HPP_

//------------------------------------------------------------------------------

#define HAS_CXX_11
#define HAS_CXX_14
#define HAS_CXX_17

#ifdef HAS_CXX_11
#  define __MARK_STD_CXX_11 std
#else
#  define __MARK_STD_CXX_11
#endif  // HAS_CXX_11

#ifdef HAS_CXX_14
#  define __MARK_STD_CXX_14 std
#else
#  define __MARK_STD_CXX_14
#endif  // HAS_CXX_14

#ifdef HAS_CXX_17
#  define __MARK_STD_CXX_17 std
#else
#  define __MARK_STD_CXX_17
#endif  // HAS_CXX_17

#ifndef _MARK_ASSERT
#  define _MARK_ASSERT(x, m) ((void)0)
#endif

//------------------------------------------------------------------------------

#if !defined (__MARK_WIN) && (defined(WIN32) || defined (_WIN32) || defined(_WIN64) || defined(_WINDOWS) || defined (_MSC_VER))
#  define __MARK_WIN
#endif

#if !defined (__MARK_LINUX) && (defined(__linux__) || defined(__linux) || defined(linux))
#  define __MARK_LINUX
#endif

#if !defined (__MARK_CLANG) && defined (__clang__)
#  define __MARK_CLANG
#endif

#if !defined (__MARK_MSVC) && (defined (_MSC_VER) && !defined (__MARK_CLANG))
#  define __MARK_MSVC
#endif

#if !defined (__MARK_GCC) && defined(__GNUC__)
#  define __MARK_GCC
#endif

//------------------------------------------------------------------------------


#endif  // !MARK_CONFIG_HPP_
