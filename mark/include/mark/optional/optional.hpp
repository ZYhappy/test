
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 07/06/2020 23:24
//

#ifndef MARK_OPTIONAL_OPTIONAL_HPP_
#define MARK_OPTIONAL_OPTIONAL_HPP_

#include "mark/utility.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

namespace mark {

template <typename T>
class optional;

#if 0

struct nullopt_t {
  struct __secret_tag {  explicit __secret_tag() = default; };
  constexpr explicit nullopt_t(__secret_tag, __secret_tag) noexcept {}
};

constexpr nullopt_t nullopt{nullopt_t::__secret_tag{}, nullopt_t::__secret_tag{}};

#endif

namespace __optional_detail {

template <typename T, bool = std::is_trivially_destructible<T>::value>
struct __optional_destruct_base;

template <typename T>
struct __optional_destruct_base<T, false> {
  typedef T value_type;
  static_assert(std::is_object<T>::value);

  union {
    char __null_state;
    value_type __value;
  };
  bool __engaged;

  ~__optional_destruct_base() {
    if (__engaged) { __value.~value_type(); }
  }

  constexpr __optional_destruct_base() noexcept
    : __null_state(), __engaged(false) {}

  template <typename... Args>
  constexpr explicit __optional_destruct_base(in_place_t, Args&&... args)
    : __value(std::forward<Args>(args)...), __engaged(true) {}

  void reset() noexcept {
    if (__engaged) {
      __value.~value_type();
      __engaged = false;
    }
  }
};

template <typename T>
struct __optional_destruct_base<T, true> {
  typedef T value_type;
  static_assert(std::is_object<T>::value);

  union {
    char __null_state;
    value_type __value;
  };
  bool __engaged;

  ~__optional_destruct_base() = default;

  constexpr __optional_destruct_base() noexcept
    : __null_state(), __engaged(false) {}

  template <typename... Args>
  constexpr explicit __optional_destruct_base(in_place_t, Args&&... args)
    : __value(std::forward<Args>(args)...), __engaged(true) {}

  void reset() noexcept {
    if (__engaged) { __engaged = false; }
  }
};

template <typename T, bool = std::is_reference<T>::value>
struct __optional_storage_base;

template <typename T>
struct __optional_storage_base<T, false> : public __optional_destruct_base<T> {
  using __base = __optional_destruct_base<T>;
  using value_type = T;
  using __base::__base;

  constexpr bool has_value() const noexcept { return this->__engaged; }

  constexpr value_type& __get() & noexcept { return this->__value; }

  constexpr const value_type& __get() const& noexcept { return this->__value; }

  constexpr value_type&& __get() && noexcept {
    return std::move(this->__value);
  }

  constexpr const value_type&& __get() const&& noexcept {
    return std::move(this->__value);
  }

  template <typename... Args>
  void __construct(Args... args) {
    ::new ((void*)std::addressof(this->__value))
      value_type(std::forward<Args>(args)...);
    this->__engaged = true;
  }

  template <typename OtherOption>
  void __construct_from(OtherOption&& opt) {
    if (opt.has_value()) {
      __construct(std::forward<OtherOption>(opt).__get());
    }
  }

  template <typename OtherOption>
  void __assign_from(OtherOption&& opt) {
    if (this->__engaged == opt.has_value()) {
      if (this->__engaged) {
        this->__value = std::forward<OtherOption>(opt).__get();
      }
    } else {
      if (this->__engaged) {
        this->reset();
      } else {
        __construct(std::forward<OtherOption>(opt).__get());
      }
    }
  }
};

template <class T>
struct __optional_storage_base<T, true> {
  using value_type = T;
  using __raw_type = std::remove_reference_t<T>;
  __raw_type* __value;

  template <class U>
  static constexpr bool __can_bind_reference() {
    using __raw_u = typename std::remove_reference<U>::type;
    using __u_ptr = __raw_u*;
    using __raw_t = typename std::remove_reference<T>::type;
    using _t_ptr = __raw_t*;
    using __check_lvalue_arg = std::integral_constant<
      bool, (std::is_lvalue_reference<U>::value &&
             std::is_convertible<__u_ptr, _t_ptr>::value) ||
              std::is_same<__raw_u, std::reference_wrapper<__raw_t>>::value ||
              std::is_same<
                __raw_u, std::reference_wrapper<
                           typename std::remove_const<__raw_t>::type>>::value>;
    return (std::is_lvalue_reference<T>::value && __check_lvalue_arg::value) ||
           (std::is_rvalue_reference<T>::value &&
            !std::is_lvalue_reference<U>::value &&
            std::is_convertible<__u_ptr, _t_ptr>::value);
  }

  constexpr __optional_storage_base() noexcept : __value(nullptr) {}

  template <typename U>
  constexpr explicit __optional_storage_base(in_place_t, U&& arg)
    : __value(std::addressof(arg)) {
    static_assert(
      __can_bind_reference<U>(),
      "Attempted to construct a reference element in tuple from a "
      "possible temporary");
  }

  void reset() noexcept { __value = nullptr; }

  constexpr bool has_value() const noexcept { return __value != nullptr; }

  constexpr value_type& __get() const& noexcept { return *__value; }

  constexpr value_type&& __get() const&& noexcept {
    return std::forward<value_type>(*__value);
  }

  template <typename U>
  void __construct(U&& __val) {
    _MARK_ASSERT(!has_value(), "");
    static_assert(
      __can_bind_reference<U>(),
      "Attempted to construct a reference element in tuple from a "
      "possible temporary");
    __value = std::addressof(__val);
  }

  template <typename _That>
  void __construct_from(_That&& __opt) {
    if (__opt.has_value()) __construct(std::forward<_That>(__opt).__get());
  }

  template <typename _That>
  void __assign_from(_That&& __opt) {
    if (has_value() == __opt.has_value()) {
      if (has_value()) *__value = std::forward<_That>(__opt).__get();
    } else {
      if (has_value())
        reset();
      else
        __construct(std::forward<_That>(__opt).__get());
    }
  }
};

template <typename T, bool = std::is_trivially_copy_constructible<T>::value>
struct __optional_copy_construct_base : __optional_storage_base<T> {
  using __optional_storage_base<T>::__optional_storage_base;
};

template <typename T>
struct __optional_copy_construct_base<T, false> : __optional_storage_base<T> {
  using __optional_storage_base<T>::__optional_storage_base;

  __optional_copy_construct_base() = default;

  __optional_copy_construct_base(const __optional_copy_construct_base& other) noexcept(
    noexcept(std::declval<__optional_storage_base<T>&>().__construct_from(other))) {
    this->__construct_from(other);
  }
  __optional_copy_construct_base(__optional_copy_construct_base&&) = default;

  __optional_copy_construct_base& operator=(
    const __optional_copy_construct_base&) = default;
  __optional_copy_construct_base& operator=(__optional_copy_construct_base&&) =
    default;
};

template <typename T, bool = std::is_trivially_move_constructible<T>::value>
struct __optional_move_construct_base : __optional_copy_construct_base<T> {
  using __optional_copy_construct_base<T>::__optional_copy_construct_base;
};

template <typename T>
struct __optional_move_construct_base<T, false>
  : __optional_copy_construct_base<T> {
  using value_type = T;
  using __optional_copy_construct_base<T>::__optional_copy_construct_base;

  __optional_move_construct_base() = default;
  __optional_move_construct_base(const __optional_move_construct_base&) =
    default;

  __optional_move_construct_base(
    __optional_move_construct_base&&
      other) noexcept(std::is_nothrow_move_constructible<value_type>::value) {
    this->__construct_from(std::move(other));
  }

  __optional_move_construct_base& operator=(
    const __optional_move_construct_base&) = default;
  __optional_move_construct_base& operator=(__optional_move_construct_base&&) =
    default;
};

template <typename T, bool = std::is_trivially_copy_assignable<T>::value>
struct __optional_copy_assign_base : __optional_move_construct_base<T> {
  using __optional_move_construct_base<T>::__optional_move_construct_base;
};

template <typename T>
struct __optional_copy_assign_base<T, false>
  : __optional_move_construct_base<T> {
  using __optional_move_construct_base<T>::__optional_move_construct_base;

  __optional_copy_assign_base() = default;
  __optional_copy_assign_base(const __optional_copy_assign_base&) = default;
  __optional_copy_assign_base(__optional_copy_assign_base&&) = default;

  __optional_copy_assign_base& operator=(
    const __optional_copy_assign_base& other) {
    this->__assign_from(other);
    return *this;
  }
  __optional_copy_assign_base& operator=(__optional_copy_assign_base&&) =
    default;
};

template <typename T, bool = std::is_trivially_move_assignable<T>::value>
struct __optional_move_assign_base : __optional_copy_assign_base<T> {
  using __optional_copy_assign_base<T>::__optional_copy_assign_base;
};

template <typename T>
struct __optional_move_assign_base<T, false> : __optional_copy_assign_base<T> {
  using value_type = T;
  using __optional_copy_assign_base<T>::__optional_copy_assign_base;

  __optional_move_assign_base() = default;
  __optional_move_assign_base(const __optional_move_assign_base&) = default;
  __optional_move_assign_base(__optional_move_assign_base&&) = default;

  __optional_move_assign_base& operator=(const __optional_move_assign_base&) =
    default;
  __optional_move_assign_base&
  operator=(__optional_move_assign_base&& other) noexcept(
    std::is_nothrow_move_assignable<value_type>::value&&
      std::is_nothrow_move_constructible<value_type>::value) {
    this->__assign_from(std::move(other));
    return *this;
  }
};

template <typename T>
using __optional_sfinae_ctor_base_t = __sfinae_ctor_base<
  std::is_copy_constructible<T>::value, std::is_move_constructible<T>::value>;

template <typename T>
using __optional_sfinae_assign_base_t = __sfinae_assign_base<
  (std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value),
  (std::is_move_constructible<T>::value && std::is_move_assignable<T>::value)>;

}  // namespace __optional_detail

using nullopt_t = std::nullopt_t;
inline constexpr nullopt_t nullopt = std::nullopt;

namespace __result_detail {

template <typename T>
struct Ok;

template <typename E>
struct Err;

}  // namespace __result_detail

template <typename T>
auto Ok(T&& val) -> __result_detail::Ok<T>;

template <typename E>
auto Err(E&& val) -> __result_detail::Err<E>;

template <typename T, typename E>
class result;

template <typename T>
class optional;

template <typename T>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<optional<T>> : std::true_type {};

template <typename T>
optional<T> Some(T&& value) {
  return optional<T>(std::forward<T>(value));
}

template <typename T>
class optional : private __optional_detail::__optional_sfinae_ctor_base_t<T>,
                 private __optional_detail::__optional_sfinae_assign_base_t<T>,
                 private __optional_detail::__optional_move_assign_base<T> {
 public:
  using value_type = T;
  using base_optional = __optional_detail::__optional_move_assign_base<T>;

 private:
  // Disable the reference extension using this static assert.
  static_assert(
    !std::is_same_v<value_type, in_place_t>,
    "instantiation of optional with in_place_t is ill-formed");
  static_assert(
    !std::is_same_v<remove_cvref_t<value_type>, nullopt_t>,
    "instantiation of optional with nullopt_t is ill-formed");
  static_assert(
    std::is_destructible_v<value_type>,
    "instantiation of optional with a non-destructible type is ill-formed");

 public:
  constexpr optional() noexcept {}
  constexpr optional(const optional&) = default;
  constexpr optional(optional&&) = default;
  constexpr optional(nullopt_t) noexcept {}

  template <
    typename... Args, typename = typename std::enable_if<std::is_constructible<
                        value_type, Args...>::value>::type>
  constexpr explicit optional(in_place_t, Args&&... args)
    : base_optional(in_place, std::forward<Args>(args)...){};

  template <
    typename Up, typename... Args,
    typename = typename std::enable_if<std::is_constructible<
      value_type, std::initializer_list<Up>&, Args...>::value>::type>
  constexpr explicit optional(
    in_place_t, std::initializer_list<Up> il, Args&&... args)
    : base_optional(in_place, il, std::forward<Args>(args)...){};

 public:
  template <
    typename Up = value_type,
    std::enable_if_t<
      check_args_ctor<optional, T, Up>::template _enable_implicit<Up>::value,
      int> = 0>
  constexpr optional(Up&& v) : base_optional(in_place, std::forward<Up>(v)) {}

  template <
    typename Up,
    std::enable_if_t<
      check_args_ctor<optional, T, Up>::template _enable_explicit<Up>::value,
      int> = 0>
  constexpr explicit optional(Up&& v)
    : base_optional(in_place, std::forward<Up>(v)) {}

  template <
    typename Up, std::enable_if_t<
                   check_like_ctor<::mark::optional, T, Up, const Up&>::
                     template _enable_implicit<Up>::value,
                   int> = 0>
  optional(const optional<Up>& v) {
    this->__construct_from(v);
  }

  template <
    typename Up, std::enable_if_t<
                   check_like_ctor<::mark::optional, T, Up, const Up&>::
                     template _enable_explicit<Up>::value,
                   int> = 0>
  explicit optional(const optional<Up>& v) {
    this->__construct_from(v);
  }

  template <
    typename Up, std::enable_if_t<
                   check_like_ctor<::mark::optional, T, Up, Up&&>::
                     template _enable_implicit<Up>::value,
                   int> = 0>
  optional(optional<Up>&& v) {
    this->__construct_from(std::move(v));
  }

  template <
    typename Up, std::enable_if_t<
                   check_like_ctor<::mark::optional, T, Up, Up&&>::
                     template _enable_explicit<Up>::value,
                   int> = 0>
  explicit optional(optional<Up>&& v) {
    this->__construct_from(std::move(v));
  }

  optional& operator=(nullopt_t) noexcept {
    this->reset();
    return *this;
  }

  optional& operator=(const optional&) = default;
  optional& operator=(optional&&) = default;

  template <
    typename U = value_type,
    typename = std::enable_if_t<__lazy_and<
      std::integral_constant<
        bool,
        !std::is_same_v<remove_cvref_t<U>, optional> &&
          !(std::is_same_v<U, value_type> && std::is_scalar_v<value_type>)>,
      std::is_constructible<value_type, U>,
      std::is_assignable<value_type&, U>>::value>>
  optional& operator=(U&& v) {
    if (this->has_value()) {
      this->__get() = std::forward<U>(v);
    } else {
      this->__construct(std::forward<U>(v));
    }
    return *this;
  }

  template <
    typename U, std::enable_if_t<
                  check_like_assign<::mark::optional, T, U, const U&>::
                    template _enable_assign<U>::value,
                  int> = 0>
  optional& operator=(const optional<U>& v) {
    this->__assign_from(v);
    return *this;
  }

  template <
    typename U, std::enable_if_t<
                  check_like_ctor<::mark::optional, T, U, U&&>::
                    template _enable_assign<U>::value,
                  int> = 0>
  optional& operator=(optional<U>&& v) {
    this->__assign_from(std::move(v));
    return *this;
  }

  template <
    typename... Args,
    typename = std::enable_if_t<std::is_constructible_v<value_type, Args...>>>
  decltype(auto) emplace(Args&&... args) {
    this->reset();
    this->__construct(std::forward<Args>(args)...);
    return this->__get();
  }

  template <
    typename U, typename... Args,
    typename = std::enable_if_t<
      std::is_constructible_v<value_type, std::initializer_list<U>&, Args...>>>
  value_type& emplace(std::initializer_list<U> il, Args&&... args) {
    this->reset();
    this->__construct(il, std::forward<Args>(args)...);
    return this->__get();
  }

  using base_optional::__get;
  using base_optional::has_value;
  using base_optional::reset;

  constexpr value_type& unwrap(const char* exception_msg = "") & {
    if (!this->has_value()) throw std::runtime_error(exception_msg);
    return this->__get();
  }

  constexpr const value_type& unwrap(const char* exception_msg = "") const& {
    if (!this->has_value()) throw std::runtime_error(exception_msg);
    return this->__get();
  }

  constexpr value_type&& unwrap(const char* exception_msg = "") && {
    if (!this->has_value()) throw std::runtime_error(exception_msg);
    return std::move(this->__get());
  }

  constexpr const value_type&& unwrap(const char* exception_msg = "") const&& {
    if (!this->has_value()) throw std::runtime_error(exception_msg);
    return std::move(this->__get());
  }

  constexpr decltype(auto) value() & {
    if (!this->has_value()) throw std::exception();
    return this->__get();
  }

  constexpr decltype(auto) value() const& {
    if (!this->has_value()) throw std::exception();
    return this->__get();
  }

  constexpr decltype(auto) value() && {
    if (!this->has_value()) throw std::exception();
    return std::move(this->__get());
  }

  constexpr decltype(auto) value() const&& {
    if (!this->has_value()) throw std::exception();
    return std::move(this->__get());
  }

  optional take() {
    optional _new = std::move(*this);
    this->reset();
    return _new;
  }

  constexpr const value_type& operator*() const& {
    _MARK_ASSERT(
      this->has_value(), "optional operator* called for disengaged value");
    return this->__get();
  }

  constexpr value_type& operator*() & {
    _MARK_ASSERT(
      this->has_value(), "optional operator* called for disengaged value");
    return this->__get();
  }

  constexpr value_type&& operator*() && {
    _MARK_ASSERT(
      this->has_value(), "optional operator* called for disengaged value");
    return std::move(*this).__get();
  }

  constexpr const value_type&& operator*() const&& {
    _MARK_ASSERT(
      this->has_value(), "optional operator* called for disengaged value");
    return std::move(*this).__get();
  }

  constexpr std::add_pointer_t<const value_type> operator->() const {
    _MARK_ASSERT(
      this->has_value(), "optional operator* called for disengaged value");
    return &(this->__get());
  }

  constexpr std::add_pointer_t<value_type> operator->() {
    _MARK_ASSERT(
      this->has_value(), "optional operator* called for disengaged value");
    return &(this->__get());
  }

  void swap(optional& opt) noexcept {
    if (this->has_value() == opt.has_value()) {
      using std::swap;
      if (this->has_value()) swap(this->__get(), opt.__get());
    } else {
      if (this->has_value()) {
        opt.__construct(std::move(this->__get()));
        this->reset();
      } else {
        this->__construct(std::move(opt.__get()));
        opt.reset();
      }
    }
  }

  constexpr explicit operator bool() const noexcept {
    return this->has_value();
  }

  auto
  as_ref() & -> optional<std::add_const_t<std::add_lvalue_reference_t<T>>> {
    return this->value();
  }

  auto as_ref()
    const& -> optional<std::add_const_t<std::add_lvalue_reference_t<T>>> {
    return this->value();
  }

  auto as_mut() & -> optional<std::add_lvalue_reference_t<T>> {
    return this->value();
  }

  auto as_mut() const& -> optional<std::add_lvalue_reference_t<T>> {
    return this->value();
  }


 private:
  template <typename Fn, typename _T, typename... Args>
  struct _map_traits {
    using invoke_result_type = std::invoke_result_t<Fn, _T, Args...>;
    using result_type =
      std::conditional_t<std::is_void_v<invoke_result_type>,
        optional<std::tuple<>>,
        optional<invoke_result_type>
      >;
  };
  template <typename Fn, typename... Args, typename Traits = _map_traits<Fn, Args...>>
  static auto _map_invoke(Fn&& fn, Args&&... args) ->
    std::enable_if_t<
      !std::is_void_v<typename Traits::invoke_result_type>,
      typename Traits::result_type
    > {
    return typename Traits::result_type(
      std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...)
    );
  }
  template <typename Fn, typename... Args, typename Traits = _map_traits<Fn, Args...>>
  static auto _map_invoke(Fn&& fn, Args&&... args) ->
    std::enable_if_t<
      std::is_void_v<typename Traits::invoke_result_type>,
      typename Traits::result_type
    > {
    std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
    return typename Traits::result_type(in_place);
  }

 public:
  template <typename Fn, typename... Args>
  auto map(Fn&& f, Args&&... args) & {
    using traits = _map_traits<Fn, T&, Args...>;
    using result_t = typename traits::result_type;

    if (this->has_value()) {
      return _map_invoke(std::forward<Fn>(f), this->value(), std::forward<Args>(args)...);
    } else {
      return result_t();
    }
  }

  template <typename Fn, typename... Args>
  auto map(Fn&& f, Args&&... args) const& {
    using traits = _map_traits<Fn, const T&, Args...>;
    using result_t = typename traits::result_type;

    if (this->has_value()) {
      return _map_invoke(std::forward<Fn>(f), this->value(), std::forward<Args>(args)...);
    } else {
      return result_t();
    }
  }

  template <typename Fn, typename... Args>
  auto map(Fn&& f, Args&&... args) && {
    using traits = _map_traits<Fn, T&&, Args...>;
    using result_t = typename traits::result_type;

    if (this->has_value()) {
      return _map_invoke(std::forward<Fn>(f), std::move(*this).value(), std::forward<Args>(args)...);
    } else {
      return result_t();
    }
  }

  template <typename Fn, typename... Args>
  auto map(Fn&& f, Args&&... args) const&& {
    using traits = _map_traits<Fn, const T&&, Args...>;
    using result_t = typename traits::result_type;

    if (this->has_value()) {
      return _map_invoke(std::forward<Fn>(f), std::move(*this).value(), std::forward<Args>(args)...);
    } else {
      return result_t();
    }
  }

  template <typename F, typename O>
  decltype(auto) match(F&& f, O&& o) & {
    if (this->has_value()) {
      return std::forward<F>(f)(this->value());
    } else {
      return o();
    }
  }

  template <typename F, typename O>
  decltype(auto) match(F&& f, O&& o) const & {
    if (this->has_value()) {
      return std::forward<F>(f)(this->value());
    } else {
      return o();
    }
  }

  template <typename F, typename O>
  decltype(auto) match(F&& f, O&& o) && {
    if (this->has_value()) {
      return std::forward<F>(f)(std::move(this->value()));
    } else {
      return o();
    }
  }

  template <typename F, typename O>
  decltype(auto) match(F&& f, O&& o) const && {
    if (this->has_value()) {
      return std::forward<F>(f)(std::move(this->value()));
    } else {
      return o();
    }
  }

  template <typename U, typename F>
  auto map_or(U&& _default, F&& f) & -> U {
    if (this->has_value()) {
      return std::forward<F>(f)(this->value());
    } else {
      return _default;
    }
  }

  template <typename U, typename F>
  auto map_or(U&& _default, F&& f) const & -> U {
    if (this->has_value()) {
      return std::forward<F>(f)(this->value());
    } else {
      return _default;
    }
  }

  template <typename U, typename F>
  auto map_or(U&& _default, F&& f) && -> U {
    if (this->has_value()) {
      return std::forward<F>(f)(std::move(this->value()));
    } else {
      return _default;
    }
  }

  template <typename U, typename F>
  auto map_or(U&& _default, F&& f) const && -> U {
    if (this->has_value()) {
      return std::forward<F>(f)(std::move(this->value()));
    } else {
      return _default;
    }
  }

  template <typename D, typename F>
  decltype(auto) map_or_else(D&& _default, F&& f) & {
    if (this->has_value()) {
      return std::forward<F>(f)(this->value());
    } else {
      return _default();
    }
  }
  template <typename D, typename F>
  decltype(auto) map_or_else(D&& _default, F&& f) const & {
    if (this->has_value()) {
      return std::forward<F>(f)(this->value());
    } else {
      return _default();
    }
  }
  template <typename D, typename F>
  decltype(auto) map_or_else(D&& _default, F&& f) && {
    if (this->has_value()) {
      return std::forward<F>(f)(std::move(this->value()));
    } else {
      return _default();
    }
  }
  template <typename D, typename F>
  decltype(auto) map_or_else(D&& _default, F&& f) const && {
    if (this->has_value()) {
      return std::forward<F>(f)(std::move(this->value()));
    } else {
      return _default();
    }
  }

  template <typename E>
  result<T, E> ok_or(E&& e) & {
    if (this->has_value()) {
      return __result_detail::Ok<T>(this->value());
    } else {
      return __result_detail::Err<E>(std::forward<E>(e));
    }
  }

  template <typename E>
  result<T, E> ok_or(E&& e) && {
    if (this->has_value()) {
      return __result_detail::Ok<T>(std::move(this->value()));
    } else {
      return __result_detail::Err<E>(std::forward<E>(e));
    }
  }

  template <typename F>
  auto ok_or_else(F&& err) && {
    using result_t = result<T, std::invoke_result_t<F>>;
    if (this->has_value()) {
      return result_t::Ok(std::move(this->value()));
    } else {
      return result_t::Err(std::forward<F>(err)());
    }
  }

  template <typename U>
  auto and_(U&& optb) -> std::enable_if_t<is_optional<U>::value, U> {
    if (this->has_value()) {
      return std::forward<U>(optb);
    } else {
      return nullopt;
    }
  }

  template <typename F, typename U = std::invoke_result_t<F, T>>
  auto and_then(F&& f) & -> std::enable_if_t<is_optional<U>::value, U> {
    if (this->has_value()) {
      return std::forward<F>(f)(this->value());
    } else {
      return nullopt;
    }
  }

  template <typename F, typename U = std::invoke_result_t<F, T>>
  auto and_then(F&& f) const & -> std::enable_if_t<is_optional<U>::value, U> {
    if (this->has_value()) {
      return std::forward<F>(f)(this->value());
    } else {
      return nullopt;
    }
  }

  template <typename F, typename U = std::invoke_result_t<F, T>>
  auto and_then(F&& f) && -> std::enable_if_t<is_optional<U>::value, U> {
    if (this->has_value()) {
      return std::forward<F>(f)(std::move(this->value()));
    } else {
      return nullopt;
    }
  }

  template <typename F, typename U = std::invoke_result_t<F, T>>
  auto and_then(F&& f) const && -> std::enable_if_t<is_optional<U>::value, U> {
    if (this->has_value()) {
      return std::forward<F>(f)(std::move(this->value()));
    } else {
      return nullopt;
    }
  }

  template <typename U>
  auto xor_(U&& optb) -> std::enable_if_t<
    std::is_same_v<optional, U>, decltype(std::forward<U>(optb))> {
    if (this->has_value() && !optb.has_value()) {
      return this;
    } else if (!this->has_value() && optb.has_value()) {
      return std::forward<U>(optb);
    } else {
      return nullopt;
    }
  }

  template <typename U>
  auto zip(U&& other) & -> std::enable_if_t<
    is_optional<U>::value, optional<std::tuple<T, typename U::value_type>>> {
    if (this->has_value() && other.has_value()) {
      return std::make_tuple(this->value(), std::forward<U>(other).value());
    } else {
      return mark::nullopt;
    }
  }

  template <typename U>
  auto zip(U&& other) && -> std::enable_if_t<
    is_optional<U>::value, optional<std::tuple<T, typename U::value_type>>> {
    if (this->has_value() && other.has_value()) {
      return std::make_tuple(
        std::move(this->value()), std::forward<U>(other).value());
    } else {
      return mark::nullopt;
    }
  }

  template <typename U>
  remove_cvref_t<T> value_or(U&& other) const& {
    if (this->has_value()) {
      return this->value();
    } else {
      return std::forward<U>(other);
    }
  }

  template <typename U>
  remove_cvref_t<T> value_or(U&& other) && {
    if (this->has_value()) {
      return std::move(this->value());
    } else {
      return std::forward<U>(other);
    }
  }

  template <typename F>
  remove_cvref_t<T> value_or_else(F&& f) const & {
    if (this->has_value()) {
      return this->value();
    } else {
      return std::forward<F>(f)();
    }
  }

  template <typename F>
  remove_cvref_t<T> value_or_else(F&& f) && {
    if (this->has_value()) {
      return std::move(this->value());
    } else {
      return std::forward<F>(f)();
    }
  }

  optional filter(std::function<bool(const T&)> predicate) & {
    if (this->has_value() && predicate(this->value())) {
      return this->value();
    } else {
      return mark::nullopt;
    }
  }

  optional filter(std::function<bool(const T&)> predicate) const& {
    if (this->has_value() && predicate(this->value())) {
      return this->value();
    } else {
      return mark::nullopt;
    }
  }

  optional filter(std::function<bool(const T&)> predicate) && {
    if (this->has_value() && predicate(this->value())) {
      return std::move(this->value());
    } else {
      return mark::nullopt;
    }
  }

  optional filter(std::function<bool(const T&)> predicate) const && {
    if (this->has_value() && predicate(this->value())) {
      return std::move(*this).value();
    } else {
      return mark::nullopt;
    }
  }

  template <typename Fn>
  optional or_else(Fn&& f) && {
    static_assert(is_optional<std::invoke_result_t<Fn>>::value);
    if (this->has_value()) {
      return std::move(this->value());
    } else {
      return std::forward<Fn>(f)();
    }
  }

  template <typename F>
  decltype(auto) inspect(F&& fn) & {
    if (this->has_value()) {
      fn(this->value());
    }
    return *this;
  }
  template <typename F>
  decltype(auto) inspect(F&& fn) const& {
    if (this->has_value()) {
      fn(this->value());
    }
    return *this;
  }
  template <typename F>
  decltype(auto) inspect(F&& fn) && {
    if (this->has_value()) {
      fn(this->value());
    }
    return *this;
  }
  template <typename F>
  decltype(auto) inspect(F&& fn) const&& {
    if (this->has_value()) {
      fn(this->value());
    }
    return *this;
  }
  template <typename F>
  decltype(auto) inspect_none(F&& fn) {
    if (!this->has_value()) {
      fn();
    }
    return *this;
  }

  template <typename _T>
  struct __can_transpose : std::false_type {};

  template <typename _T, typename _E>
  struct __can_transpose<optional<result<_T, _E>>> : std::true_type {};

  template <typename _T>
  struct __transpose_traits;

  template <typename _T, typename _E>
  struct __transpose_traits<optional<result<_T, _E>>> {
    using result_type = result<optional<_T>, _E>;
    using result_ok_type = typename result<optional<_T>, _E>::ok_type;
    using result_err_type = typename result<optional<_T>, _E>::error_type;
  };

  auto transpose() & {
    using __traits = __transpose_traits<optional>;
    using result_t = typename __traits::result_type;
    if (this->has_value()) {
      return std::move(*this).value().map([](auto&& x) {
        return typename __traits::result_ok_type(x);
      });
    } else {
      return result_t();
    }
  }

  auto transpose() const & {
    using __traits = __transpose_traits<optional>;
    using result_t = typename __traits::result_type;
    if (this->has_value()) {
      return std::move(*this).value().map([](auto&& x) {
        return typename __traits::result_ok_type(x);
      });
    } else {
      return result_t();
    }
  }

  auto transpose() && {
    using __traits = __transpose_traits<optional>;
    using result_t = typename __traits::result_type;
    if (this->has_value()) {
      return std::move(*this).value().map([](auto&& x) {
        return typename __traits::result_ok_type(std::move(x));
      });
    } else {
      return result_t();
    }
  }

  auto transpose() const && {
    using __traits = __transpose_traits<optional>;
    using result_t = typename __traits::result_type;
    if (this->has_value()) {
      return std::move(*this).value().map([](auto&& x) {
        return typename __traits::result_ok_type(std::move(x));
      });
    } else {
      return result_t();
    }
  }

  template <typename U>
  bool contains(const U& other) const {
    if (this->has_value()) {
      return other == this->value();
    } else {
      return false;
    }
  }
};

//-----------------------------------------------------------------------------

template <typename T, typename U>
auto operator==(const optional<T>& _x, const optional<U>& _y)
  -> std::enable_if_t<
    std::is_convertible_v<
      decltype(std::declval<const T&>() == std::declval<const U&>()), bool>,
    bool> {
  if (static_cast<bool>(_x) != static_cast<bool>(_y)) { return false; }
  if (!static_cast<bool>(_x)) { return true; }
  return *_x == *_y;
}

template <typename T, typename U>
auto operator!=(const optional<T>& _x, const optional<U>& _y)
  -> std::enable_if_t<
    std::is_convertible_v<
      decltype(std::declval<const T&>() != std::declval<const U&>()), bool>,
    bool> {
  if (static_cast<bool>(_x) != static_cast<bool>(_y)) { return true; }
  if (!static_cast<bool>(_x)) { return false; }
  return *_x != *_y;
}

template <typename T, typename U>
auto operator<(const optional<T>& _x, const optional<U>& _y)
  -> std::enable_if_t<
    std::is_convertible_v<
      decltype(std::declval<const T&>() < std::declval<const U&>()), bool>,
    bool> {
  if (!static_cast<bool>(_y)) { return false; }
  if (!static_cast<bool>(_x)) { return true; }
  return *_x < *_y;
}

template <typename T, typename U>
auto operator>(const optional<T>& _x, const optional<U>& _y)
  -> std::enable_if_t<
    std::is_convertible_v<
      decltype(std::declval<const T&>() > std::declval<const U&>()), bool>,
    bool> {
  if (!static_cast<bool>(_x)) { return false; }
  if (!static_cast<bool>(_y)) { return true; }
  return *_x > *_y;
}

template <typename T, typename U>
auto operator<=(const optional<T>& _x, const optional<U>& _y)
  -> std::enable_if_t<
    std::is_convertible_v<
      decltype(std::declval<const T&>() <= std::declval<const U&>()), bool>,
    bool> {
  if (!static_cast<bool>(_x)) { return true; }
  if (!static_cast<bool>(_y)) { return false; }
  return *_x <= *_y;
}

template <typename T, typename U>
auto operator>=(const optional<T>& _x, const optional<U>& _y)
  -> std::enable_if_t<
    std::is_convertible_v<
      decltype(std::declval<const T&>() >= std::declval<const U&>()), bool>,
    bool> {
  if (!static_cast<bool>(_y)) { return true; }
  if (!static_cast<bool>(_x)) { return false; }
  return *_x >= *_y;
}

//-----------------------------------------------------------------------------

template <class T>
bool operator==(const optional<T>& _x, nullopt_t) noexcept {
  return !static_cast<bool>(_x);
}

template <class T>
bool operator==(nullopt_t, const optional<T>& _x) noexcept {
  return !static_cast<bool>(_x);
}

template <class T>
bool operator!=(const optional<T>& _x, nullopt_t) noexcept {
  return static_cast<bool>(_x);
}

template <class T>
bool operator!=(nullopt_t, const optional<T>& _x) noexcept {
  return static_cast<bool>(_x);
}

template <class T>
bool operator<(const optional<T>&, nullopt_t) noexcept {
  return false;
}

template <class T>
bool operator<(nullopt_t, const optional<T>& _x) noexcept {
  return static_cast<bool>(_x);
}

template <class T>
bool operator<=(const optional<T>& _x, nullopt_t) noexcept {
  return !static_cast<bool>(_x);
}

template <class T>
bool operator<=(nullopt_t, const optional<T>&) noexcept {
  return true;
}

template <class T>
bool operator>(const optional<T>& _x, nullopt_t) noexcept {
  return static_cast<bool>(_x);
}

template <class T>
bool operator>(nullopt_t, const optional<T>&) noexcept {
  return false;
}

template <class T>
bool operator>=(const optional<T>&, nullopt_t) noexcept {
  return true;
}

template <class T>
bool operator>=(nullopt_t, const optional<T>& _x) noexcept {
  return !static_cast<bool>(_x);
}

//-----------------------------------------------------------------------------

template <typename T, typename U>
auto operator==(const optional<T>& _x, const U& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const T&>() == std::declval<const U&>()), bool>,
  bool> {
  return static_cast<bool>(_x) ? *_x == _y : false;
}

template <typename T, typename U>
auto operator==(const U& _x, const optional<T>& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const U&>() == std::declval<const T&>()), bool>,
  bool> {
  return static_cast<bool>(_y) ? _x == *_y : false;
}

template <typename T, typename U>
auto operator!=(const optional<T>& _x, const U& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const T&>() != std::declval<const U&>()), bool>,
  bool> {
  return static_cast<bool>(_x) ? *_x != _y : true;
}

template <typename T, typename U>
auto operator!=(const U& _x, const optional<T>& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const U&>() != std::declval<const T&>()), bool>,
  bool> {
  return static_cast<bool>(_y) ? _x != *_y : true;
}

template <typename T, typename U>
auto operator<(const optional<T>& _x, const U& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const T&>() < std::declval<const U&>()), bool>,
  bool> {
  return static_cast<bool>(_x) ? *_x < _y : true;
}

template <typename T, typename U>
auto operator<(const U& _x, const optional<T>& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const U&>() < std::declval<const T&>()), bool>,
  bool> {
  return static_cast<bool>(_y) ? _x < *_y : false;
}

template <typename T, typename U>
auto operator<=(const optional<T>& _x, const U& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const T&>() <= std::declval<const U&>()), bool>,
  bool> {
  return static_cast<bool>(_x) ? *_x <= _y : true;
}

template <typename T, typename U>
auto operator<=(const U& _x, const optional<T>& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const U&>() <= std::declval<const T&>()), bool>,
  bool> {
  return static_cast<bool>(_y) ? _x <= *_y : false;
}

template <typename T, typename U>
auto operator>(const optional<T>& _x, const U& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const T&>() > std::declval<const U&>()), bool>,
  bool> {
  return static_cast<bool>(_x) ? *_x > _y : false;
}

template <typename T, typename U>
auto operator>(const U& _x, const optional<T>& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const U&>() > std::declval<const T&>()), bool>,
  bool> {
  return static_cast<bool>(_y) ? _x > *_y : true;
}

template <typename T, typename U>
auto operator>=(const optional<T>& _x, const U& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const T&>() >= std::declval<const U&>()), bool>,
  bool> {
  return static_cast<bool>(_x) ? *_x >= _y : false;
}

template <typename T, typename U>
auto operator>=(const U& _x, const optional<T>& _y) -> std::enable_if_t<
  std::is_convertible_v<
    decltype(std::declval<const U&>() >= std::declval<const T&>()), bool>,
  bool> {
  return static_cast<bool>(_y) ? _x >= *_y : true;
}

//-----------------------------------------------------------------------------

inline optional<std::tuple<>> Some() { return std::tuple(); }
inline optional<std::tuple<>> None() { return mark::nullopt; }

//-----------------------------------------------------------------------------

}  // namespace mark

#endif  // !MARK_OPTIONAL_OPTIONAL_HPP_
