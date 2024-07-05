
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/24/2020 13:27
//

#ifndef MARK_MEMORY_NON_NULL_HPP_
#define MARK_MEMORY_NON_NULL_HPP_

#include <cassert>
#include <utility>

#include "mark/utility.hpp"
#include "mark/optional/optional.hpp"

namespace mark {

template<typename T>
class non_null {
 public:
  non_null(std::nullptr_t) = delete;
  non_null operator=(std::nullptr_t) = delete;
  non_null &operator++() = delete;
  non_null &operator--() = delete;
  non_null operator++(int) = delete;
  non_null operator--(int) = delete;
  non_null &operator+=(std::ptrdiff_t) = delete;
  non_null &operator-=(std::ptrdiff_t) = delete;
  void operator[](std::ptrdiff_t) const = delete;

 public:
  typedef T element_type;

 public:
  template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, std::add_pointer_t<T>>>>
  constexpr explicit non_null(U&& __other_ptr) : __ptr(std::forward<U>(__other_ptr)) {
    assert(__ptr != nullptr);
  }

  template<typename = std::enable_if_t<!std::is_same_v<std::nullptr_t, std::add_pointer_t<T>>>>
  constexpr explicit non_null(T* __other_ptr) : __ptr(__other_ptr) {
    assert(__ptr != nullptr);
  }

  template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, std::add_pointer_t<T>>>>
  constexpr explicit non_null(const non_null<U>& __other_ptr) : non_null(__other_ptr.__ptr) {}

 public:
  non_null(const non_null&) = default;
  non_null& operator=(const non_null&) = default;

 public:
  element_type *get() const noexcept {
    assert(__ptr);
    return __ptr;
  }

  T& operator*() const noexcept {
    return *get();
  }

  T* operator->() const noexcept {
    return get();
  }

  template <typename U>
  non_null<U> cast() const {
    return non_null<U>(__ptr);
  }

 private:
  T *__ptr;
};

template<typename T>
auto make_non_null(T &&ptr) {
  using _non_null_t = non_null<std::remove_pointer_t<mark::remove_cvref_t<T>>>;
  if (ptr != nullptr) {
    return optional<_non_null_t>(std::forward<T>(ptr));
  } else {
    return optional<_non_null_t>{nullopt};
  }
}

template <class T>
std::ostream& operator<<(std::ostream& os, const non_null<T>& val) {
  os << val.get();
  return os;
}

template <typename T, typename U>
auto operator==(const non_null<T>& _x, const non_null<U>& _y) {
  return _x.get() == _y.get();
}

template <typename T, typename U>
auto operator!=(const non_null<T>& _x, const non_null<U>& _y) {
  return _x.get() != _y.get();
}

template <typename T, typename U>
auto operator<(const non_null<T>& _x, const non_null<U>& _y) {
  return _x.get() < _y.get();
}

template <typename T, typename U>
auto operator<=(const non_null<T>& _x, const non_null<U>& _y) {
  return _x.get() <= _y.get();
}

template <typename T, typename U>
auto operator>(const non_null<T>& _x, const non_null<U>& _y) {
  return _x.get() > _y.get();
}

template <typename T, typename U>
auto operator>=(const non_null<T>& _x, const non_null<U>& _y) {
  return _x.get() >= _y.get();
}

template <class T, class U>
std::ptrdiff_t operator-(const non_null<T>&, const non_null<U>&) = delete;
template <class T>
non_null<T> operator-(const non_null<T>&, std::ptrdiff_t) = delete;
template <class T>
non_null<T> operator+(const non_null<T>&, std::ptrdiff_t) = delete;
template <class T>
non_null<T> operator+(std::ptrdiff_t, const non_null<T>&) = delete;

}  // namespace mark

namespace std {

template <typename T>
struct hash<mark::non_null<T>> {
  std::size_t operator()(const mark::non_null<T>& value) const {
    return hash<add_pointer_t<T*>>()(value);
  }
};

}  // namespace std

#endif  // !MARK_MEMORY_NON_NULL_HPP_
