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

template <typename Input, typename Rule>
struct parser_result {
  using type = std::invoke_result_t<decltype(parser<Rule>::template parse<Input>), Input>;
};

template <typename Input, typename Rule>
using parser_result_t = typename parser_result<Input, Rule>::type;

template <typename F>
struct function_return;

template <typename R, typename ...As>
struct function_return<R(As...)> {
  using type = R;
};

template <typename Rule>
struct action_return {
  using type = typename function_return<decltype(Rule::action)>::type;
};

template <typename Rule>
using action_return_t = typename action_return<Rule>::type;

template <typename A, typename B>
struct result_followed_by {};

template <typename A, typename ...Bs>
struct result_followed_by<result<A>, result<std::tuple<Bs...>>> {
  using type = result<std::tuple<A, Bs...>>;
};

template <typename A, typename B>
using result_followed_by_t = typename result_followed_by<A, B>::type;
}

#endif
