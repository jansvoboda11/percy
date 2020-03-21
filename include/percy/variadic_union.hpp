#ifndef PERCY_VARIADIC_UNION
#define PERCY_VARIADIC_UNION

#include "type_traits.hpp"

#include <utility>

namespace percy {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// VARIADIC_UNION INTERFACE ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// todo: Decide if: * `move` and `copy` should be moved to `percy::detail` namespace,
//                  * `destroy` should become a member function,
//                  * `get` and `set` should become member functions.

template <typename... Ts>
union variadic_union {
  template <typename T, std::enable_if_t<contains_v<T, Ts...>, int> = 0>
  constexpr explicit variadic_union(const T& value);

  template <typename T, std::enable_if_t<contains_v<T, Ts...>, int> = 0>
  constexpr explicit variadic_union(T&& value);

  constexpr variadic_union(variadic_union<Ts...>&& other, std::size_t index);

  constexpr variadic_union(const variadic_union<Ts...>& other, std::size_t index);

  constexpr ~variadic_union();
};

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
//////////////////////////////////////////// SINGLE ELEMENT IMPLEMENTATION /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
union variadic_union<T> {
  T head_;

  constexpr explicit variadic_union(const T& value) : head_(value) {}

  constexpr explicit variadic_union(T&& value) : head_(std::forward<T>(value)) {}

  constexpr variadic_union(variadic_union<T>&& other, std::size_t index) {
    move(*this, std::forward<variadic_union<T>>(other), index);
  }

  constexpr variadic_union(const variadic_union<T>& other, std::size_t index) {
    copy(*this, other, index);
  }

  constexpr ~variadic_union() {}
};

template <typename T>
constexpr void move(variadic_union<T>& target, variadic_union<T>&& source, std::size_t index) {
  target.head_ = std::forward<T>(source.head_);
}

template <typename T>
constexpr void copy(variadic_union<T>& target, const variadic_union<T>& source, std::size_t index) {
  target.head_ = source.head_;
}

template <typename T>
constexpr void destroy(variadic_union<T>& u, std::size_t index) {
  u.head_.~T();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// MULTIPLE ELEMENTS IMPLEMENTATION ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T1, typename T2, typename... Ts>
union variadic_union<T1, T2, Ts...> {
  static_assert(are_unique_v<T1, T2, Ts...>, "Each type in the variadic union must be unique.");

  T1 head_;
  variadic_union<T2, Ts...> tail_;

  constexpr explicit variadic_union(const T1& value) : head_(value) {}

  constexpr explicit variadic_union(T1&& value) : head_(std::forward<T1>(value)) {}

  template <typename V, std::enable_if_t<contains_v<V, T2, Ts...>, int> = 0>
  constexpr explicit variadic_union(const V& value) : tail_(value) {}

  template <typename V, std::enable_if_t<contains_v<V, T2, Ts...>, int> = 0>
  constexpr explicit variadic_union(V&& value) : tail_(std::forward<V>(value)) {}

  constexpr variadic_union(variadic_union<T1, T2, Ts...>&& other, std::size_t index) {
    move(*this, std::forward<variadic_union<T1, T2, Ts...>>(other), index);
  }

  constexpr variadic_union(const variadic_union<T1, T2, Ts...>& other, std::size_t index) {
    copy(*this, other, index);
  }

  constexpr ~variadic_union() {}
};

template <typename T1, typename T2, typename... Ts>
constexpr void move(variadic_union<T1, T2, Ts...>& target, variadic_union<T1, T2, Ts...>&& source, std::size_t index) {
  if (index == 0) {
    target.head_ = std::forward<T1>(source.head_);
  } else {
    move(target.tail_, std::forward<variadic_union<T2, Ts...>>(source.tail_), index - 1);
  }
}

template <typename T1, typename T2, typename... Ts>
constexpr void copy(variadic_union<T1, T2, Ts...>& target, const variadic_union<T1, T2, Ts...>& source, std::size_t index) {
  if (index == 0) {
    target.head_ = source.head_;
  } else {
    copy(target.tail_, source.tail_, index - 1);
  }
}

template <typename T1, typename T2, typename... Ts>
constexpr void destroy(variadic_union<T1, T2, Ts...>& u, std::size_t index) {
  if (index == 0) {
    u.head_.~T1();
  } else {
    destroy(u.tail_, index - 1);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// SHARED IMPLEMENTATION /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename U, typename T, typename... Ts, std::enable_if_t<contains_v<U, T, Ts...>, int> = 0>
constexpr const U& get(const variadic_union<T, Ts...>& u) {
  if constexpr (std::is_same_v<U, T>) {
    return u.head_;
  } else {
    return get<U>(u.tail_);
  }
}

template <typename U, typename T, typename... Ts, std::enable_if_t<contains_v<U, T, Ts...>, int> = 0>
constexpr U& get(variadic_union<T, Ts...>& u) {
  if constexpr (std::is_same_v<U, T>) {
    return u.head_;
  } else {
    return get<U>(u.tail_);
  }
}

template <typename U, typename T, typename... Ts, std::enable_if_t<contains_v<U, T, Ts...>, int> = 0>
constexpr void set(variadic_union<T, Ts...>& u, U&& value) {
  if constexpr (std::is_same_v<U, T>) {
    u.head_ = std::forward<U>(value);
  } else {
    set<U>(u.tail_, std::forward<U>(value));
  }
}

template <typename U, typename T, typename... Ts, std::enable_if_t<contains_v<U, T, Ts...>, int> = 0>
constexpr void set(variadic_union<T, Ts...>& u, const U& value) {
  if constexpr (std::is_same_v<U, T>) {
    u.head_ = value;
  } else {
    set<U>(u.tail_, value);
  }
}
} // namespace percy

#endif
