//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 01/25/2021 18:12:59
//

#ifndef MARK_TIME_SLEEP_HPP_
#define MARK_TIME_SLEEP_HPP_


#include <chrono>
#include "mark/config.hpp"

#if defined(__MARK_WIN)
#include <windows.h>
#elif defined(__MARK_LINUX)
#include <unistd.h>
#endif

namespace mark {

template <class _Rep, class _Period>
void sleep_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
  auto dwMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(_Rel_time).count();
#if defined(__MARK_WIN)
  ::Sleep(dwMilliseconds);
#elif defined(__MARK_LINUX)
  ::usleep(dwMilliseconds * 1000);
#else
  static_assert(false, "unknown platform");
#endif
}

}

#endif  // MARK_TIME_SLEEP_HPP_
