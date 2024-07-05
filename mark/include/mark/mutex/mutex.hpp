
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

#ifndef MARK_MUTEX_MUTEX_HPP_
#define MARK_MUTEX_MUTEX_HPP_

#include <mutex>
#include <utility>

#include "mark/result/result.hpp"

namespace mark {

template <typename Guard>
using lock_result = result<Guard, std::error_code>;

template <typename T>
struct try_lock_error {
  struct _WouldBlock {};
  inline static _WouldBlock WouldBlock;
  std::variant<std::error_code, _WouldBlock> enum_value;
};

template <typename T>
struct mutex_guard;

template <typename Guard>
using try_lock_result = result<Guard, try_lock_error<Guard>>;

template <typename T>
struct mutex {
  std::unique_ptr<std::mutex> inner;
  std::unique_ptr<T> t;

  mutex(const T& t)
    : inner(std::make_unique<std::mutex>()), t(std::make_unique<T>(t)) {}

  mutex(T&& t)
    : inner(std::make_unique<std::mutex>()), t(std::make_unique<T>(std::move(t))) {}

  mutex(const mutex&) = delete;

  auto lock() -> lock_result<mutex_guard<T>> {
    try {
      inner->lock();
      return lock_result<mutex_guard<T>>::Ok(this);
    } catch (const std::system_error& err) {
      return lock_result<mutex_guard<T>>::Err(err.code());
    }
  }

  auto try_lock() -> try_lock_result<mutex_guard<T>> {
    try {
      if (inner->try_lock()) {
        return try_lock_result<mutex_guard<T>>::Ok(this);
      } else {
        return try_lock_result<mutex_guard<T>>::Err(try_lock_error<mutex_guard<T>>{try_lock_error<mutex_guard<T>>::WouldBlock});
      }
    } catch (const std::system_error& err) {
      return try_lock_result<mutex_guard<T>>::Err(try_lock_error<mutex_guard<T>>{err.code()});
    }
  }

  static void unlock(mutex_guard<T> unused) {
    (unused);
  }

  auto into_inner() const {
    return lock_result<std::reference_wrapper<T>>::Ok(std::ref(*t));
  }
  auto get_mut() const {
    return lock_result<std::reference_wrapper<T>>::Ok(std::ref(const_cast<T&>(*t)));
  }
};

template <typename T>
struct mutex_guard {
  mutex_guard(mutex<T>* _lock)
    : lock(_lock) {}

  mutex_guard(mutex_guard&& other) noexcept { std::swap(lock, other.lock); }

  mutex_guard& operator=(mutex_guard&& other) noexcept {
    if (lock != nullptr) {
      lock->inner->unlock();
    }

    lock = std::exchange(other.lock, nullptr);
  }

  mutex_guard(const mutex_guard& other) = delete;
  mutex_guard& operator=(const mutex_guard& other) = delete;

  ~mutex_guard() {
    if (lock != nullptr) {
      lock->inner->unlock();
    }
  }

  T& operator*() const { return *(lock->t); }
  T* operator->() const { return lock->t.operator->(); }

  T* get() const { return lock->t->get(); }

 private:
  mutex<T>* lock = nullptr;
};

}  // namespace mark

#endif  // !MARK_MUTEX_MUTEX_HPP_
