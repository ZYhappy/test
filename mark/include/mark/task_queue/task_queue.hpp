
//
// Author: Zhengyu
// Date: 10/23/2020 17:04
//

#ifndef MARK_TASK_QUEUE_TASK_QUEUE_HPP_
#define MARK_TASK_QUEUE_TASK_QUEUE_HPP_

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

namespace mark {

class task_queue {
 public:
  task_queue() = default;

 public:
  void start() {}
  void stop();
  void pause();
  void resume();

 public:
  template <typename F>
  void push_task(F&& task);

  template <typename F>
  void unsafe_push_task(F&& task);

 private:
  using __task_t = std::function<void()>;
  std::deque<__task_t> m_task_queue;
  std::mutex m_mtx;
  std::condition_variable m_cv;
  std::thread m_exec_thread;
};

}  // namespace mark

#endif  // !MARK_TASK_QUEUE_TASK_QUEUE_HPP_
