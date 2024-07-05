
//
//   ____    ____        _        _______      ___  ____
//  |_   \  /   _|      / \      |_   __ \    |_  ||_  _|
//    |   \/   |       / _ \       | |__) |     | |_/ /
//    | |\  /| |      / ___ \      |  __ /      |  __'.
//   _| |_\/_| |_   _/ /   \ \_   _| |  \ \_   _| |  \ \_
//  |_____||_____| |____| |____| |____| |___| |____||____|
//
// Author: Zhengyu
// Date: 09/19/2020 03:14
//

#ifndef MARK_ALIAS_TYPE_TYPE_CONTAINER_LIST_HPP_
#define MARK_ALIAS_TYPE_TYPE_CONTAINER_LIST_HPP_

#include <list>
#include <algorithm>

#include "mark/alias_type/alias_type.hpp"
#include "mark/alias_type/traits/traits.hpp"
#include "mark/alias_type/alias_type_trait.hpp"
#include "mark/alias_type/type/container/iter.hpp"

namespace mark {

template <typename T, auto TypeID, template<typename , typename = void> class... Traits>
using __original_list_t = mark::alias_type<std::list<T>, TypeID, Traits...>;

namespace traits {

template <typename AliasType, typename = void>
struct ListImpl;

template <typename T, auto TypeID, template<typename , typename = void> class... Traits>
struct ListImpl<__original_list_t<T, TypeID, Traits...>>
  : mark::traits::Self<__original_list_t<T, TypeID, Traits...>, ListImpl> {

  using __raw_list_t = std::list<T>;
  using iterator = typename __raw_list_t::iterator;
  using const_iterator = typename __raw_list_t::const_iterator;
  using size_type = typename __raw_list_t::size_type;
  using value_type = typename __raw_list_t::value_type;

  using __list_type_t = __original_list_t<T, TypeID, Traits...>;
  using mark::traits::Self<__list_type_t, ::mark::traits::ListImpl>::self;

  template <typename... Args>
  auto emplace(const_iterator pos, Args&&... args ) {
    return this->self().value().emplace(pos, std::forward<Args>(args)...);
  }

  template <typename... Args>
  auto emplace_back(Args&&... args) {
    return this->self().value().emplace_back(std::forward<Args>(args)...);
  }

  void push_back(const T& value) {
    this->self().value().push_back(value);
  }
  void push_back(T&& value) {
    this->self().value().push_back(value);
  }

  void resize(size_type count) {
    this->self().value().resize(count);
  }
  void resize(size_type count, T value = T()) {
    this->self().value().resize(count, value);
  }
  void resize(size_type count, const value_type& value) {
    this->self().value().resize(count, value);
  }
  void swap(__list_type_t& other) {
    this->self().value().swap(other.value());
  }
  void merge(__list_type_t& other) {
    this->self().value().merge(other.value());
  }
  void merge(__list_type_t&& other ) {
    this->self().value().merge(std::move(other.value()));
  }
  template <class Compare>
  void merge(__list_type_t& other, Compare comp) {
    this->self().value().merge(other.value(), comp);
  }
  template <class Compare>
  void merge(__list_type_t&& other, Compare comp) {
    this->self().value().merge(std::move(other.value()), comp);
  }
  void splice(const_iterator pos, __list_type_t& other) {
    this->self().value().splice(pos, other.value());
  }
  void splice(const_iterator pos, __list_type_t&& other) {
    this->self().value().splice(pos, std::move(other.value()));
  }
  void splice(const_iterator pos, __list_type_t& other, const_iterator it) {
    this->self().value().splice(pos, other.value(), it);
  }
  void splice(const_iterator pos, __list_type_t&& other, const_iterator it) {
    this->self().value().splice(pos, std::move(other.value()), it);
  }
  void splice(const_iterator pos, __list_type_t& other,
              const_iterator first, const_iterator last) {
    this->self().value().splice(pos, other.value(), first, last);
  }
  void splice(const_iterator pos, __list_type_t&& other,
              const_iterator first, const_iterator last ) {
    this->self().value().splice(pos, std::move(other.value()), first, last);
  }

  void remove(const T& value) {
    this->self().value().remove(value);
  }
  template<typename UnaryPredicate>
  void remove_if(UnaryPredicate p) {
    this->self().value().remove(std::forward<UnaryPredicate>(p));
  }

  template<typename BinaryPredicate>
  void unique(BinaryPredicate p) {
    this->self().value().unique(std::forward<BinaryPredicate>(p));
  }

  template<typename Compare>
  void sort(Compare comp) {
    this->self().value().sort(std::forward<Compare>(comp));
  }

#define __NO_ARGUMENT_METHOD(method) \
  auto method() { return this->self().value().method(); }

  __NO_ARGUMENT_METHOD(begin)
  __NO_ARGUMENT_METHOD(end)
  __NO_ARGUMENT_METHOD(front)
  __NO_ARGUMENT_METHOD(back)
  __NO_ARGUMENT_METHOD(pop_front)
  __NO_ARGUMENT_METHOD(reverse)
  __NO_ARGUMENT_METHOD(unique)
  __NO_ARGUMENT_METHOD(sort)

#undef __NO_ARGUMENT_METHOD

#define __NO_ARGUMENT_CONST_METHOD(method) \
  auto method() const { return this->self().value().method(); }

  __NO_ARGUMENT_CONST_METHOD(size)
  __NO_ARGUMENT_CONST_METHOD(empty)
  __NO_ARGUMENT_CONST_METHOD(begin)
  __NO_ARGUMENT_CONST_METHOD(end)
  __NO_ARGUMENT_CONST_METHOD(cbegin)
  __NO_ARGUMENT_CONST_METHOD(cend)
  __NO_ARGUMENT_CONST_METHOD(front)
  __NO_ARGUMENT_CONST_METHOD(back)

#undef __NO_ARGUMENT_CONST_METHOD

  Iter<iterator> iter() {
    return Iter<iterator> {
      {},
      this->begin(),
      this->end()
    };
  }

  Iter<const_iterator> iter() const {
    return Iter<iterator> {
      {},
      this->begin(),
      this->end()
    };
  }
};
}  // namespace traits

template <typename T, auto TypeID, template <typename, typename = void> class... Traits>
using alias_list_t = mark::default_alias_type<std::list<T>, TypeID, mark::traits::ListImpl, Traits...>;

}  // namespace mark

#endif  // !MARK_ALIAS_TYPE_TYPE_CONTAINER_LIST_HPP_
