#ifndef PERCY_TYPE_TRAITS
#define PERCY_TYPE_TRAITS

#include <tuple>
#include <type_traits>
#include <vector>

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
} // namespace percy

#endif
