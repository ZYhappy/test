
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/14/2020 6:48:54
//

#ifndef MARK_ALIAS_TYPE_ALIAS_TYPE_BASE_HPP_
#define MARK_ALIAS_TYPE_ALIAS_TYPE_BASE_HPP_

#include <tuple>
#include <type_traits>
#include "mark/utility.hpp"
#include "mark/alias_type/traits/traits.hpp"

namespace mark {

namespace detail {

template <typename T, bool = std::is_trivially_destructible_v<T>>
struct __alias_type_destruct_base;

template <typename T>
struct __alias_type_destruct_base<T, false> {
  typedef T value_type;

  union {
    value_type __value;
  };

  static_assert(std::is_object_v<value_type>,
                "instantiation of alias_type with a non-object type is undefined behavior");

  // 对于非平凡析构的类型，需要显式调用析构函数
  ~__alias_type_destruct_base() {
    __value.~value_type();
  }

  constexpr __alias_type_destruct_base() noexcept {}

  template <typename... Args>
  constexpr explicit __alias_type_destruct_base(in_place_t, Args&&... __args)
    : __value(std::forward<Args>(__args)...) {}
};

template <typename T>
struct __alias_type_destruct_base<T, true> {
  typedef T value_type;

  union {
    value_type __value;
  };

  static_assert(std::is_object_v<value_type>,
                "instantiation of alias_type with a non-object type is undefined behavior");

  constexpr __alias_type_destruct_base() noexcept {}

  template <typename... Args>
  constexpr explicit __alias_type_destruct_base(in_place_t, Args&&... __args)
    : __value(std::forward<Args>(__args)...) {}
};

template<typename T, bool = std::is_reference_v<T>>
struct __alias_type_storage_base;

template<typename T>
struct __alias_type_storage_base<T, false> : __alias_type_destruct_base<T> {
  typedef T value_type;
  using __base = __alias_type_destruct_base<T>;
  using __base::__base;

  constexpr value_type& __get()& noexcept {
    return this->__value;
  }

  constexpr const value_type& __get() const& noexcept {
    return this->__value;
  }

  constexpr value_type&& __get()&& noexcept {
    return std::move(this->__value);
  }

  constexpr const value_type&& __get() const&& noexcept {
    return std::move(this->__value);
  }

  template<typename... Args>
  void __construct(Args... args) {
    new((void*)std::addressof(this->__value)) value_type(std::forward<Args>(args)...);
  }

  template<typename OtherAliasType>
  void __construct_from(OtherAliasType&& other) {
    __construct(std::forward<OtherAliasType>(other).__get());
  }

  template<typename OtherAliasType>
  void __assign_from(OtherAliasType&& other) {
    this->__value = std::forward<OtherAliasType>(other).__get();
  }

};

template <class T>
struct __alias_type_storage_base<T, true> {
  using value_type = T;
  using __raw_type = std::remove_reference_t<T>;
  __raw_type *__value;

  template<class U>
  static constexpr bool __can_bind_reference() {
    using __raw_u = typename std::remove_reference<U>::type;
    using __u_ptr = __raw_u *;
    using __raw_t = typename std::remove_reference<T>::type;
    using _t_ptr = __raw_t *;
    using __check_lvalue_arg = std::integral_constant<bool,
      (std::is_lvalue_reference<U>::value && std::is_convertible<__u_ptr, _t_ptr>::value)
      || std::is_same<__raw_u, std::reference_wrapper<__raw_t>>::value
      || std::is_same<__raw_u, std::reference_wrapper<typename std::remove_const<__raw_t>::type>>::value
    >;
    return (std::is_lvalue_reference<T>::value && __check_lvalue_arg::value)
           || (std::is_rvalue_reference<T>::value && !std::is_lvalue_reference<U>::value &&
               std::is_convertible<__u_ptr, _t_ptr>::value);
  }

  constexpr __alias_type_storage_base() noexcept = delete;

  template<typename U>
  constexpr explicit __alias_type_storage_base(in_place_t, U &&arg)
    :  __value(std::addressof(arg)) {
    static_assert(__can_bind_reference<U>(),
                  "Attempted to construct a reference element in tuple from a "
                  "possible temporary");
  }

  constexpr value_type &__get() const & noexcept { return *__value; }

  constexpr value_type &&__get() const && noexcept { return std::forward<value_type>(*__value); }

  template<typename U>
  void __construct(U &&__val) {
    static_assert(__can_bind_reference<U>(),
                  "Attempted to construct a reference element in tuple from a "
                  "possible temporary");
    __value = std::addressof(__val);
  }

  template<typename _That>
  void __construct_from(_That &&__opt) {
    __construct(std::forward<_That>(__opt).__get());
  }

  template<typename _That>
  void __assign_from(_That &&__opt) {
    *__value = std::forward<_That>(__opt).__get();
  }
};

template<typename T, bool = std::is_trivially_copy_constructible_v<T>>
struct __alias_type_copy_construct_base : __alias_type_storage_base<T> {
  using __alias_type_storage_base<T>::__alias_type_storage_base;
};

template<typename T>
struct __alias_type_copy_construct_base<T, false> : __alias_type_storage_base<T> {
  using __alias_type_storage_base<T>::__alias_type_storage_base;
  __alias_type_copy_construct_base() = default;
  __alias_type_copy_construct_base(const __alias_type_copy_construct_base& __alias) {
    this->__construct_from(__alias);
  }

  __alias_type_copy_construct_base(__alias_type_copy_construct_base&&) = default;
  __alias_type_copy_construct_base& operator=(const __alias_type_copy_construct_base&) = default;
  __alias_type_copy_construct_base& operator=(__alias_type_copy_construct_base&&) = default;
};

template<typename T, bool = std::is_trivially_move_constructible_v<T>>
struct __alias_type_move_construct_base : __alias_type_copy_construct_base<T> {
  using __alias_type_copy_construct_base<T>::__alias_type_copy_construct_base;
};

template<typename T>
struct __alias_type_move_construct_base<T, false> : __alias_type_copy_construct_base<T> {
  using value_type = T;

  using __alias_type_copy_construct_base<T>::__alias_type_copy_construct_base;

  __alias_type_move_construct_base() = default;
  __alias_type_move_construct_base(const __alias_type_move_construct_base&) = default;

  __alias_type_move_construct_base(__alias_type_move_construct_base&& __alias)
  noexcept(std::is_nothrow_move_constructible_v<value_type>) {
    this->__construct_from(std::move(__alias));
  }

  __alias_type_move_construct_base& operator=(const __alias_type_move_construct_base&) = default;
  __alias_type_move_construct_base& operator=(__alias_type_move_construct_base&&) = default;
};

template<typename T, bool = std::is_trivially_copy_assignable_v<T>>
struct __alias_type_copy_assignable_base : __alias_type_move_construct_base<T> {
  using __alias_type_move_construct_base<T>::__alias_type_move_construct_base;
};

template<typename T>
struct __alias_type_copy_assignable_base<T, false> : __alias_type_move_construct_base<T> {
  using __alias_type_move_construct_base<T>::__alias_type_move_construct_base;

  __alias_type_copy_assignable_base() = default;
  __alias_type_copy_assignable_base(const __alias_type_copy_assignable_base&) = default;
  __alias_type_copy_assignable_base(__alias_type_copy_assignable_base&&) = default;

  __alias_type_copy_assignable_base& operator=(const __alias_type_copy_assignable_base& other) {
    this->__assign_from(other);
    return *this;
  }
  __alias_type_copy_assignable_base& operator=(__alias_type_copy_assignable_base&&) = default;
};

template<typename T, bool = std::is_trivially_copy_assignable_v<T>>
struct __alias_type_move_assignable_base : __alias_type_copy_assignable_base<T> {
  using __alias_type_copy_assignable_base<T>::__alias_type_copy_assignable_base;
};

template<typename T>
struct __alias_type_move_assignable_base<T, false> : __alias_type_copy_assignable_base<T> {
  using value_type = T;
  using __alias_type_copy_assignable_base<T>::__alias_type_copy_assignable_base;

  __alias_type_move_assignable_base() = default;
  __alias_type_move_assignable_base(const __alias_type_move_assignable_base&) = default;
  __alias_type_move_assignable_base(__alias_type_move_assignable_base&&) = default;
  __alias_type_move_assignable_base& operator=(const __alias_type_move_assignable_base&) = default;

  __alias_type_move_assignable_base& operator=(__alias_type_move_assignable_base&& other) {
    this->__assign_from(std::move(other));
    return *this;
  }
};
}

template <class _Tp>
using __alias_type_sfinae_ctor_base_t = __sfinae_ctor_base<
  std::is_copy_constructible<_Tp>::value,
  std::is_move_constructible<_Tp>::value
>;

template <class _Tp>
using __alias_type_sfinae_assign_base_t = __sfinae_assign_base<
  (std::is_copy_constructible<_Tp>::value && std::is_copy_assignable<_Tp>::value),
  (std::is_move_constructible<_Tp>::value && std::is_move_assignable<_Tp>::value)
>;

template<typename T, auto TypeID>
class __alias_type_base :
  private detail::__alias_type_move_assignable_base<T>,
  private __alias_type_sfinae_ctor_base_t<T>,
  private __alias_type_sfinae_assign_base_t<T> {
 public:
  using value_type = T;
  static constexpr auto type_id = TypeID;
  using base = detail::__alias_type_move_assignable_base<T>;

 public:
  constexpr __alias_type_base() noexcept : base() {};
  constexpr __alias_type_base(const __alias_type_base&) = default;
  constexpr __alias_type_base(__alias_type_base&&) = default;

  template<typename... Args,
    typename = typename std::enable_if<std::is_constructible<value_type, Args...>::value>::type>
  constexpr explicit __alias_type_base(in_place_t, Args&&... args)
    : base(in_place, std::forward<Args>(args)...) {};

  template<typename Up, typename... Args,
    typename = typename std::enable_if<std::is_constructible<value_type, std::initializer_list<Up>&, Args...>::value>::type>
  constexpr explicit __alias_type_base(in_place_t, std::initializer_list<Up> il, Args&&... args)
    : base(in_place, il, std::forward<Args>(args)...) {};

 public:
  using base::__get;

  constexpr const value_type& operator*() const& {
    return this->__get();
  }
  constexpr value_type& operator*() & {
    return this->__get();
  }
  constexpr const value_type&& operator*() const&& {
    return std::move(this->__get());
  }
  constexpr value_type&& operator*() && {
    return std::move(this->__get());
  }
  constexpr value_type const& value() const&{
    return this->__get();
  }
  constexpr value_type& value() &{
    return this->__get();
  }
  constexpr value_type&& value() &&{
    return std::move(this->__get());
  }
  constexpr value_type const&& value() const&&{
    return std::move(this->__get());
  }

  __alias_type_base& operator=(const __alias_type_base&) = default;
  __alias_type_base& operator=(__alias_type_base&&) = default;
};

template<typename T, auto TypeID,
  template<typename...> class... Traits>
class alias_type;

namespace traits {

template<typename T, template<typename...> class... Traits>
struct traits_driver : Traits<T, void> ... {};

}

template<typename T, auto TypeID,
  template<typename...> class... Traits>
class alias_type :
  public __alias_type_base<T, TypeID>,
  public Traits<alias_type<T, TypeID, Traits...>>... {

  static_assert(__all<std::is_empty_v<Traits<alias_type>>...>::value, "trait must be empty");

  using base = __alias_type_base<T, TypeID>;

 public:
  using value_type = T;
  static constexpr auto type_id = TypeID;

 public:
  constexpr alias_type() noexcept : base(in_place) {};
  constexpr alias_type(const alias_type&) = default;
  constexpr alias_type(alias_type&&)  noexcept = default;

  template<typename... Args,
    typename = typename std::enable_if<std::is_constructible<value_type, Args...>::value>::type>
  constexpr explicit alias_type(in_place_t, Args&&... args)
    : base(in_place, std::forward<Args>(args)...) {};

  template<typename Up, typename... Args,
    typename = typename std::enable_if<std::is_constructible<value_type, std::initializer_list<Up>&, Args...>::value>::type>
  constexpr explicit alias_type(in_place_t, std::initializer_list<Up> il, Args&&... args)
    : base(in_place, il, std::forward<Args>(args)...) {};

 public:
  template<typename Up = value_type, std::enable_if_t<
    check_args_ctor<alias_type, T, Up>::template _enable_implicit<Up>::value
    , int> = 0>
  constexpr alias_type(Up&& v)
    : base(in_place, std::forward<Up>(v)) {}

  template<typename Up, std::enable_if_t<
    check_args_ctor<alias_type, T, Up>::template _enable_explicit<Up>::value
    , int> = 0>
  constexpr explicit alias_type(Up&& v)
    : base(in_place, std::forward<Up>(v)) {}

 public:
  using base::__get;

  alias_type& operator=(const alias_type&) = default;
  alias_type& operator=(alias_type&&) noexcept = default;

  template <typename U = value_type,
    typename = std::enable_if_t<
      __lazy_and<
        std::integral_constant<bool,
          !std::is_same_v<remove_cvref_t<U>, alias_type> &&
          !(std::is_same_v<U, value_type> && std::is_scalar_v<value_type>)>,
        std::is_constructible<value_type, U>,
        std::is_assignable<value_type&, U>>::value>>
  alias_type& operator=(U&& v) {
    this->__get() = std::forward<U>(v);
    return *this;
  }

  template <template<typename, typename = void> class... _Traits>
  operator alias_type<T, TypeID, _Traits...>&() {
    return static_cast<alias_type<T, TypeID, _Traits...>&>(static_cast<__alias_type_base<T, TypeID>&>(*this));
  }


 public:
  // TODO curry化处理
  // template<typename Mem, typename ...Args>
  // auto operator|(Mem mem) {
  //   return [this, mem] (Args&&... args)  {
  //     return mem(std::addressof(this->__get()), std::forward<Args>(args)...);
  //   };
  // }
};

}  // namespace mark

#endif  // !MARK_ALIAS_TYPE_ALIAS_TYPE_BASE_HPP_
