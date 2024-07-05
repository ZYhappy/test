//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/16/2020 08:22:28
//

#ifndef MARK_TIME_SIMPLE_TIMER_HPP_
#define MARK_TIME_SIMPLE_TIMER_HPP_

#include <chrono>
#include <ciso646>
#include <type_traits>
#include <utility>

namespace mark {

template <typename T>
struct __is_duration : std::false_type {};

template <typename Rep, typename Period>
struct __is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};

template <typename Clock = std::chrono::steady_clock>
class simple_timer {
  using clock_type   = Clock;
  using time_point_t = std::chrono::time_point<clock_type>;

  static_assert(Clock::is_steady, "timer clock must be steady");

 public:
  simple_timer() noexcept(noexcept(clock_type::now()))
      : m_begin(clock_type::now()), m_end(m_begin) {}
  template <typename Duration>
  constexpr explicit simple_timer(const Duration& Rel_time) noexcept(
      noexcept(clock_type::now()))
      : m_begin(clock_type::now()), m_end(m_begin + Rel_time) {}

  template <typename Duration,
            std::enable_if_t<__is_duration<Duration>::value, int> = 0>
  constexpr simple_timer(
      const time_point_t& begin,
      const Duration&
          Rel_time) noexcept(std::is_nothrow_copy_constructible_v<time_point_t>)
      : m_begin(begin), m_end(m_begin + Rel_time) {}

  constexpr simple_timer(
      const time_point_t& begin,
      const time_point_t&
          end) noexcept(std::is_nothrow_copy_constructible_v<time_point_t>)
      : m_begin(begin), m_end(end) {}

  simple_timer(simple_timer&& other) noexcept(
      std::is_nothrow_copy_constructible_v<time_point_t>and noexcept(
          std::declval<simple_timer<clock_type>&>().reset()))
      : m_begin(other.m_begin), m_end(other.m_end) {}
  simple_timer(const simple_timer& other) noexcept(
      std::is_nothrow_copy_constructible_v<time_point_t>and noexcept(
          std::declval<simple_timer<clock_type>&>().reset()))
      : m_begin(other.m_begin), m_end(other.m_end) {
    this->reset();
  }

  simple_timer& operator=(simple_timer&& other) noexcept(
      std::is_nothrow_copy_assignable_v<time_point_t>and noexcept(
          std::declval<simple_timer<clock_type>&>().reset())) {
    this->m_begin = other.m_begin;
    this->m_end   = other.m_end;
    this->reset();
    return *this;
  }
  simple_timer& operator=(const simple_timer& other) noexcept(
      std::is_nothrow_copy_assignable_v<time_point_t>and noexcept(
          std::declval<simple_timer<clock_type>&>().reset())) {
    this->m_begin = other.m_begin;
    this->m_end   = other.m_end;
    this->reset();
    return *this;
  }
  ~simple_timer() = default;

 public:
  template <typename Duration,
            std::enable_if_t<__is_duration<Duration>::value, int> = 0>
  void set(const Duration& Rel_time) noexcept(noexcept(clock_type::now())) {
    m_begin = clock_type::now();
    m_end   = m_begin + Rel_time;
  }
  void reset() noexcept(noexcept(clock_type::now())) {
    const auto diff_time = m_end - m_begin;
    m_begin              = clock_type::now();
    m_end                = m_begin + diff_time;
  }

 public:
  const time_point_t& begin() const noexcept { return m_begin; }

  const time_point_t& end() const noexcept { return m_end; }

 public:
  template <typename Duration = std::chrono::milliseconds,
            std::enable_if_t<__is_duration<Duration>::value, int> = 0>
  int64_t elapsed() const noexcept(noexcept(clock_type::now())) {
    return std::chrono::duration_cast<Duration>(clock_type::now() - m_begin)
        .count();
  }
  bool time_over() const noexcept(noexcept(clock_type::now())) {
    bool ret = false;
    if (m_begin != m_end) {
      const auto now = clock_type::now();
      if (now > m_end) {
        ret = true;
      }
    } else {
      ret = true;
    }
    return ret;
  }

  bool friend operator<(const simple_timer& lhs, const simple_timer& rhs) {
    return lhs.m_end < rhs.m_end;
  }

 private:
  time_point_t m_begin;
  time_point_t m_end;
};

}  // namespace mark

#endif  // !MARK_TIME_SIMPLE_TIMER_HPP_
