#ifndef PERCY_PARSER
#define PERCY_PARSER

#include <percy/result.hpp>
#include <percy/rules.hpp>
#include <percy/type_traits.hpp>
#include <percy/utils.hpp>

#include <tuple>
#include <variant>
#include <vector>

namespace percy {
template <typename Rule>
struct parser {
  using result_type = result<action_return_t<Rule>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto raw_result = parser<typename Rule::rule>::parse(input);

    if (raw_result.is_failure()) {
      return result_type::failure(raw_result.span());
    }

    return result_type::success(Rule::action(raw_result), raw_result.span());
  }
};

template <>
struct parser<end> {
  using result_type = result<std::true_type>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    if (!input.ended()) {
      return result_type::failure({input.position(), input.position()});
    }

    return result_type::success(std::true_type(), {input.position(), input.position()});
  }
};

template <char Symbol>
struct parser<symbol<Symbol>> {
  using result_type = result<char>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    if (input.ended()) {
      return result_type::failure({input.position(), input.position()});
    }

    if (input.peek() != Symbol) {
      return result_type::failure({input.position(), input.position() + 1});
    }

    return result_type::success(Symbol, {input.position(), input.position() + 1});
  }
};

template <>
struct parser<sequence<>> {
  using result_type = result<std::tuple<>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    using tuple_type = result_value_t<result_type>;

    return result_type::success(tuple_type(), {input.position(), input.position()});
  }
};

template <typename Rule, typename ...FollowingRules>
struct parser<sequence<Rule, FollowingRules...>> {
  using result_type = result<std::tuple<result_value_t<parser_result_t<Rule>>,
                                        result_value_t<parser_result_t<FollowingRules>>...>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto result = parser<Rule>::parse(input);

    if (result.is_failure()) {
      return result_type::failure(result.span());
    }

    auto result_following = parser<sequence<FollowingRules...>>::parse(input.advanced_to(result.end()));

    if (result_following.is_failure()) {
      return result_type::failure({input.position(), result_following.end()});
    }

    return result.followed_by(result_following);
  }
};

template <typename Rule>
struct parser<one_of<Rule>> {
  using result_type = result<std::variant<result_value_t<parser_result_t<Rule>>>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    using variant_type = result_value_t<result_type>;

    auto result = parser<Rule>::parse(input);

    if (result.is_failure()) {
      return result_type::failure(result.span());
    }

    auto value = variant_type(result.get());
    return result_type::success(value, result.span());
  }
};

template <typename Rule, typename AlternativeRule, typename ...AlternativeRules>
struct parser<one_of<Rule, AlternativeRule, AlternativeRules...>> {
  using result_type = result<std::variant<result_value_t<parser_result_t<Rule>>,
                                          result_value_t<parser_result_t<AlternativeRule>>,
                                          result_value_t<parser_result_t<AlternativeRules>>...>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    if (auto result = parser<one_of<Rule>>::parse(input)) {
      auto value = append_types<result_value_t<parser_result_t<AlternativeRule>>,
                                result_value_t<parser_result_t<AlternativeRules>>...>(result.get());
      return result_type::success(value, result.span());
    }

    if (auto alternative_result = parser<one_of<AlternativeRule, AlternativeRules...>>::parse(input)) {
      auto value = prepend_types<result_value_t<parser_result_t<Rule>>>(alternative_result.get());
      return result_type::success(value, alternative_result.span());
    }

    return result_type::failure({input.position(), input.position()});
  }
};

template <typename Rule>
struct parser<repeat<Rule>> {
  using result_type = result<std::vector<result_value_t<parser_result_t<Rule>>>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
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
}

#endif
