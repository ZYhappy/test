#ifndef UTIL_TIMER_H
#define UTIL_TIMER_H

#include <stdint.h>
#include <map>
#include <chrono>
#include <functional>
#include <memory>
//#include <ostream>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace utils::internal {

using UniqueId_t = uint32_t;
using DurationS  = std::chrono::seconds;
using DurationMs = std::chrono::milliseconds;
using TimePoint  = std::chrono::system_clock::time_point;
using TimerId    = std::shared_ptr<std::pair<TimePoint, UniqueId_t> >;

constexpr int kForever = -1;

//inline std::ostream& operator<< (std::ostream& os, const TimerId& d) {
//    os << "[TimerId:" << (void*)d.get() << "]";
//    return os;
//}

enum TimerStatus: int16_t {
  kInit = 0,
  kRuning,
  kStop,
};

///@brief TimerManager class
///

class TimerManager final {
public:
  TimerManager();
  ~TimerManager();

  TimerManager(const TimerManager&) = delete;
  void operator= (const TimerManager&) = delete;

  void Destory();
  // Tick
  void Update();

  ///@brief Schedule timer at absolute timepoint then repeat with period
  ///@param triggerTime The absolute time when timer first triggered
  ///@param period After first trigger, will be trigger by this period repeated until RepeatCount
  ///@param f The function to execute
  ///@param args Args for f
  ///
  /// RepeatCount: Timer will be canceled after trigger RepeatCount times, kForever implies forever.
  template <int RepeatCount, typename Duration, typename F, typename... Args>
  TimerId ScheduleAtWithRepeat(const TimePoint& triggerTime, const Duration& period, F&& f, Args&& ... args);

  ///@brief Schedule timer with period
  ///@param period: Timer will be triggered every period
  ///
  /// RepeatCount: Timer will be canceled after triggered RepeatCount times, kForever implies forever.
  /// PAY ATTENTION: Timer's first trigger isn't at once, but after period time
  template <int RepeatCount, typename Duration, typename F, typename... Args>
  TimerId ScheduleAfterWithRepeat(const Duration& period, F&& f, Args&& ... args);

  ///@brief Schedule timer at timepoint
  ///@param triggerTime: The absolute time when timer trigger
  ///
  /// It'll trigger only once
  template <typename F, typename... Args>
  TimerId ScheduleAt(const TimePoint& triggerTime, F&& f, Args&& ... args);

  ///@brief Schedule timer after duration
  ///@param duration: After duration, timer will be triggered
  ///
  /// It'll trigger only once
  template <typename Duration, typename F, typename... Args>
  TimerId ScheduleAfter(const Duration& duration, F&& f, Args&& ... args);

  ///@brief Cancel timer
  bool Cancel(TimerId id);

  void CancelAll();

  ///@brief how far the nearest timer will be trigger.
  DurationS NearestTimer();

  // timer_ empty or timer_'s status are all kStop
  bool IsEmpty()const;

  //  only for stop timer where status is kRunning
  bool StopScheduler(TimerId id, int64_t&);

  //  only for start timer where status is kStop
  bool StartScheduler(TimerId id);
private:
  class Timer {
    friend class TimerManager;
  public:
    Timer(const TimePoint& tp, const uint32_t unique_id);

    // only move
    Timer(Timer&& timer) noexcept;
    Timer& operator= (Timer&&) noexcept;

    Timer(const Timer&) = delete;
    void operator= (const Timer&) = delete;

    template <typename F, typename... Args>
    void SetCallback(F&& f, Args&& ... args);

    void OnTimer();

    TimerId Id() const;
    unsigned int UniqueId() const;

  private:
    void _Move(Timer&& timer);

    TimerId id_ = nullptr;

    std::function<void()> func_ = nullptr;
    DurationS interval_ = DurationS(0);
    int count_ = kForever;
    TimerStatus status_{ TimerStatus::kStop };

    DurationS remaining_time_;
  };

  std::atomic_bool stop_{ false };
  std::mutex timers_mtx_;
  std::multimap<TimePoint, Timer> timers_;
  std::condition_variable timers_cv_;

  friend class Timer;

  std::atomic_int32_t timer_unique_id_{ 0 };
};


template <int RepeatCount, typename Duration, typename F, typename... Args>
TimerId TimerManager::ScheduleAtWithRepeat(const TimePoint& triggerTime, const Duration& period, F&& f, Args&&... args) {
  static_assert(RepeatCount != 0, "Why do you add a timer with zero count?");

  using namespace std::chrono;

  Timer t(triggerTime, ++timer_unique_id_);
  // precision: seconds
  t.interval_ = duration_cast<DurationS>(period);
  t.count_ = RepeatCount;
  t.status_ = TimerStatus::kRuning;
  TimerId id = t.Id();

  t.SetCallback(std::forward<F>(f), std::forward<Args>(args)...);

  std::unique_lock lock(timers_mtx_);
  timers_.insert(std::make_pair(triggerTime, std::move(t)));
  lock.unlock();
  timers_cv_.notify_one();
  return id;
}

template <int RepeatCount, typename Duration, typename F, typename... Args>
TimerId TimerManager::ScheduleAfterWithRepeat(const Duration& period, F&& f, Args&&... args) {
  const auto now = std::chrono::system_clock::now();
  return ScheduleAtWithRepeat<RepeatCount>(now + period,
                                           period,
                                           std::forward<F>(f),
                                           std::forward<Args>(args)...);
}

template <typename F, typename... Args>
TimerId TimerManager::ScheduleAt(const TimePoint& triggerTime, F&& f, Args&&... args) {
  return ScheduleAtWithRepeat<1>(triggerTime,
                                 DurationS(0), // dummy
                                 std::forward<F>(f),
                                 std::forward<Args>(args)...);
}

template <typename Duration, typename F, typename... Args>
TimerId TimerManager::ScheduleAfter(const Duration& duration, F&& f, Args&&... args) {
  const auto now = std::chrono::system_clock::now();
  return ScheduleAt(now + duration,
                    std::forward<F>(f),
                    std::forward<Args>(args)...);
}

template <typename F, typename... Args>
void TimerManager::Timer::SetCallback(F&& f, Args&&... args) {
  func_ = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
}

} // end namespace internal

#endif //UTIL_TIMER_H

