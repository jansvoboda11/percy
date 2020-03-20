#ifndef PERCY_VARIANT
#define PERCY_VARIANT

#include "type_traits.hpp"

namespace percy {
template <typename U, typename... Ts>
struct contains;

template <typename U>
struct contains<U> {
  constexpr static bool value = false;
};

template <typename U, typename T, typename... Ts>
struct contains<U, T, Ts...> {
  constexpr static bool value = std::is_same_v<U, T> || contains<U, Ts...>::value;
};

template <typename U, typename... Ts>
constexpr inline bool contains_v = contains<U, Ts...>::value;

template <std::size_t N, typename... Ts>
struct nth;

template <typename T, typename... Ts>
struct nth<0, T, Ts...> {
  using type = T;
};

template <std::size_t N, typename T, typename... Ts>
struct nth<N, T, Ts...> {
  using type = typename nth<N - 1, Ts...>::type;
};

template <std::size_t N, typename... Ts>
using nth_t = typename nth<N, Ts...>::type;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// variant_union /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct valueless {
  constexpr explicit valueless() {}
};

template <std::size_t Index>
struct variant_index {};

template <typename... Ts>
union variant_union;

template <typename T>
union variant_union<T> {
private:
  T value_;

public:
  template <std::size_t Index, typename... Xs, typename>
  friend constexpr const nth_t<Index, Xs...>& get(const variant_union<Xs...>&);

  template <typename V, std::size_t Index, std::enable_if_t<Index == 0, int> = 0>
  constexpr variant_union(V value, variant_index<Index>) : value_(value) {}

  constexpr void copy(const variant_union<T>& other, std::size_t index) {
    value_ = other.value_;
  }

  constexpr void destroy(std::size_t index) {
    value_.~T();
  }

  constexpr ~variant_union() {}
};

template <typename T, typename U, typename... Us>
union variant_union<T, U, Us...> {
private:
  T value_;
  variant_union<U, Us...> rest_;

public:
  template <std::size_t Index, typename... Xs, typename>
  friend constexpr const nth_t<Index, Xs...>& get(const variant_union<Xs...>&);

  template <typename V, std::size_t Index, std::enable_if_t<Index == 0, int> = 0>
  constexpr variant_union(V value, variant_index<Index>) : value_(value) {}

  template <typename V, std::size_t Index, std::enable_if_t<Index != 0, int> = 0>
  constexpr variant_union(V value, variant_index<Index>) : rest_(value, variant_index<Index - 1>{}) {}

  constexpr void copy(const variant_union<T, U, Us...>& other, std::size_t index) {
    if (index == 0) {
      static_assert(std::is_same_v<decltype(value_), decltype(other.value_)>);
      value_ = other.value_;
    } else {
      rest_.copy(other.rest_, index - 1);
    }
  }

  constexpr void destroy(std::size_t index) {
    if (index == 0) {
//      value_.~T();
    } else {
      rest_.destroy(index - 1);
    }
  }

  constexpr ~variant_union() {}
};

// clang-format off
template <std::size_t Index, typename... Xs, typename = std::enable_if_t<Index < sizeof...(Xs)>>
constexpr const nth_t<Index, Xs...>& get(const variant_union<Xs...>& u) {
  if constexpr (Index == 0) {
    return u.value_;
  } else {
    return get<Index - 1>(u.rest_);
  }
}
// clang-format on

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// variant ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Ts>
class variant {
  static_assert(are_unique_v<Ts...>, "The `variant` type requires each type argument to be unique.");

  variant_union<Ts..., valueless> union_;
  std::size_t index_;

public:
  template <std::size_t Index, typename... Xs, typename>
  friend constexpr const nth_t<Index, Xs...>& get(const variant<Xs...>&);

  template <typename V, typename = std::enable_if_t<contains_v<V, Ts...>>>
  constexpr explicit variant(V value) : variant(value, variant_index<index_of_v<V, Ts...>>{}) {}

  constexpr variant(const variant<Ts...>& other)
      : union_(valueless{}, variant_index<sizeof...(Ts)>{}), index_(other.index_) {
    union_.copy(other.union_, other.index_);
  }

  constexpr variant<Ts...>& operator=(const variant<Ts...>& other) {
    union_.copy(other.union_, other.index_);
    index_ = other.index_;
    return *this;
  }

  constexpr std::size_t index() const {
    return index_;
  }

  constexpr ~variant() {
    union_.destroy(index_);
  }

private:
  template <typename V, std::size_t Index, typename = std::enable_if_t<std::is_same_v<V, nth_t<Index, Ts...>>>>
  constexpr variant(V value, variant_index<Index>) : union_(value, variant_index<Index>{}), index_(Index) {}
};

// clang-format off
template <std::size_t Index, typename... Xs, typename = std::enable_if_t<Index < sizeof...(Xs)>>
constexpr const nth_t<Index, Xs...>& get(const variant<Xs...>& v) {
  return get<Index>(v.union_);
}
// clang-format on

template <typename U, typename... Xs, typename = std::enable_if_t<contains_v<U, Xs...>>>
constexpr const U& get(const variant<Xs...>& v) {
  return get<index_of_v<U, Xs...>>(v);
}

template <typename U, typename... Xs, typename = std::enable_if_t<contains_v<U, Xs...>>>
constexpr bool holds_alternative(const variant<Xs...>& v) {
  return v.index() == index_of_v<U, Xs...>;
}
} // namespace percy

#endif
