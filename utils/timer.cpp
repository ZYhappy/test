#include <vector>
#include <cassert>
#include <algorithm>
#include "timer.h"

namespace utils {



TimerManager::TimerManager() {
}

TimerManager::~TimerManager() {
  Destory();
}

void TimerManager::Destory() {
  std::unique_lock lock(timers_mtx_);
  stop_.store(true);
  lock.unlock();
  timers_cv_.notify_all();
}

void TimerManager::Update() {
  std::unique_lock lock(timers_mtx_);
  if (IsEmpty() && !stop_.load()) {
    timers_cv_.wait(lock, [&] {
      return !IsEmpty() || stop_.load();
    });
    if (stop_.load()) return;
  }

  const auto now = std::chrono::system_clock::now();

  for (auto it(timers_.begin()); it != timers_.end(); ) {
    if (it->first > now) {
      return;
    }

    if (it->second.status_ == TimerStatus::kStop) {
      ++it;
      continue;
    }
    // 支持取消自身
    it->second.OnTimer();

    Timer timer(std::move(it->second));
    it = timers_.erase(it);

    if (timer.count_ != 0) {
      const auto tp = timer.id_->first;
      auto itNew = timers_.insert(std::make_pair(tp, std::move(timer)));
      if (it == timers_.end() || itNew->first < it->first) {
        it = itNew;
      }
    }
  }
}

bool TimerManager::Cancel(TimerId id) {
  std::unique_lock lock(timers_mtx_);
  auto begin = timers_.lower_bound(id->first);
  if (begin == timers_.end()) {
    return false;
  }

  auto end = timers_.upper_bound(id->first);
  for (auto it(begin); it != end; ++it) {
    if (it->second.UniqueId() == id->second) {
      // lazy delete, 万一在定时器执行期间取消另一个计时器
      it->second.count_ = 0;
      return true;
    }
  }

  return false;
}

void TimerManager::CancelAll() {
  std::unique_lock lock(timers_mtx_);
  timers_.clear();
}

DurationS TimerManager::NearestTimer()  {
  std::unique_lock lock(timers_mtx_);
  if (timers_.empty()) {
    return DurationS::max();
  }
  const auto& timer = timers_.begin()->second;
  auto now = std::chrono::system_clock::now();
  if (now > timer.Id()->first) {
    return DurationS::min();
  } else {
    return std::chrono::duration_cast<DurationS>(timer.Id()->first - now);
  }
}

bool TimerManager::IsEmpty()const {
  if (timers_.empty()) 
    return true;

  auto target = std::find_if(timers_.begin(), timers_.end(), [&](const auto& iter) {
    return iter.second.status_ == TimerStatus::kRuning;
  });
  if (target == timers_.cend()) {
    return true;
  }

  return false;
}

bool TimerManager::StopScheduler(TimerId id, int64_t& remaining_time) {
  std::unique_lock lock(timers_mtx_);
  auto begin = timers_.lower_bound(id->first);
  if (begin == timers_.end())
    return false;

  auto end = timers_.upper_bound(id->first);
  for (auto it(begin); it != end; ++it) {
    if (it->second.UniqueId() == id->second) {
      if (it->second.status_ != TimerStatus::kRuning) {
        return false;
      }
      it->second.status_ = TimerStatus::kStop;

      const auto now = std::chrono::system_clock::now();

      if (it->first < now) {
        return false;
      }
      it->second.remaining_time_ = std::chrono::duration_cast<DurationS>(it->first - now);
      remaining_time = it->second.remaining_time_.count();
      return true;
    }
  }
  return false;
}

bool TimerManager::StartScheduler(TimerId id) {
  std::unique_lock lock(timers_mtx_);
  auto begin = timers_.lower_bound(id->first);
  if (begin == timers_.end())
    return false;

  auto end = timers_.upper_bound(id->first);
  for (auto it(begin); it != end; ++it) {
    if (it->second.UniqueId() == id->second) {
      Timer timer(std::move(it->second));

      if (timer.count_ != 0) {
        const auto now = std::chrono::system_clock::now();
        timer.id_->first = now + it->second.remaining_time_;
        timer.status_ = TimerStatus::kRuning;
        timers_.insert(std::make_pair(timer.id_->first, std::move(timer)));
      }
      timers_.erase(it);
      return true;
    }
  }
  return false;
}


TimerManager::Timer::Timer(const TimePoint& tp, const uint32_t unique_id) :
    id_(std::make_shared<std::pair<TimePoint, unsigned int>>(tp, unique_id)){
}

TimerManager::Timer::Timer(Timer&& timer) noexcept{
    _Move(std::move(timer));
}

TimerManager::Timer& TimerManager::Timer::operator= (Timer&& timer) noexcept {
  if (this != &timer) {
    _Move(std::move(timer));
  }
  return *this;
}

void TimerManager::Timer::_Move(Timer&& timer) {
  this->id_       = std::move(timer.id_);
  this->func_     = std::move(timer.func_);
  this->interval_ = std::move(timer.interval_);
  this->count_    = timer.count_;
  this->status_   = timer.status_;
}

void TimerManager::Timer::OnTimer() {
  if (!func_ || count_ == 0) {
    return;
  }
  if (count_ == kForever || count_-- > 0) {
    func_();
    id_->first += interval_;
  } else {
    count_ = 0; // 防止count_ 为-1以外值
  }
}

TimerId TimerManager::Timer::Id() const {
  return id_;
}

unsigned int TimerManager::Timer::UniqueId() const {
  return id_->second;
}

} // end namespace utils

