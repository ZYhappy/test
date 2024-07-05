
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//

#ifndef MARK_RESULT_RESULT_HPP_
#define MARK_RESULT_RESULT_HPP_

#include "mark/optional/optional.hpp"
#include "mark/utility.hpp"

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

namespace mark {

//-----------------------------------------------------------------------------

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

//-----------------------------------------------------------------------------

namespace __result_detail {

//-----------------------------------------------------------------------------

template <typename T>
struct Ok {
  //static_assert(
  //  !std::is_reference_v<T>,
  //  "instantiation of Ok value with a reference type is ill-formed");

  constexpr Ok() noexcept = default;
  ;
  constexpr Ok(const Ok&) = default;
  constexpr Ok(Ok&&) noexcept = default;

  template <
    typename... Args,
    typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
  constexpr explicit Ok(in_place_t, Args&&... args)
    : t(std::forward<Args>(args)...) {}

  template <
    typename Up, typename... Args,
    typename = std::enable_if_t<
      std::is_constructible_v<T, std::initializer_list<Up>&, Args...>>>
  constexpr explicit Ok(
    in_place_t, std::initializer_list<Up> il, Args&&... args)
    : t(il, std::forward<Args>(args)...) {}

 public:
  template <
    typename U = T,
    std::enable_if_t<
      check_args_ctor<Ok, T, U>::template _enable_implicit<U>::value, int> = 0>
  constexpr Ok(U&& _t) : t(std::forward<U>(_t)) {}

  template <
    typename U,
    std::enable_if_t<
      check_args_ctor<Ok, T, U>::template _enable_explicit<U>::value, int> = 0>
  constexpr explicit Ok(U&& _t) : t(std::forward<U>(_t)) {}

  template <
    typename U,
    std::enable_if_t<
      check_like_ctor<::mark::__result_detail::Ok, T, U, const U&&>::
        template _enable_implicit<U>::value,
      int> = 0>
  Ok(const Ok<U>& v) : t(v.t) {}

  template <
    typename U,
    std::enable_if_t<
      check_like_ctor<::mark::__result_detail::Ok, T, U, const U&&>::
        template _enable_explicit<U>::value,
      int> = 0>
  explicit Ok(const Ok<U>& v) : t(v.t) {}

  template <
    typename U,
    std::enable_if_t<
      check_like_ctor<::mark::__result_detail::Ok, T, U, const U&&>::
        template _enable_implicit<U>::value,
      int> = 0>
  Ok(Ok<U>&& v) : t(std::move(v.t)) {}

  template <
    typename U,
    std::enable_if_t<
      check_like_ctor<::mark::__result_detail::Ok, T, U, const U&&>::
        template _enable_explicit<U>::value,
      int> = 0>
  explicit Ok(Ok<U>&& v) : t(std::move(v.t)) {}

  constexpr bool operator==(const Ok& other) const { return t == other.t; }

  Ok& operator=(const Ok&) = default;
  Ok& operator=(Ok&&) = default;

  T t;
};

//-----------------------------------------------------------------------------

template <typename E>
struct Err {
  //static_assert(
  //  !std::is_reference_v<E>,
  //  "instantiation of Err value with a reference type is ill-formed");

  constexpr Err() noexcept = default;
  ;
  constexpr Err(const Err&) = default;
  constexpr Err(Err&&) noexcept = default;

  template <
    typename... Args,
    typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
  constexpr explicit Err(in_place_t, Args&&... args)
    : e(std::forward<Args>(args)...) {}

  template <
    typename Up, typename... Args,
    typename = std::enable_if_t<
      std::is_constructible_v<E, std::initializer_list<Up>&, Args...>>>
  constexpr explicit Err(
    in_place_t, std::initializer_list<Up> il, Args&&... args)
    : e(il, std::forward<Args>(args)...) {}

 public:
  template <
    typename U = E,
    std::enable_if_t<
      check_args_ctor<Err, E, U>::template _enable_implicit<U>::value, int> = 0>
  constexpr Err(U&& _e) : e(std::forward<U>(_e)) {}

  template <
    typename U,
    std::enable_if_t<
      check_args_ctor<Err, E, U>::template _enable_explicit<U>::value, int> = 0>
  constexpr explicit Err(U&& _e) : e(std::forward<U>(_e)) {}

  template <
    typename U,
    std::enable_if_t<
      check_like_ctor<::mark::__result_detail::Err, E, U, const U&&>::
        template _enable_implicit<U>::value,
      int> = 0>
  Err(const Err<U>& other) : e(other.e) {}

  template <
    typename U,
    std::enable_if_t<
      check_like_ctor<::mark::__result_detail::Err, E, U, const U&&>::
        template _enable_explicit<U>::value,
      int> = 0>
  explicit Err(const Err<U>& other) : e(other.e) {}

  template <
    typename U,
    std::enable_if_t<
      check_like_ctor<::mark::__result_detail::Err, E, U, const U&&>::
        template _enable_implicit<U>::value,
      int> = 0>
  Err(Err<U>&& other) : e(std::move(other.e)) {}

  template <
    typename U,
    std::enable_if_t<
      check_like_ctor<::mark::__result_detail::Err, E, U, const U&&>::
        template _enable_explicit<U>::value,
      int> = 0>
  explicit Err(Err<U>&& other) : e(std::move(other.e)) {}

  Err& operator=(const Err&) = default;
  Err& operator=(Err&&) = default;

  constexpr bool operator==(const Err& other) const { return e == other.e; }

  E e;
};

//-----------------------------------------------------------------------------

}  // namespace __result_detail

//-----------------------------------------------------------------------------

template <typename T, typename E>
class result;

//-----------------------------------------------------------------------------

template <typename T>
struct __is_result_type : std::false_type {};

template <typename T, typename E>
struct __is_result_type<result<T, E>> : std::true_type {};

//-----------------------------------------------------------------------------

template <typename Result>
struct __ok_type_of;

template <typename T, typename E>
struct __ok_type_of<result<T, E>> {
  typedef T type;
};

template <typename T>
struct __is_ok_value : std::false_type {};

template <typename T>
struct __is_ok_value<__result_detail::Ok<T>> : std::true_type {};

//-----------------------------------------------------------------------------

template <typename Result>
using __ok_type_of_t = typename __ok_type_of<Result>::type;

template <typename Result>
struct __error_type_of;

template <typename T, typename E>
struct __error_type_of<result<T, E>> {
  typedef E type;
};

template <typename Result>
using __error_type_of_t = typename __error_type_of<Result>::type;

template <typename T>
struct __is_err_value : std::false_type {};

template <typename T>
struct __is_err_value<__result_detail::Ok<T>> : std::true_type {};

//-----------------------------------------------------------------------------

template <typename T, typename U>
struct __has_same_ok : std::false_type {};

template <typename T, typename E1, typename E2>
struct __has_same_ok<result<T, E1>, result<T, E2>> : std::true_type {};

template <typename T, typename U>
constexpr bool __has_same_ok_v = __has_same_ok<T, U>::value;

template <typename T, typename U>
struct __has_same_error : std::false_type {};

template <typename T1, typename T2, typename E>
struct __has_same_error<result<T1, E>, result<T2, E>> : std::true_type {};

template <typename T, typename U>
constexpr bool __has_same_error_v = __has_same_error<T, U>::value;

//-----------------------------------------------------------------------------

template <typename T>
auto Ok(T&& val) -> __result_detail::Ok<T> {
  // return __result_detail::Ok<mark::remove_cvref_t<T>>(std::forward<T>(val));
  return __result_detail::Ok<T>(std::forward<T>(val));
}

template <typename E>
auto Err(E&& val) -> __result_detail::Err<E> {
  // return __result_detail::Err<mark::remove_cvref_t<E>>(std::forward<E>(val));
  return __result_detail::Err<E>(std::forward<E>(val));
}

//-----------------------------------------------------------------------------

template <typename T, typename E>
class result
  : private __sfinae_ctor_base<
      __all<std::is_copy_constructible_v<T>, std::is_copy_constructible_v<E>>::
        value,
      __all<std::is_move_constructible_v<T>, std::is_move_constructible_v<E>>::
        value>,
    private __sfinae_assign_base<
      __all<
        std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>,
        std::is_copy_constructible_v<E> && std::is_copy_assignable_v<E>>::value,
      __all<
        std::is_move_constructible_v<T> && std::is_move_assignable_v<T>,
        std::is_move_constructible_v<E> && std::is_move_assignable_v<E>>::value> {
  //static_assert(
  //  __all<!std::is_reference_v<T>, !std::is_reference_v<E>>::value,
  //  "variant can not have a reference type as an alternative.");

  static_assert(
    __all<!std::is_void_v<T>, !std::is_void_v<E>>::value,
    "variant can not have a void type as an alternative.");

 public:
  typedef T ok_type;
  typedef E error_type;

 private:
  typedef __result_detail::Ok<T> Ok_t;
  typedef __result_detail::Err<E> Err_t;
  typedef std::variant<Ok_t, Err_t> value_type;

  value_type value;

 public:
  result() = default;
  result(const result&) = default;
  result(result&&) = default;

  result(const Ok_t& ok) : value(ok) {}
  result(Ok_t&& ok) : value(std::in_place_index<0>, std::move(ok.t)) {}

  result(const Err_t& err) : value(err) {}
  result(Err_t&& err) : value(std::in_place_index<1>, std::move(err.e)) {}

  template <
    typename _Ok,
    std::enable_if_t<
      __is_ok_value<_Ok>::value && std::is_constructible_v<Ok_t, _Ok&&>, int> =
      0>
  result(_Ok&& ok) : value(std::in_place_type<Ok_t>, std::forward<_Ok>(ok)) {}

  template <
    typename _Err,
    std::enable_if_t<
      __is_err_value<_Err>::value && std::is_constructible_v<Err_t, _Err&&>,
      int> = 0>
  result(_Err&& err)
    : value(std::in_place_type<Err_t>, std::forward<_Err>(err)) {}

  template <typename _T, typename _E>
  result(const result<_T, _E>&);

  template <typename _T, typename _E>
  result(result<_T, _E>&&);

  result& operator=(const Ok_t& ok) {
    value = ok;
    return *this;
  }
  result& operator=(Ok_t&& ok) {
    value = std::move(ok);
    return *this;
  }
  result& operator=(Err_t& err) {
    value = err;
    return *this;
  }
  result& operator=(Err_t&& err) {
    value = std::move(err);
    return *this;
  }

 public:
  template <
    typename... Args,
    typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
  static constexpr result Ok(Args&&... v) {
    return Ok_t(in_place, std::forward<Args>(v)...);
  }

  template <
    typename Up, typename... Args,
    typename = std::enable_if_t<
      std::is_constructible_v<T, std::initializer_list<Up>&, Args...>>>
  static constexpr result Ok(std::initializer_list<Up> il, Args&&... args) {
    return Ok_t(in_place, il, std::forward<Args>(args)...);
  }

  template <
    typename... Args,
    typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
  static constexpr result Err(Args&&... v) {
    return Err_t(in_place, std::forward<Args>(v)...);
  }

  template <
    typename Up, typename... Args,
    typename = std::enable_if_t<
      std::is_constructible_v<E, std::initializer_list<Up>&, Args...>>>
  static constexpr result Err(std::initializer_list<Up> il, Args&&... args) {
    return Err_t(in_place, il, std::forward<Args>(args)...);
  }

 public:
  template <typename MatchOk, typename MatchErr>
  auto match(MatchOk&& match_ok, MatchErr&& match_err) & {
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return match_ok(x.t); },
        [&](__result_detail::Err<E>& x) { return match_err(x.e); }},
      value);
  }
  template <typename MatchOk, typename MatchErr>
  auto match(MatchOk&& match_ok, MatchErr&& match_err) const& {
    return std::visit(
      overloaded{
        [&](const __result_detail::Ok<T>& x) { return match_ok(x.t); },
        [&](const __result_detail::Err<E>& x) { return match_err(x.e); }},
      value);
  }
  template <typename MatchOk, typename MatchErr>
  auto match(MatchOk&& match_ok, MatchErr&& match_err) && {
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return match_ok(std::move(x.t)); },
        [&](__result_detail::Err<E>& x) { return match_err(std::move(x.e)); }},
      value);
  }
  template <typename MatchOk, typename MatchErr>
  auto match(MatchOk&& match_ok, MatchErr&& match_err) const&& {
    return std::visit(
      overloaded{
        [&](const __result_detail::Ok<T>& x) { return match_ok(std::move(x.t)); },
        [&](const __result_detail::Err<E>& x) { return match_err(std::move(x.e)); }},
      value);
  }

  constexpr bool is_ok() const {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>&) { return true; },
        [](const __result_detail::Err<E>&) { return false; }},
      value);
  }
  constexpr bool is_err() const { return !is_ok(); }

  auto ok() & -> mark::optional<T> {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>& x) { return mark::optional<T>(x.t); },
        [](__result_detail::Err<E>&) {
          return mark::optional<T>(std::nullopt);
        }},
      value);
  }
  auto ok() const& -> mark::optional<T> {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>& x) { return mark::optional<T>(x.t); },
        [](const __result_detail::Err<E>&) {
          return mark::optional<T>(std::nullopt);
        }},
      value);
  }
  auto ok() && -> mark::optional<T> {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>& x) { return mark::optional<T>(std::move(x.t)); },
        [](__result_detail::Err<E>&) {
          return mark::optional<T>(std::nullopt);
        }},
      value);
  }
  auto ok() const&& -> mark::optional<T> {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>& x) { return mark::optional<T>(std::move(x.t)); },
        [](const __result_detail::Err<E>&) {
          return mark::optional<T>(std::nullopt);
        }},
      value);
  }


  auto err() & -> mark::optional<E> {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>&) { return mark::optional<E>(std::nullopt); },
        [](__result_detail::Err<E>& x) { return mark::optional<E>(x.e); }},
      value);
  }
  auto err() const& -> mark::optional<E> {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>&) { return mark::optional<E>(std::nullopt); },
        [](__result_detail::Err<E>& x) { return mark::optional<E>(x.e); }},
      value);
  }
  auto err() && -> mark::optional<E> {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>&) { return mark::optional<E>(std::nullopt); },
        [](__result_detail::Err<E>& x) { return mark::optional<E>(std::move(x.e)); }},
      value);
  }
  auto err() const && -> mark::optional<E> {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>&) { return mark::optional<E>(std::nullopt); },
        [](__result_detail::Err<E>& x) { return mark::optional<E>(std::move(x.e)); }},
      value);
  }

  auto unwrap() & noexcept(noexcept(std::get<__result_detail::Ok<T>>(value)))
    -> T& {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>& x) -> T& { return x.t; },
        [](__result_detail::Err<E>&) -> T& {
          throw(
            std::logic_error{"called `result::unwrap()` on an `Err` value"});
        }},
      value);
  }
  auto unwrap() const& noexcept(noexcept(std::get<__result_detail::Ok<T>>(value)))
    -> const T& {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>& x) -> const T& { return x.t; },
        [](const __result_detail::Err<E>&) -> const T& {
          throw(
            std::logic_error{"called `result::unwrap()` on an `Err` value"});
        }},
      value);
  }
  auto unwrap() && noexcept(noexcept(std::get<__result_detail::Ok<T>>(value)))
    -> T {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>& x) -> T { return std::move(x.t); },
        [](__result_detail::Err<E>&) -> T {
          throw(
            std::logic_error{"called `result::unwrap()` on an `Err` value"});
        }},
      value);
  }
  auto unwrap() const&& noexcept(noexcept(std::get<__result_detail::Ok<T>>(value)))
    -> T {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>& x) -> T { return std::move(x.t); },
        [](const __result_detail::Err<E>&) -> T {
          throw(
            std::logic_error{"called `result::unwrap()` on an `Err` value"});
        }},
      value);
  }

  auto unwrap_or(T&& optb) & noexcept -> T {
    return this->ok().value_or(std::forward<T>(optb));
  }
  auto unwrap_or(T&& optb) const& noexcept -> T {
    return this->ok().value_or(std::forward<T>(optb));
  }
  auto unwrap_or(T&& optb) && noexcept -> T {
    return std::move(*this).ok().value_or(std::forward<T>(optb));
  }
  auto unwrap_or(T&& optb) const&& noexcept -> T {
    return std::move(*this).ok().value_or(std::forward<T>(optb));
  }

  template <typename F>
  auto unwrap_or_else(F&& op) & noexcept(
    noexcept(std::forward<F>(op)(std::get<__result_detail::Err<E>>(value).e))) -> T {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>& x) { return x.t; },
        [&](__result_detail::Err<E>& x) -> T { return std::forward<F>(op)(x.e); }},
      value);
  }
  template <typename F>
  auto unwrap_or_else(F&& op) const& noexcept(
    noexcept(std::forward<F>(op)(std::get<__result_detail::Err<E>>(value).e))) -> T {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>& x) { return x.t; },
        [&](const __result_detail::Err<E>& x) -> T { return std::forward<F>(op)(x.e); }},
      value);
  }
  template <typename F>
  auto unwrap_or_else(F&& op) && noexcept(
    noexcept(std::forward<F>(op)(std::move(std::get<__result_detail::Err<E>>(value).e)))) -> T {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>& x) { return std::move(x.t); },
        [&](__result_detail::Err<E>& x) { return std::forward<F>(op)(std::move(x.e)); }},
      value);
  }
  template <typename F>
  auto unwrap_or_else(F&& op) const&& noexcept(
    noexcept(std::forward<F>(op)(std::move(std::get<__result_detail::Err<E>>(value).e)))) -> T {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>& x) { return std::move(x.t); },
        [&](const __result_detail::Err<E>& x) -> T { return std::forward<F>(op)(std::move(x.e)); }},
      value);
  }

  auto unwrap_err() & noexcept(noexcept(std::get<__result_detail::Err<E>>(value)))
    -> E {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>&) -> E {
          throw(
            std::logic_error{"called `result::unwrap_err()` on an `Ok` value"});
        },
        [](__result_detail::Err<E>& x) { return x.e; }},
      value);
  }
  auto unwrap_err() const& noexcept(noexcept(std::get<__result_detail::Err<E>>(value)))
    -> E {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>&) -> E {
          throw(
            std::logic_error{"called `result::unwrap_err()` on an `Ok` value"});
        },
        [](const __result_detail::Err<E>& x) { return x.e; }},
      value);
  }
  auto unwrap_err() && noexcept(noexcept(std::get<__result_detail::Err<E>>(value)))
    -> E {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>&) -> E {
          throw(
            std::logic_error{"called `result::unwrap_err()` on an `Ok` value"});
        },
        [](__result_detail::Err<E>& x) { return std::move(x.e); }},
      value);
  }
  auto unwrap_err() const&& noexcept(noexcept(std::get<__result_detail::Err<E>>(value)))
    -> E {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>&) -> E {
          throw(
            std::logic_error{"called `result::unwrap_err()` on an `Ok` value"});
        },
        [](const __result_detail::Err<E>& x) { return std::move(x.e); }},
      value);
  }

  template <typename F>
  auto unwrap_err_or_else(F&& op) & noexcept(
    noexcept(std::forward<F>(op)(std::get<__result_detail::Ok<T>>(value).t))) -> E {
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) -> E { return std::forward<F>(op)(x.t); },
        [](__result_detail::Err<E>& x) { return x.e; }},
      value);
  }
  template <typename F>
  auto unwrap_err_or_else(F&& op) const& noexcept(
    noexcept(std::forward<F>(op)(std::get<__result_detail::Ok<T>>(value).t))) -> E {
    return std::visit(
      overloaded{
        [&](const __result_detail::Ok<T>& x) -> E { return std::forward<F>(op)(x.t); },
        [](const __result_detail::Err<E>& x) { return x.e; }},
      value);
  }
  template <typename F>
  auto unwrap_err_or_else(F&& op) && noexcept(
    noexcept(std::forward<F>(op)(std::get<__result_detail::Ok<T>>(value).t))) -> E {
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) -> E { return std::forward<F>(op)(std::move(x.t)); },
        [](__result_detail::Err<E>& x) { return std::move(x.e); }},
      value);
  }
  template <typename F>
  auto unwrap_err_or_else(F&& op) const&& noexcept(
    noexcept(std::forward<F>(op)(std::get<__result_detail::Ok<T>>(value).t))) -> E {
    return std::visit(
      overloaded{
        [&](const __result_detail::Ok<T>& x) -> E { return std::forward<F>(op)(std::move(x.t)); },
        [](const __result_detail::Err<E>& x) { return std::move(x.e); }},
      value);
  }

  auto expect(const std::string& msg) & -> T {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>& x) { return x.t; },
        [&](__result_detail::Err<E>&) -> T { throw(std::logic_error(msg)); }},
      value);
  }
  auto expect(const std::string& msg) const& -> T {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>& x) { return x.t; },
        [&](const __result_detail::Err<E>&) -> T { throw(std::logic_error(msg)); }},
      value);
  }
  auto expect(const std::string& msg) && -> T {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>& x) { return std::move(x.t); },
        [&](__result_detail::Err<E>&) -> T { throw(std::logic_error(msg)); }},
      value);
  }
  auto expect(const std::string& msg) const&& -> T {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>& x) { return std::move(x.t); },
        [&](const __result_detail::Err<E>&) -> T { throw(std::logic_error(msg)); }},
      value);
  }

  auto as_ref() const -> result<const T&, const E&> {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>& x) {
          return result<const T&, const E&>::Ok(x.t);
        },
        [](const __result_detail::Err<E>& x) {
          return result<const T&, const E&>::Err(x.e);
        }},
      value);
  };

  auto as_mut() -> result<T&, E&> {
    return std::visit(
      overloaded{
        [](__result_detail::Ok<T>& x) { return result<T&, E&>::Ok(x.t); },
        [](__result_detail::Err<E>& x) { return result<T&, E&>::Err(x.e); }},
      value);
  }

  bool operator==(const result& other) const {
    return std::visit(
      overloaded{
        [this](const __result_detail::Ok<T>& x) { return *this == x; },
        [this](const __result_detail::Err<E>& x) { return *this == x; }},
      other.value);
  }

 private:
  bool operator==(const __result_detail::Ok<T>& ok) const {
    return std::visit(
      overloaded{
        [&ok](const __result_detail::Ok<T>& x) { return x == ok; },
        [](const __result_detail::Err<E>&) { return false; }},
      value);
  }

  bool operator==(const __result_detail::Err<E>& err) const {
    return std::visit(
      overloaded{
        [](const __result_detail::Ok<T>&) { return false; },
        [&err](const __result_detail::Err<E>& x) { return x == err; }},
      value);
  }



 private:
  template <typename Fn, typename _T, typename... Args>
  struct _map_traits {
    using invoke_result_type = std::invoke_result_t<Fn, _T, Args...>;
    using result_type =
      std::conditional_t<std::is_void_v<invoke_result_type>,
        result<std::tuple<>, E>,
        result<invoke_result_type, E>
      >;
  };

  template <typename Fn, typename... Args, typename Traits = _map_traits<Fn, Args...>>
  static auto _map_ok_invoke(Fn&& fn, Args&&... args) ->
    std::enable_if_t<
      !std::is_void_v<typename Traits::invoke_result_type>,
      typename Traits::result_type
    > {
    using _T = std::invoke_result_t<Fn, Args...>;
    return result<_T, E>::Ok(
      std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...)
    );
  }
  template <typename Fn, typename... Args, typename Traits = _map_traits<Fn, Args...>>
  static auto _map_ok_invoke(Fn&& fn, Args&&... args) ->
    std::enable_if_t<
      std::is_void_v<typename Traits::invoke_result_type>,
      typename Traits::result_type
    > {
    std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
    return result<std::tuple<>, E>::Ok();
  }

 public:
  template <typename Fn, typename... Args>
  constexpr auto map(Fn&& f, Args&&... args) & {
    using traits = _map_traits<Fn, T&, Args...>;
    using result_t = typename traits::result_type;

    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) {
          return _map_ok_invoke(std::forward<Fn>(f), x.t, std::forward<Args>(args)...);
        },
        [&](__result_detail::Err<E>& x) { return result_t::Err(x.e); }},
      value);
  }

  template <typename Fn, typename... Args>
  constexpr auto map(Fn&& f, Args&&... args) const & {
    using traits = _map_traits<Fn, const T&, Args...>;
    using result_t = typename traits::result_type;

    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) {
          return _map_ok_invoke(std::forward<Fn>(f), x.t, std::forward<Args>(args)...);
        },
        [&](__result_detail::Err<E>& x) { return result_t::Err(x.e); }},
      value);
  }

  template <typename Fn, typename... Args>
  constexpr auto map(Fn&& f, Args&&... args) && {
    using traits = _map_traits<Fn, T&&, Args...>;
    using result_t = typename traits::result_type;

    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) {
          return _map_ok_invoke(std::forward<Fn>(f), std::move(x.t), std::forward<Args>(args)...);
        },
        [&](__result_detail::Err<E>& x) { return result_t::Err(std::move(x.e)); }},
      value);
  }

  template <typename Fn, typename... Args>
  constexpr auto map(Fn&& f, Args&&... args) const && {
    using traits = _map_traits<Fn, const T&&, Args...>;
    using result_t = typename traits::result_type;

    return std::visit(
      overloaded{
        [&](auto&& x) {
          return _map_ok_invoke(std::forward<Fn>(f), std::move(x.t), std::forward<Args>(args)...);
        },
        [&](const __result_detail::Err<E>& x) { return result_t::Err(std::move(x.e)); }},
      value);
  }

  template <typename U, typename F>
  auto map_or(U&& _default, F&& op) &
    -> std::enable_if_t<std::is_invocable_v<F, T&>, std::invoke_result_t<F, T&>> {
    using _ReturnType = std::invoke_result_t<F, T&>;
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return std::forward<F>(op)(x.t); },
        [&](__result_detail::Err<E>& x) {
          return _ReturnType::Err(std::forward<U>(_default));
        }},
      value);
  }
  template <typename U, typename F>
  auto map_or(U&& _default, F&& op) const&
    -> std::enable_if_t<std::is_invocable_v<F, const T&>, std::invoke_result_t<F, const T&>> {
    using _ReturnType = std::invoke_result_t<F, const T&>;
    return std::visit(
      overloaded{
        [&](const __result_detail::Ok<T>& x) { return std::forward<F>(op)(x.t); },
        [&](const __result_detail::Err<E>& x) {
          return _ReturnType::Err(std::forward<U>(_default));
        }},
      value);
  }
  template <typename U, typename F>
  auto map_or(U&& _default, F&& op) &&
    -> std::enable_if_t<std::is_invocable_v<F, T&&>, std::invoke_result_t<F, T&&>> {
    using _ReturnType = std::invoke_result_t<F, T&&>;
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return std::forward<F>(op)(std::move(x.t)); },
        [&](__result_detail::Err<E>& x) {
          return _ReturnType::Err(std::forward<U>(_default));
        }},
      value);
  }
  template <typename U, typename F>
  auto map_or(U&& _default, F&& op) const &&
    -> std::enable_if_t<std::is_invocable_v<F, const T&&>, std::invoke_result_t<F, const T&&>> {
    using _ReturnType = std::invoke_result_t<F, const T&&>;
    return std::visit(
      overloaded{
        [&](const __result_detail::Ok<T>& x) { return std::forward<F>(op)(std::move(x.t)); },
        [&](const __result_detail::Err<E>& x) {
          return _ReturnType::Err(std::forward<U>(_default));
        }},
      value);
  }

  template <typename D, typename F, typename U = std::invoke_result_t<D, E>>
  auto map_or_else(D _default, F&& op)
    -> std::enable_if_t<std::is_invocable_r_v<U, F, T>, U> {
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return std::forward<F>(op)(x.t); },
        [&](__result_detail::Err<E>& x) { return _default(x.e); }},
      value);
  }

 private:
  template <typename Fn, typename _E, typename... Args>
  struct _map_err_traits {
    using invoke_result_type = std::invoke_result_t<Fn, _E, Args...>;
    using result_t =
      std::conditional_t<std::is_void_v<invoke_result_type>,
        result<T, std::tuple<>>,
        result<T, invoke_result_type>
      >;
  };

  template <typename Fn, typename... Args>
  auto _map_err_invoke(Fn&& fn, Args&&... args) ->
    std::enable_if_t<
      !std::is_void_v<std::invoke_result_t<Fn, Args...>>,
      result<T, std::invoke_result_t<Fn, Args...>>
    > {
    using _E = std::invoke_result_t<Fn, Args...>;
    return result<T, _E>::Err(
      std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...)
    );
  }
  template <typename Fn, typename... Args>
  auto _map_err_invoke(Fn&& fn, Args&&... args) ->
    std::enable_if_t<
      std::is_void_v<std::invoke_result_t<Fn, Args...>>,
      result<T, std::tuple<>>
    > {
    std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
    return result<T, std::tuple<>>::Err();
  }

 public:
  template <typename O, typename... Args>
  auto map_err(O&& op, Args&&... args) & {
    using traits = _map_err_traits<O, T&, Args...>;
    using result_t = typename traits::result_t;

    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return result_t::Ok(x.t); },
        [&](__result_detail::Err<E>& x) {
          return _map_err_invoke(std::forward<O>(op), x.e, std::forward<Args>(args)...);
        }},
      value);
  }
  template <typename O, typename... Args>
  auto map_err(O&& op, Args&&... args) const& {
    using traits = _map_err_traits<O, T&, Args...>;
    using result_t = typename traits::result_t;

    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return result_t::Ok(x.t); },
        [&](__result_detail::Err<E>& x) {
          return _map_err_invoke(std::forward<O>(op), x.e, std::forward<Args>(args)...);
        }},
      value);
  }
  template <typename O, typename... Args>
  auto map_err(O&& op, Args&&... args) && {
    using traits = _map_err_traits<O, T&, Args...>;
    using result_t = typename traits::result_t;

    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return result_t::Ok(std::move(x.t)); },
        [&](__result_detail::Err<E>& x) {
          return _map_err_invoke(std::forward<O>(op), std::move(x.e), std::forward<Args>(args)...);
        }},
      value);
  }
  template <typename O, typename... Args>
  auto map_err(O&& op, Args&&... args) const&& {
    using traits = _map_err_traits<O, T&, Args...>;
    using result_t = typename traits::result_t;

    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return result_t::Ok(std::move(x.t)); },
        [&](__result_detail::Err<E>& x) {
          return _map_err_invoke(std::forward<O>(op), std::move(x.e), std::forward<Args>(args)...);
        }},
      value);
  }

  template <typename U>
  auto and_(result<U, E> res) -> result<U, E> {
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>&) { return std::move(res); },
        [&](__result_detail::Err<E>& x) { return result<U, E>::Err(x.e); }},
      value);
  }

  template <typename F, typename... Args>
  auto and_then(F&& op, Args&&... args) & {
    using result_t = std::invoke_result_t<F, T&, Args...>;
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) {
          return std::invoke(
            std::forward<F>(op), x.t, std::forward<Args>(args)...);
        },
        [&](__result_detail::Err<E>& x) { return result_t::Err(x.e); }},
      value);
  }

  template <typename F, typename... Args>
  auto and_then(F&& op, Args&&... args) const& {
    using result_t = std::invoke_result_t<F, const T&, Args...>;
    return std::visit(
      overloaded{
        [&](const __result_detail::Ok<T>& x) {
          return std::invoke(std::forward<F>(op), x.t, std::forward<Args>(args)...);
        },
        [&](const __result_detail::Err<E>& x) { return result_t::Err(x.e); }},
      value);
  }

  template <typename F, typename... Args>
  auto and_then(F&& op, Args&&... args) && {
    using result_t = std::invoke_result_t<F, T&&, Args...>;
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>&& x) {
          return std::invoke(std::forward<F>(op), std::move(x.t), std::forward<Args>(args)...);
        },
        [&](__result_detail::Err<E>&& x) {
          return result_t::Err(std::move(x.e));
        }},
      std::move(value));
  }

  template <typename F, typename... Args>
  auto and_then(F&& op, Args&&... args) const&& {
    using result_t = std::invoke_result_t<F, const T&&, Args...>;
    return std::visit(
      overloaded{
        [&](const __result_detail::Ok<T>&& x) {
          return std::invoke(std::forward<F>(op), std::move(x.t), std::forward<Args>(args)...);
        },
        [&](const __result_detail::Err<E>&& x) {
          return result_t::Err(std::move(x.e));
        }},
      std::move(value));
  }

  template <typename F>
  auto do_or(result<T, F> res) -> result<T, F> {
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return result<T, F>::Ok(x.t); },
        [&](__result_detail::Err<E>&) { return res; }},
      value);
  }

  template <
    typename O, typename F = __error_type_of_t<std::invoke_result_t<O, E>>>
  auto or_else(O&& op) -> std::enable_if_t<
    __has_same_ok_v<result, std::invoke_result_t<O, E>>, result<T, F>> {
    return std::visit(
      overloaded{
        [&](__result_detail::Ok<T>& x) { return result<T, F>::Ok(x.t); },
        [&](__result_detail::Err<E>& x) { return std::forward<O>(op)(x.e); }},
      value);
  }

  template <typename F, typename... Args>
  result& inspect(F&& op, Args&&... args) & {
    match(
      [&](const auto& v) { std::forward<F>(op)(v, std::forward<Args>(args)...); },
      [&](...) {}
    );
    return *this;
  }

  template <typename F, typename... Args>
  const result& inspect(F&& op, Args&&... args) const& {
    match(
      [&](const auto& v) { std::forward<F>(op)(v, std::forward<Args>(args)...); },
      [&](...) {}
    );
    return *this;
  }

  template <typename F, typename... Args>
  result inspect(F&& op, Args&&... args) && {
    std::visit(
      overloaded{
        [&](const __result_detail::Ok<T>& x) { std::forward<F>(op)(x.t, std::forward<Args>(args)...); },
        [&](...) {}},
      value);
    return std::move(*this);
  }

  template <typename F, typename... Args>
  const result inspect(F&& op, Args&&... args) const&& {
    std::visit(
      overloaded{
        [&](const __result_detail::Ok<T>& x) { std::forward<F>(op)(x.t, std::forward<Args>(args)...); },
        [&](...) {}},
      value);
    return std::move(*this);
  }

  template <typename F, typename... Args>
  constexpr result& inspect_err(F&& op, Args&&... args) & {
    std::visit(
      overloaded{
        [&](const auto&) {},
        [&](const __result_detail::Err<E>& x) { return std::forward<F>(op)(x.e, std::forward<Args>(args)...); }},
      value);
    return *this;
  }

  template <typename F, typename... Args>
  constexpr const result& inspect_err(F&& op, Args&&... args) const& {
    std::visit(
      overloaded{
        [&](const auto&) {},
        [&](const __result_detail::Err<E>& x) { return std::forward<F>(op)(x.e, std::forward<Args>(args)...); }},
      value);
    return *this;
  }

  template <typename F, typename... Args>
  constexpr result inspect_err(F&& op, Args&&... args) && {
    std::visit(
      overloaded{
        [&](const auto&) {},
        [&](const __result_detail::Err<E>& x) { return std::forward<F>(op)(x.e, std::forward<Args>(args)...); }},
      value);
    return std::move(*this);
  }

  template <typename F, typename... Args>
  constexpr const result inspect_err(F&& op, Args&&... args) const&& {
    std::visit(
      overloaded{
        [&](const auto&) {},
        [&](const __result_detail::Err<E>& x) { return std::forward<F>(op)(x.e, std::forward<Args>(args)...); }},
      value);
    return std::move(*this);
  }

  template <typename _T>
  struct __can_flatten : std::false_type {};

  template <typename _T, typename _E>
  struct __can_flatten<result<result<_T, _E>, _E>> : std::true_type {};

  // convert from `result<result<T, E>>` to `result<T, E>`
  auto flatten() & {
    static_assert(__can_flatten<result<T, E>>::value);
    return match(
      [](auto&& x) { return x; },
      [](auto&& e) { return T::Err(e); }
    );
  }
  auto flatten() const & {
    static_assert(__can_flatten<result<T, E>>::value);
    return match(
      [](auto&& x) { return x; },
      [](auto&& e) { return T::Err(e); }
    );
  }
  auto flatten() && {
    static_assert(__can_flatten<result<T, E>>::value);
    return match(
      [](auto&& x) { return std::move(x); },
      [](auto&& e) { return T::Err(std::move(e)); }
    );
  }
  auto flatten() const && {
    static_assert(__can_flatten<result<T, E>>::value);
    return match(
      [](auto&& x) { return std::move(x); },
      [](auto&& e) { return T::Err(std::move(e)); }
    );
  }
};

}  // namespace mark

#endif  // !MARK_RESULT_RESULT_HPP_
