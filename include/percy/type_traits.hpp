#ifndef PERCY_TYPE_TRAITS
#define PERCY_TYPE_TRAITS

#include <type_traits>

namespace percy {
/// Forward declaration.
template <typename Rule>
struct parser;

/// The value type of a result type.
template <typename Result>
using result_value_t = typename Result::value_type;

/// The result type of a rule.
template <typename Rule>
using parser_result_t = typename parser<Rule>::result_type;

/// The return type of a function.
template <typename F>
struct function_return;

template <typename R, typename... As>
struct function_return<R(As...)> {
  using type = R;
};

template <typename Rule>
struct action_return {
  using type = typename function_return<decltype(Rule::action)>::type;
};

/// The return type of a rule with custom action.
template <typename Rule>
using action_return_t = typename action_return<Rule>::type;

template <typename... Ts>
struct are_identical_impl;

template <typename T>
struct are_identical_impl<T> {
  constexpr static bool value = true;
};

template <typename T, typename... Ts>
struct are_identical_impl<T, Ts...> {
  constexpr static bool value = std::conjunction_v<std::is_same<T, Ts>...>;
};

/// Determines whether all types `Ts...` are identical.
template <typename... Ts>
inline constexpr bool are_identical_v = are_identical_impl<Ts...>::value;

/// Returns the type at index `Index`.
template <std::size_t Index, typename... Ts>
using nth_t = std::tuple_element_t<Index, std::tuple<Ts...>>;

/// The algorithm iterates over a list of types `Ts...` of size `Size` with two indices `Left` and `Right`.
/// The inner iteration compares the type at index `Left` with each type between index `Left + 1` and `Size - 1`
/// (indexed by `Right`). The outer iteration sets the index `Left` to values from `0` to `Size - 1`.
template <std::size_t Left, std::size_t Right, std::size_t Size, typename... Ts>
struct are_distinct_impl;

/// The terminating condition of the outer iteration: `Left == Size - 1`.
/// Returning `true` here ensures that a list with only a single type is considered distinct.
template <std::size_t Left, std::size_t Right, typename... Ts>
struct are_distinct_impl<Left, Right, Left + 1, Ts...> {
  constexpr static bool value = true;
};

/// The terminating condition of the inner iteration: `Right == Size - 1`.
/// The algorithm continues with new inner iteration with incremented `Left`.
template <std::size_t Left, std::size_t Right, typename... Ts>
struct are_distinct_impl<Left, Right, Right + 1, Ts...> {
  constexpr static bool value = !std::is_same_v<nth_t<Left, Ts...>, nth_t<Right, Ts...>> &&
                                are_distinct_impl<Left + 1, Left + 2, Right + 1, Ts...>::value;
};

/// The common case where `Left < Size - 1 && Right < Size - 1`.
template <std::size_t Left, std::size_t Right, std::size_t Size, typename... Ts>
struct are_distinct_impl {
  constexpr static bool value = !std::is_same_v<nth_t<Left, Ts...>, nth_t<Right, Ts...>> &&
                                are_distinct_impl<Left, Right + 1, Size, Ts...>::value;
};

/// Determines whether all types `Ts...` are distinct.
template <typename... Ts>
inline constexpr bool are_distinct_v = are_distinct_impl<0, 1, sizeof...(Ts), Ts...>::value;
} // namespace percy

#endif
