#ifndef UTILS_SINGLETON_H_
#define UTILS_SINGLETON_H_

#include <type_traits>

namespace Utils{

template<typename T>
class Singleton {
public:
  static T& get_instance() noexcept(std::is_nothrow_constructible<T>::value) {
    static T instance;
    return instance;
  }

  virtual ~Singleton() noexcept {};

  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;
protected:
  Singleton(){}

};

} // namespace Utils


#endif // !UTILS_SINGLETON_H_
