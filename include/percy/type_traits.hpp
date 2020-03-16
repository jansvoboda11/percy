#ifndef PERCY_TYPE_TRAITS
#define PERCY_TYPE_TRAITS

#include <type_traits>

namespace percy {
template <typename Rule>
struct parser;

template <typename Result>
using result_value_t = typename Result::value_type;

template <typename Rule>
using parser_result_t = typename parser<Rule>::result_type;

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

template <typename Rule>
using action_return_t = typename action_return<Rule>::type;

template <typename Type, typename... Types>
struct are_same {
  constexpr static bool value = std::conjunction_v<std::is_same<Type, Types>...>;
};

template <typename Type, typename... Types>
inline constexpr bool are_same_v = are_same<Type, Types...>::value;
} // namespace percy

#endif
