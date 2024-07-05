
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 10/31/2020 6:48:54
//

#ifndef MARK_DEFER_TASK_HPP_
#define MARK_DEFER_TASK_HPP_

#include "mark/optional/optional.hpp"

namespace mask {

template <typename F>
class defer_task {
 public:
  defer_task() = delete;
  defer_task(const defer_task&) = delete;
  defer_task operator=(const defer_task&) = delete;

 public:
  defer_task(defer_task&& other) : m_task(std::move(other.m_task)) {
    other.m_task.reset();
  }
  defer_task operator=(defer_task&& other) {
    m_task = std::move(other.m_task);
    other.m_task.reset();
    return *this;
  }
  ~defer_task() {
    m_task.map([] (F&& task) {
      task();
    });
  }
 private:
  mark::optional<F> m_task;
};

};

#endif  // !MARK_DEFER_TASK_HPP_
