
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 06/04/2020 13:27
//

#ifndef MARK_MUTEX_SPIN_MUTEX_HPP_
#define MARK_MUTEX_SPIN_MUTEX_HPP_

#include <atomic>

namespace mark {

class spin_mutex {
 public:
  spin_mutex() = default;
  spin_mutex(const spin_mutex&) = delete;
  spin_mutex& operator=(const spin_mutex&) = delete;

  ~spin_mutex() = default;

 public:
  void lock() {
    while (!m_lock.test_and_set());
  }
  bool try_lock() {
    return !m_lock.test_and_set();
  }
  void unlock() {
    m_lock.clear();
  }

 private:
  std::atomic_flag m_lock  = ATOMIC_FLAG_INIT;
};

}

#endif  // MARK_MUTEX_SPIN_MUTEX_HPP_
