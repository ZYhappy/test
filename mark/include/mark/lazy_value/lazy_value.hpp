
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/10/2020 16:50
//

#ifndef MARK_LAZY_VALUE_LAZY_VALUE_HPP_
#define MARK_LAZY_VALUE_LAZY_VALUE_HPP_

#include <memory>
#include "mark/type_traits.hpp"
#include "mark/utility.hpp"

namespace mark {

template <typename T>
class lazy_value {
 public:
  using value_type = T;

  constexpr lazy_value() = default;

  template<typename... Args,
    typename = typename std::enable_if<std::is_constructible<value_type, Args...>::value>::type>
  constexpr explicit lazy_value(in_place_t, Args&&... args)
  : m_inner(std::make_shared<T>(std::forward<Args>(args)...)) {};

  template<typename Up, typename... Args,
    typename = typename std::enable_if<std::is_constructible<value_type, std::initializer_list<Up>&, Args...>::value>::type>
  constexpr explicit lazy_value(in_place_t, std::initializer_list<Up> il, Args&&... args)
  : m_inner(std::make_shared<T>(il, std::forward<Args>(args)...)) {};
  /**
   * \brief 与装入的lazy_value共享底层
   * \return
   */
  lazy_value(const lazy_value&) = default;
  lazy_value(lazy_value&&) noexcept = default;
  /**
   * \brief 与装入的lazy_value共享底层
   * \return
   */
  constexpr lazy_value& operator=(const lazy_value&) noexcept = default;
  constexpr lazy_value& operator=(lazy_value&& other) noexcept {
    m_inner.swap(other.m_inner);
    other.m_inner = nullptr;
    return *this;
  }

 public:
  /**
   * \brief 创建一个新lazy_value，新建底层
   * \return
   */
  // lazy_value clone() const {
  //   if (m_inner != nullptr) {
  //     return *m_inner;
  //   }
  //   return lazy_value();
  // }

  /**
   * \brief 替换原有底层，与原共享lazy_value解除关系
   * \return
   */
  template <typename U = value_type,
    typename = std::enable_if_t<
      __lazy_and<
      std::integral_constant<bool,
        !std::is_same_v<remove_cvref_t<U>, lazy_value> &&
      !(std::is_same_v<U, value_type> && std::is_scalar_v<value_type>)>,
    std::is_constructible<value_type, U>,
    std::is_assignable<value_type&, U>>::value>>
  lazy_value& operator=(U&& other) {
    m_inner = std::make_shared<T>(std::forward<U>(other));
    return *this;
  }

  T* operator->() noexcept {
    return &(this->operator*());
  }

  T& operator*() noexcept {
    if (m_inner.use_count() != 1) {
      m_inner = std::make_shared<T>(*(m_inner.get()));
    }
    return inner_ref();
  }

 public:
  const T& as_ref() noexcept {
    if (m_inner == nullptr) {
      return m_inner = std::make_shared<T>();
    }
    return *m_inner;
  }

  T& as_mut() noexcept {
    const_cast<T&>(this->as_ref());
  }

 public:
  /**
   * \brief
   * \warning 非常不安全，不推荐使用
   * \return
   */
  T* inner() const {
    return m_inner.get();
  }
  /**
   * \brief
   * \warning 非常不安全，不推荐使用
   * \return
   */
  T& inner_ref() const {
    return *inner();
  }

 private:
  std::shared_ptr<T> m_inner;
};

}

#endif  // !MARK_LAZY_VALUE_LAZY_VALUE_HPP_
