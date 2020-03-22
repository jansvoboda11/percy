#ifndef PERCY_VARIADIC_UNION
#define PERCY_VARIADIC_UNION

#include "type_traits.hpp"

#include <utility>

namespace percy {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// VARIADIC_UNION INTERFACE ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

namespace detail {
template <typename T>
constexpr void move(variadic_union<T>& dst, variadic_union<T>&& src, std::size_t index) {
  dst.head_ = std::forward<T>(src.head_);
}

template <typename T>
constexpr void copy(variadic_union<T>& dst, const variadic_union<T>& src, std::size_t index) {
  dst.head_ = src.head_;
}
} // namespace detail

template <typename T>
union variadic_union<T> {
  T head_;

  constexpr explicit variadic_union(const T& value) : head_(value) {}

  constexpr explicit variadic_union(T&& value) : head_(std::forward<T>(value)) {}

  constexpr variadic_union(variadic_union<T>&& other, std::size_t index) {
    detail::move(*this, std::forward<variadic_union<T>>(other), index);
  }

  constexpr variadic_union(const variadic_union<T>& other, std::size_t index) {
    detail::copy(*this, other, index);
  }

  constexpr ~variadic_union() {}
};

template <typename T>
constexpr void destroy(variadic_union<T>& u, std::size_t index) {
  u.head_.~T();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// MULTIPLE ELEMENTS IMPLEMENTATION ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail {
template <typename T1, typename T2, typename... Ts>
constexpr void move(variadic_union<T1, T2, Ts...>& dst, variadic_union<T1, T2, Ts...>&& src, std::size_t index) {
  if (index == 0) {
    dst.head_ = std::forward<T1>(src.head_);
  } else {
    move(dst.tail_, std::forward<variadic_union<T2, Ts...>>(src.tail_), index - 1);
  }
}

template <typename T1, typename T2, typename... Ts>
constexpr void copy(variadic_union<T1, T2, Ts...>& dst, const variadic_union<T1, T2, Ts...>& src, std::size_t index) {
  if (index == 0) {
    dst.head_ = src.head_;
  } else {
    copy(dst.tail_, src.tail_, index - 1);
  }
}
} // namespace detail

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
    detail::move(*this, std::forward<variadic_union<T1, T2, Ts...>>(other), index);
  }

  constexpr variadic_union(const variadic_union<T1, T2, Ts...>& other, std::size_t index) {
    detail::copy(*this, other, index);
  }

  constexpr ~variadic_union() {}
};

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
