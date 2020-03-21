#ifndef PERCY_VARIADIC_UNION
#define PERCY_VARIADIC_UNION

#include "type_traits.hpp"

#include <utility>

namespace percy {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// GENERIC DECLARATION /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Ts>
union variadic_union;

template <typename... Ts>
constexpr void move(variadic_union<Ts...>& target, variadic_union<Ts...>&& source, std::size_t index);

template <typename... Ts>
constexpr void copy(variadic_union<Ts...>& target, const variadic_union<Ts...>& source, std::size_t index);

template <typename... Ts>
constexpr void destroy(std::size_t index);

template <typename U, typename... Ts, std::enable_if_t<contains_v<U, Ts...>, int> = 0>
constexpr const U& get(const variadic_union<Ts...>& u);

template <typename U, typename... Ts, std::enable_if_t<contains_v<U, Ts...>, int> = 0>
constexpr U& get(variadic_union<Ts...>& u);

template <typename U, typename... Ts, std::enable_if_t<contains_v<U, Ts...>, int> = 0>
constexpr void set(variadic_union<Ts...>& u, U&& value);

template <typename U, typename... Ts, std::enable_if_t<contains_v<U, Ts...>, int> = 0>
constexpr void set(variadic_union<Ts...>& u, const U& value);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// SINGLE ELEMENT ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
union variadic_union<T> {
  T last_;

  template <typename V, std::enable_if_t<std::is_same_v<V, T>, int> = 0>
  constexpr explicit variadic_union(const V& value) : last_(value) {}

  template <typename V, std::enable_if_t<std::is_same_v<V, T>, int> = 0>
  constexpr explicit variadic_union(V&& value) : last_(std::forward<V>(value)) {}

  constexpr variadic_union(variadic_union<T>&& other, std::size_t index) {
    move(*this, std::forward<variadic_union<T>>(other), index);
  }

  constexpr variadic_union(const variadic_union<T>& other, std::size_t index) {
    copy(*this, other, index);
  }

  constexpr ~variadic_union() {}
};

template <typename X>
constexpr void move(variadic_union<X>& target, variadic_union<X>&& source, std::size_t index) {
  target.last_ = std::forward<X>(source.last_);
}

template <typename X>
constexpr void copy(variadic_union<X>& target, const variadic_union<X>& source, std::size_t index) {
  target.last_ = source.last_;
}

template <typename X>
constexpr void destroy(variadic_union<X>& u, std::size_t index) {
  u.last_.~X();
}

template <typename Y, typename X, std::enable_if_t<std::is_same_v<Y, X>, int> = 0>
constexpr const Y& get(const variadic_union<X>& u) {
  return u.last_;
}

template <typename Y, typename X, std::enable_if_t<std::is_same_v<Y, X>, int> = 0>
constexpr Y& get(variadic_union<X>& u) {
  return u.last_;
}

template <typename U, typename X, std::enable_if_t<std::is_same_v<U, X>, int> = 0>
constexpr void set(variadic_union<X>& u, U&& value) {
  u.last_ = std::forward<U>(value);
}

template <typename Y, typename X, std::enable_if_t<std::is_same_v<Y, X>, int> = 0>
constexpr void set(variadic_union<X>& u, const Y& value) {
  u.last_ = value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// MULTIPLE ELEMENTS //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, typename... Ts>
union variadic_union<T, Ts...> {
  static_assert(are_unique_v<T, Ts...>, "Each type in the variadic union must be unique.");

  T head_;
  variadic_union<Ts...> tail_;

  template <typename V, std::enable_if_t<std::is_same_v<V, T>, int> = 0>
  constexpr explicit variadic_union(const V& value) : head_(value) {}

  template <typename V, std::enable_if_t<std::is_same_v<V, T>, int> = 0>
  constexpr explicit variadic_union(V&& value) : head_(std::forward<V>(value)) {}

  template <typename V, std::enable_if_t<contains_v<V, Ts...>, int> = 0>
  constexpr explicit variadic_union(const V& value) : tail_(value) {}

  template <typename V, std::enable_if_t<contains_v<V, Ts...>, int> = 0>
  constexpr explicit variadic_union(V&& value) : tail_(std::forward<V>(value)) {}

  constexpr variadic_union(variadic_union<T, Ts...>&& other, std::size_t index) {
    move(*this, std::forward<variadic_union<T, Ts...>>(other), index);
  }

  constexpr variadic_union(const variadic_union<T, Ts...>& other, std::size_t index) {
    copy(*this, other, index);
  }

  constexpr ~variadic_union() {}
};

template <typename X, typename... Xs>
constexpr void move(variadic_union<X, Xs...>& target, variadic_union<X, Xs...>&& source, std::size_t index) {
  if (index == 0) {
    target.head_ = std::forward<X>(source.head_);
  } else {
    move(target.tail_, std::forward<variadic_union<Xs...>>(source.tail_), index - 1);
  }
}

template <typename X, typename... Xs>
constexpr void copy(variadic_union<X, Xs...>& target, const variadic_union<X, Xs...>& source, std::size_t index) {
  if (index == 0) {
    target.head_ = source.head_;
  } else {
    copy(target.tail_, source.tail_, index - 1);
  }
}

template <typename X, typename... Xs>
constexpr void destroy(variadic_union<X, Xs...>& u, std::size_t index) {
  if (index == 0) {
    u.head_.~X();
  } else {
    destroy(u.tail_, index - 1);
  }
}

template <typename Y, typename X, typename... Xs, std::enable_if_t<contains_v<Y, X, Xs...>, int> = 0>
constexpr const Y& get(const variadic_union<X, Xs...>& u) {
  if constexpr (std::is_same_v<Y, X>) {
    return u.head_;
  } else {
    return get<Y>(u.tail_);
  }
}

template <typename Y, typename X, typename... Xs, std::enable_if_t<contains_v<Y, X, Xs...>, int> = 0>
constexpr Y& get(variadic_union<X, Xs...>& u) {
  if constexpr (std::is_same_v<Y, X>) {
    return u.head_;
  } else {
    return get<Y>(u.tail_);
  }
}

template <typename Y, typename X, typename... Xs, std::enable_if_t<contains_v<Y, X, Xs...>, int> = 0>
constexpr void set(variadic_union<X, Xs...>& u, Y&& value) {
  if constexpr (std::is_same_v<Y, X>) {
    u.head_ = std::forward<Y>(value);
  } else {
    set<Y>(u.tail_, std::forward<Y>(value));
  }
}

template <typename Y, typename X, typename... Xs, std::enable_if_t<contains_v<Y, X, Xs...>, int> = 0>
constexpr void set(variadic_union<X, Xs...>& u, const Y& value) {
  if constexpr (std::is_same_v<Y, X>) {
    u.head_ = value;
  } else {
    set<Y>(u.tail_, value);
  }
}
} // namespace percy

#endif
