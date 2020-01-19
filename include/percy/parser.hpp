#ifndef PERCY_PARSER
#define PERCY_PARSER

#include <percy/result.hpp>
#include <percy/rules.hpp>
#include <percy/type_traits.hpp>

#include <tuple>
#include <variant>
#include <vector>

namespace percy {
template <typename Rule>
struct parser {
  template <typename Input>
  constexpr static auto parse(Input input) {
    using result_type = result<action_return_t<Rule>>;

    auto raw_result = parser<typename Rule::rule>::parse(input);

    if (raw_result.is_failure()) {
      return result_type::failure(raw_result.span());
    }

    return result_type::success(Rule::action(raw_result), raw_result.span());
  }
};

template <>
struct parser<end> {
  template <typename Input>
  constexpr static auto parse(Input input) {
    using result_type = result<std::true_type>;

    if (!input.ended()) {
      return result_type::failure({input.position(), input.position()});
    }

    return result_type::success(std::true_type(), {input.position(), input.position()});
  }
};

template <char Symbol>
struct parser<symbol<Symbol>> {
  template <typename Input>
  constexpr static auto parse(Input input) {
    using result_type = result<char>;

    if (input.ended()) {
      return result_type::failure({input.position(), input.position()});
    }

    if (input.peek() != Symbol) {
      return result_type::failure({input.position(), input.position() + 1});
    }

    return result_type::success(Symbol, {input.position(), input.position() + 1});
  }
};

template <typename Rule>
struct parser<sequence<Rule>> {
  template <typename Input>
  constexpr static auto parse(Input input) {
    using result_type = result<std::tuple<result_value_t<parser_result_t<Input, Rule>>>>;

    auto result = parser<Rule>::parse(input);

    if (result.is_failure()) {
      return result_type::failure(result.span());
    }

    return result_type::success(result.get(), result.span());
  }
};

template <typename Rule, typename FollowingRule, typename ...FollowingRules>
struct parser<sequence<Rule, FollowingRule, FollowingRules...>> {
  template <typename Input>
  constexpr static auto parse(Input input) {
    using result_type = result_followed_by_t<parser_result_t<Input, Rule>,
                                             parser_result_t<Input, sequence<FollowingRule, FollowingRules...>>>;

    auto result = parser<Rule>::parse(input);

    if (result.is_failure()) {
      return result_type::failure(result.span());
    }

    auto result_following = parser<sequence<FollowingRule, FollowingRules...>>::parse(input.advanced_to(result.end()));

    if (result_following.is_failure()) {
      return result_type::failure({input.position(), result_following.end()});
    }

    return result.followed_by(result_following);
  }
};

template <typename Rule>
struct parser<repeat<Rule>> {
  template <typename Input>
  constexpr static auto parse(Input input) {
    using result_type = result<std::vector<result_value_t<parser_result_t<Input, Rule>>>>;
    using vector_type = result_value_t<result_type>;

    vector_type values;
    auto input_current = input;

    while (auto result = parser<Rule>::parse(input_current)) {
      auto value = result.get();
      values.push_back(value);
      input_current = input.advanced_after(result);
    }

    return result_type::success(values, {input.position(), input_current.position()});
  }
};

template <typename ResultType, std::size_t Index, typename Input, typename ...Rules>
struct one_of_parser {};

template <typename ResultType, std::size_t Index, typename Input, typename Rule>
struct one_of_parser<ResultType, Index, Input, Rule> {
  constexpr static auto parse(Input input) {
    using variant_type = result_value_t<ResultType>;

    auto result = parser<Rule>::parse(input);

    if (result.is_failure()) {
      return ResultType::failure(result.span());
    }

    auto value = variant_type(std::in_place_index<Index>, result.get());
    return ResultType::success(value, result.span());
  }
};

template <typename ResultType, std::size_t Index, typename Input, typename Rule, typename AlternativeRule, typename ...AlternativeRules>
struct one_of_parser<ResultType, Index, Input, Rule, AlternativeRule, AlternativeRules...> {
  constexpr static auto parse(Input input) {
    using variant_type = result_value_t<ResultType>;

    if (auto result = one_of_parser<ResultType, Index, Input, Rule>::parse(input)) {
      return result;
    }

    return one_of_parser<ResultType, Index + 1, Input, AlternativeRule, AlternativeRules...>::parse(input);
  }
};

template <typename ...Rules>
struct parser<one_of<Rules...>> {
  template <typename Input>
  constexpr static auto parse(Input input) {
    using result_type = result<std::variant<result_value_t<parser_result_t<Input, Rules>>...>>;

    return one_of_parser<result_type, 0, Input, Rules...>::parse(input);
  }
};

template <typename Rule>
struct parser<unwrap<Rule>> {
  template <typename Input>
  constexpr static auto parse(Input input) {
    auto result = parser<Rule>::parse(input);
    return result.get();
  }
};
}

#endif
