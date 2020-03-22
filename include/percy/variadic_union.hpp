#ifndef PERCY_VARIADIC_UNION
#define PERCY_VARIADIC_UNION

#include "type_traits.hpp"

#include <utility>

namespace percy {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// VARIADIC_UNION INTERFACE ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Type-generic variadic union.
template <typename... Ts>
union variadic_union {
  /// Forwarding element constructor.
  template <typename E, std::enable_if_t<contains_v<std::remove_cvref_t<E>, Ts...>, int> = 0>
  constexpr explicit variadic_union(E&& element);

  /// Move constructor.
  constexpr variadic_union(variadic_union&& other, std::size_t index);

  /// Copy constructor.
  constexpr variadic_union(const variadic_union& other, std::size_t index);

  /// Destructor. Does nothing, see `destroy` instead.
  constexpr ~variadic_union();
};

/// The actual destructor. Must be called manually with the index of the currently active element.
template <typename... Ts>
constexpr void destroy(std::size_t index);

/// Type-based element accessor.
template <typename E, typename... Ts, std::enable_if_t<contains_v<E, Ts...>, int> = 0>
constexpr const E& get(const variadic_union<Ts...>& u);

/// Type-based element accessor.
template <typename E, typename... Ts, std::enable_if_t<contains_v<E, Ts...>, int> = 0>
constexpr E& get(variadic_union<Ts...>& u);

/// Forwarding element assignment.
template <typename E, typename... Ts, std::enable_if_t<contains_v<E, Ts...>, int> = 0>
constexpr void set(variadic_union<Ts...>& u, E&& element);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// SINGLE ELEMENT IMPLEMENTATION /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail {
template <typename T>
constexpr void move(variadic_union<T>& dst, variadic_union<T>&& src, std::size_t index) {
  dst.head_ = std::move(src.head_);
}

template <typename T>
constexpr void copy(variadic_union<T>& dst, const variadic_union<T>& src, std::size_t index) {
  dst.head_ = src.head_;
}
} // namespace detail

template <typename T>
union variadic_union<T> {
  T head_;

  template <typename E, std::enable_if_t<std::is_same_v<std::remove_cvref_t<E>, T>, int> = 0>
  constexpr explicit variadic_union(E&& element) : head_(std::forward<E>(element)) {}

  constexpr variadic_union(variadic_union&& other, std::size_t index) {
    detail::move(*this, std::move(other), index);
  }

  constexpr variadic_union(const variadic_union& other, std::size_t index) {
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
    dst.head_ = std::move(src.head_);
  } else {
    move(dst.tail_, std::move(src.tail_), index - 1);
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

  template <typename E, std::enable_if_t<std::is_same_v<std::remove_cvref_t<E>, T1>, int> = 0>
  constexpr explicit variadic_union(E&& element) : head_(std::forward<E>(element)) {}

  template <typename E, std::enable_if_t<contains_v<std::remove_cvref_t<E>, T2, Ts...>, int> = 0>
  constexpr explicit variadic_union(E&& element) : tail_(std::forward<E>(element)) {}

  constexpr variadic_union(variadic_union&& other, std::size_t index) {
    detail::move(*this, std::move(other), index);
  }

  constexpr variadic_union(const variadic_union& other, std::size_t index) {
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

template <typename E, typename T, typename... Ts, std::enable_if_t<contains_v<E, T, Ts...>, int> = 0>
constexpr const E& get(const variadic_union<T, Ts...>& u) {
  if constexpr (std::is_same_v<E, T>) {
    return u.head_;
  } else {
    return get<E>(u.tail_);
  }
}

template <typename E, typename T, typename... Ts, std::enable_if_t<contains_v<E, T, Ts...>, int> = 0>
constexpr E& get(variadic_union<T, Ts...>& u) {
  if constexpr (std::is_same_v<E, T>) {
    return u.head_;
  } else {
    return get<E>(u.tail_);
  }
}

template <typename E, typename T, typename... Ts, std::enable_if_t<contains_v<std::remove_cvref_t<E>, T, Ts...>, int> = 0>
constexpr void set(variadic_union<T, Ts...>& u, E&& element) {
  if constexpr (std::is_same_v<std::remove_cvref_t<E>, T>) {
    u.head_ = std::forward<E>(element);
  } else {
    set<E>(u.tail_, std::forward<E>(element));
  }
}
} // namespace percy

#endif
