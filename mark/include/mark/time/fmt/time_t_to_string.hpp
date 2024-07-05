//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 03/20/2020 12:04:26
//


#ifndef MARK_TIME_FMT_TIME_T_TO_STRING_
#define MARK_TIME_FMT_TIME_T_TO_STRING_

#include <ctime>
#include <cinttypes>

#include <chrono>
#include <string>
#include <cstdint>
#include <tuple>

namespace mark {

// 百纳秒精度
using hundred_nanoseconds = std::chrono::duration<long long, std::ratio_multiply<std::nano, std::ratio<100, 1>>>;

template <typename Clock, typename Duration = typename Clock::duration>
inline std::time_t to_time_t(const std::chrono::time_point<Clock, Duration>& tp) {
  using namespace std::chrono;
  return system_clock::to_time_t(time_point_cast<Duration>(system_clock::now()) + (tp - time_point_cast<Duration>(Clock::now())));
}

// 将时间戳拆分为google时间戳格式
//
// 拆分百纳秒精度时间戳
// time_t s, ns;
// std::tie(s, ns) = time_t_to_google_timestamp<hundred_nanoseconds>(hns);
//
// 拆分纳秒精度时间戳
// time_t s, ns;
// std::tie(s, ns) = time_t_to_google_timestamp<std::chrono::nanoseconds>(hns);
template <typename Duration>
inline std::tuple<time_t, time_t> time_t_to_google_timestamp(const time_t& timestamp) {
  using namespace std::chrono;
  auto time = Duration(timestamp);
  auto s = duration_cast<seconds>(time);
  auto ns = duration_cast<nanoseconds>(time) - duration_cast<seconds>(time);
  return std::make_tuple(s.count(), ns.count());
}

// 将google时间戳格式合并为time_t
//
// 合并为百纳秒精度时间戳
// auto hns = time_t_to_google_timestamp<hundred_nanoseconds>(_s, _ns);
//
// 合并为纳秒精度时间戳
// auto ns = time_t_to_google_timestamp<std::chrono::nanoseconds>(_s, _ns);
template <typename Duration>
inline time_t google_timestamp_to_time_t(const time_t& s, const time_t& ns) {
  using namespace std::chrono;
  return duration_cast<Duration>(seconds(s) + nanoseconds(ns)).count();
}

// 时间打印至毫秒单位
//
// 打印毫秒精度时间戳:
// time_tostring<std::chrono::system_clock, std::chrono::milliseconds>(ms)
//
// 打印百纳秒精度时间戳:
// time_tostring<std::chrono::system_clock, hundred_nanoseconds>(hundred_ns)
template <typename Clock, typename Duration = typename Clock::duration>
inline std::string time_t_to_string(const std::time_t& timestamp, int8_t time_diff = 8) {
  using namespace std::chrono;
  auto time = Duration(timestamp);
  auto tp = time_point<Clock, Duration>(time);

  auto ms_tp = time_point_cast<milliseconds, Clock, Duration>(tp);
  auto s_tp = time_point_cast<seconds, Clock, Duration>(tp);
  auto milli = ms_tp - s_tp;

  // 时差校正
  ms_tp += milliseconds(time_diff * 60 * 60 * 1000);
  auto ms_time = to_time_t(ms_tp);
  struct tm t;
#ifdef WIN32
  gmtime_s(&t, &ms_time);
#else
  gmtime_r(&ms_time, &t);
#endif

  char time_str[25] = {0};
  sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d:%03" PRId64,
          t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, milli.count());
  return time_str;
}

template <typename Clock, typename Duration = typename Clock::duration>
inline std::string time_point_to_string(const std::chrono::time_point<Clock, Duration>& tp, int8_t time_diff = 8) {
  using namespace std::chrono;

  auto ms_tp = time_point_cast<milliseconds, Clock, Duration>(tp);
  auto s_tp = time_point_cast<seconds, Clock, Duration>(tp);
  auto milli = ms_tp - s_tp;

  // 时差校正
  ms_tp += milliseconds(time_diff * 60 * 60 * 1000);
  auto ms_time = to_time_t(ms_tp);
  struct tm t;
#ifdef WIN32
  gmtime_s(&t, &ms_time);
#else
  gmtime_r(&ms_time, &t);
#endif

  char time_str[25] = {0};
  sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d:%03" PRId64,
          t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, milli.count());
  return time_str;
}

// 获取当前时间
template <
  typename Duration = hundred_nanoseconds,
  typename Clock = std::chrono::system_clock>
inline std::string now(int8_t time_diff = 8) {
  using namespace std::chrono;
  return time_point_to_string(time_point_cast<Duration>(Clock::now()), time_diff);
}

template <
  typename Duration = hundred_nanoseconds,
  typename Clock = std::chrono::system_clock>
inline std::time_t now_time_t() {
  using namespace std::chrono;
  return duration_cast<Duration>(Clock::now().time_since_epoch()).count();
}

template <typename Duration, typename Clock = std::chrono::system_clock>
inline Duration now_timestamp() {
  using namespace std::chrono;
  return duration_cast<Duration>(Clock::now().time_since_epoch());
}

}

#endif  // !MARK_TIME_FMT_TIME_T_TO_STRING_
