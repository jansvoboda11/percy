#ifndef PERCY_TYPE_TRAITS
#define PERCY_TYPE_TRAITS

#include <type_traits>

namespace percy {
// Forward declaration.
template <typename Rule, typename... AlternativeRules>
struct one_of;

template <typename Subject>
struct is_one_of {
  constexpr static bool value = false;
};

template <typename Rule, typename... AlternativeRules>
struct is_one_of<one_of<Rule, AlternativeRules...>> {
  constexpr static bool value = true;
};

/// Determines whether Subject is one_of.
template <typename Subject>
constexpr inline bool is_one_of_v = is_one_of<Subject>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////

// Forward declaration.
template <typename Rule, typename... FollowingRules>
struct sequence;

template <typename Subject>
struct is_sequence {
  constexpr static bool value = false;
};

template <typename Rule, typename... FollowingRules>
struct is_sequence<sequence<Rule, FollowingRules...>> {
  constexpr static bool value = true;
};

/// Determines whether Subject is sequence.
template <typename Subject>
constexpr inline bool is_sequence_v = is_sequence<Subject>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////

/// The value type of a result type.
template <typename Result>
using result_value_t = typename Result::value_type;

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Forward declaration.
template <typename Rule, typename Enabled>
struct parser;

/// The result type of a rule.
template <typename Rule>
using parser_result_t = typename parser<Rule, typename std::enable_if_t<true>>::result_type;

////////////////////////////////////////////////////////////////////////////////////////////////////

/// The return type of a function.
template <typename F>
struct function_return;

template <typename R, typename... As>
struct function_return<R(As...)> {
  using type = R;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Rule>
struct action_return {
  using type = typename function_return<decltype(Rule::action)>::type;
};

/// The return type of a rule with custom action.
template <typename Rule>
using action_return_t = typename action_return<Rule>::type;

////////////////////////////////////////////////////////////////////////////////////////////////////

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

/// Determines whether all types in Ts are the same.
template <typename... Ts>
inline constexpr bool are_same_v = are_same_impl<Ts...>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Ts>
struct are_unique;

template <typename T>
struct are_unique<T> {
  constexpr static bool value = true;
};

template <typename T1, typename T2, typename... Ts>
struct are_unique<T1, T2, Ts...> {
  constexpr static bool value =
      !std::is_same_v<T1, T2> && are_unique<T1, Ts...>::value && are_unique<T2, Ts...>::value;
};

/// Determines whether all types in Ts are unique.
template <typename... Ts>
inline constexpr bool are_unique_v = are_unique<Ts...>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename U, typename... Ts>
struct type_index;

template <typename U, typename... Ts>
struct type_index<U, U, Ts...> : std::integral_constant<std::size_t, 0> {};

template <typename U, typename T, typename... Ts>
struct type_index<U, T, Ts...>
    : std::integral_constant<std::size_t, 1 + type_index<U, Ts...>::value> {};

/// The index of type U in the list Ts.
template <typename U, typename... Ts>
constexpr inline std::size_t type_index_v = type_index<U, Ts...>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////

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

/// Determines whether the list Ts contains the type U.
template <typename U, typename... Ts>
constexpr inline bool has_same_v = has_same<U, Ts...>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <std::size_t Index, typename... Ts>
struct at_index;

template <typename T, typename... Ts>
struct at_index<0, T, Ts...> {
  using type = T;
};

template <std::size_t Index, typename T, typename... Ts>
struct at_index<Index, T, Ts...> {
  using type = typename at_index<Index - 1, Ts...>::type;
};

/// The type at given index in the type list Ts.
template <std::size_t Index, typename... Ts>
using at_index_t = typename at_index<Index, Ts...>::type;
} // namespace percy

#endif
