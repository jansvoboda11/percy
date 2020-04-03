#ifndef PERCY_TYPE_TRAITS
#define PERCY_TYPE_TRAITS

#include <type_traits>

namespace percy {
// Forward declaration.
template <typename Rule, typename... AlternativeRules>
struct one_of;

template <typename T>
struct is_one_of {
  constexpr static bool value = false;
};

template <typename... Alternatives>
struct is_one_of<one_of<Alternatives...>> {
  constexpr static bool value = true;
};

template <typename T>
constexpr inline bool is_one_of_v = is_one_of<T>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Forward declaration.
template <typename Rule, typename... FollowingRules>
struct sequence;

template <typename T>
struct is_sequence {
  constexpr static bool value = false;
};

template <typename... Rules>
struct is_sequence<sequence<Rules...>> {
  constexpr static bool value = true;
};

template <typename T>
constexpr inline bool is_sequence_v = is_sequence<T>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// The value type of a result type.
template <typename Result>
using result_value_t = typename Result::value_type;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Forward declaration.
template <typename Rule, typename Enabled>
struct parser;

/// The result type of a rule.
template <typename Rule>
using parser_result_t = typename parser<Rule, typename std::enable_if_t<true>>::result_type;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// The return type of a function.
template <typename F>
struct function_return;

template <typename R, typename... As>
struct function_return<R(As...)> {
  using type = R;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Rule>
struct action_return {
  using type = typename function_return<decltype(Rule::action)>::type;
};

/// The return type of a rule with custom action.
template <typename Rule>
using action_return_t = typename action_return<Rule>::type;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Ts>
struct are_same_impl;

template <typename T>
struct are_same_impl<T> {
  constexpr static bool value = true;
};

template <typename T, typename... Ts>
struct are_same_impl<T, Ts...> {
  constexpr static bool value = std::conjunction_v<std::is_same<T, Ts>...>;
};

/// Determines whether all types in `Ts...` are the same.
template <typename... Ts>
inline constexpr bool are_same_v = are_same_impl<Ts...>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Ts>
struct are_unique;

template <typename T>
struct are_unique<T> {
  constexpr static bool value = true;
};

template <typename T1, typename T2, typename... Ts>
struct are_unique<T1, T2, Ts...> {
  constexpr static bool value = !std::is_same_v<T1, T2> && are_unique<T1, Ts...>::value && are_unique<T2, Ts...>::value;
};

/// Determines whether all types in `Ts...` are unique.
template <typename... Ts>
inline constexpr bool are_unique_v = are_unique<Ts...>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename U, typename... Ts>
struct index_of;

template <typename U, typename... Ts>
struct index_of<U, U, Ts...> : std::integral_constant<std::size_t, 0> {};

template <typename U, typename T, typename... Ts>
struct index_of<U, T, Ts...> : std::integral_constant<std::size_t, 1 + index_of<U, Ts...>::value> {};

/// The index of the type `U` in the list `Ts...`.
template <typename U, typename... Ts>
constexpr inline std::size_t index_of_v = index_of<U, Ts...>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename U, typename... Ts>
struct has_same;

template <typename U>
struct has_same<U> {
  constexpr static bool value = false;
};

template <typename U, typename T, typename... Ts>
struct has_same<U, T, Ts...> {
  constexpr static bool value = std::is_same_v<U, T> || has_same<U, Ts...>::value;
};

/// Determines whether the list `Ts...` contains the type `U`.
template <typename U, typename... Ts>
constexpr inline bool has_same_v = has_same<U, Ts...>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/// The N-th type in list `Ts...`.
template <std::size_t N, typename... Ts>
using nth_t = typename nth<N, Ts...>::type;
} // namespace percy

#endif
