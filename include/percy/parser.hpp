#ifndef PERCY_PARSER
#define PERCY_PARSER

#include "percy/result.hpp"
#include "percy/rules.hpp"

#include <percy/variant.hpp>

#include <string_view>
#include <tuple>
#include <vector>

namespace percy {
template <typename Rule, typename Enabled = void>
struct parser {
  using result_type = result<action_return_t<Rule>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto raw_result = parser<typename Rule::rule>::parse(input);

    if (raw_result.is_failure()) {
      return raw_result.failure();
    }

    return succeed(Rule::action(raw_result), raw_result->span());
  }
};

template <typename Rule>
struct parser<Rule, typename std::enable_if_t<is_one_of_v<typename Rule::rule>>> {
  using result_type = result<typename Rule::result>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto raw_result = parser<typename Rule::rule>::parse(input);

    if (raw_result.is_failure()) {
      return raw_result.failure();
    }

    auto visitor = [](auto alternative) { return Rule::action(alternative); };
    return succeed(percy::visit(visitor, raw_result->get()), raw_result->span());
  }
};

template <typename Rule>
struct parser<Rule, std::enable_if_t<is_sequence_v<typename Rule::rule>>> {
  using result_type = result<action_return_t<Rule>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto raw_result = parser<typename Rule::rule>::parse(input);

    if (raw_result.is_failure()) {
      return raw_result.failure();
    }

    auto result0 = raw_result->get();
    auto result1 = std::apply(Rule::action, result0);
    auto success = succeed(std::move(result1), raw_result->span());
    return std::move(success);
  }
};

struct eof {};

template <>
struct parser<end> {
  using result_type = result<eof>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    if (!input.ended()) {
      return fail("Expected end.", input.loc());
    }

    return succeed(eof{}, {input.loc(), input.loc()});
  }
};

template <char Symbol>
struct parser<symbol<Symbol>> {
  using result_type = result<char>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    if (input.ended()) {
      return fail("Expected symbol.", input.loc());
    }

    if (input.peek() != Symbol) {
      return fail("Expected symbol.", input.loc());
    }

    return succeed(Symbol, {input.loc(), input.loc() + 1});
  }
};

template <char Begin, char End>
struct parser<range<Begin, End>> {
  using result_type = result<char>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    if (input.ended()) {
      return fail("Range.", input.loc());
    }

    if (auto symbol = input.peek(); Begin <= symbol && symbol <= End) {
      return succeed(std::move(symbol), {input.loc(), 1});
    }

    return fail("Range.", input.loc());
  }
};

template <typename StringProvider>
struct parser<word<StringProvider>> {
  using result_type = result<std::string_view>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto string = StringProvider::string;

    if (starts_with(input, string)) {
      return succeed(string, {input.loc(), string.length()});
    }

    return fail("Expected word.", input.loc());
  }

private:
  template <typename Input>
  constexpr static bool starts_with(Input input, std::string_view string) {
    for (auto character : string) {
      if (input.ended() || input.peek() != character) {
        return false;
      }

      input = input.advanced_by(1);
    }

    return true;
  }
};

template <typename Rule>
struct parser<sequence<Rule>> {
  using result_type = result<std::tuple<result_value_t<parser_result_t<Rule>>>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    using tuple_type = result_value_t<result_type>;

    auto result = parser<Rule>::parse(input);

    if (result.is_failure()) {
      return result.failure();
    }

    return succeed(tuple_type(result->get()), result->span());
  }
};

template <typename Rule, typename FollowingRule, typename... FollowingRules>
struct parser<sequence<Rule, FollowingRule, FollowingRules...>> {
  using result_type = result<std::tuple<result_value_t<parser_result_t<Rule>>,
                                        result_value_t<parser_result_t<FollowingRule>>,
                                        result_value_t<parser_result_t<FollowingRules>>...>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto result = parser<sequence<Rule>>::parse(input);

    if (result.is_failure()) {
      return result.failure();
    }

    auto following_result =
        parser<sequence<FollowingRule, FollowingRules...>>::parse(input.advanced_to(result->end()));

    if (following_result.is_failure()) {
      return following_result.failure();
    }

    auto value = std::tuple_cat(result->get(), following_result->get());
    return succeed(std::move(value), {result->begin(), following_result->end()});
  }
};

template <typename Rule>
struct parser<either<Rule>> : parser<Rule> {};

template <typename Rule, typename AlternativeRule, typename... AlternativeRules>
struct parser<either<Rule, AlternativeRule, AlternativeRules...>> {
  using result_type = parser_result_t<Rule>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto result = parser<Rule>::parse(input);

    if (result.is_success()) {
      return result;
    }

    auto alternative_result = parser<either<AlternativeRule, AlternativeRules...>>::parse(input);

    if (alternative_result.is_success()) {
      return alternative_result;
    }

    return fail("Parser.", input.loc());
  }
};

template <typename Rule>
struct parser<one_of<Rule>> {
  using result_type = result<percy::variant<result_value_t<parser_result_t<Rule>>>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    using variant_type = result_value_t<result_type>;

    auto result = parser<Rule>::parse(input);

    if (result.is_failure()) {
      return result.failure();
    }

    return success_t(variant_type(result->get()), result->span());
  }
};

template <typename Rule, typename AlternativeRule, typename... AlternativeRules>
struct parser<one_of<Rule, AlternativeRule, AlternativeRules...>> {
  using result_type = result<percy::variant<result_value_t<parser_result_t<Rule>>,
                                            result_value_t<parser_result_t<AlternativeRule>>,
                                            result_value_t<parser_result_t<AlternativeRules>>...>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    using variant_type = result_value_t<result_type>;

    auto cast_variant = [](auto small_variant) {
      return percy::visit([](auto item) { return variant_type(item); }, small_variant);
    };

    auto result = parser<one_of<Rule>>::parse(input);

    if (result.is_success()) {
      return succeed(cast_variant(result->get()), result->span());
    }

    auto alternative_result = parser<one_of<AlternativeRule, AlternativeRules...>>::parse(input);

    if (alternative_result.is_success()) {
      return succeed(cast_variant(alternative_result->get()), alternative_result->span());
    }

    return fail("One of failed.", input.loc());
  }
};

template <typename Rule>
struct parser<many<Rule>> {
  using result_type = result<std::vector<result_value_t<parser_result_t<Rule>>>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    using vector_type = result_value_t<result_type>;

    auto start = input;

    vector_type values;

    while (auto result = parser<Rule>::parse(input)) {
      values.push_back(result->get());
      input = input.advanced_to(result->end());
    }

    return succeed(values, {start.loc(), input.loc()});
  }
};
} // namespace percy

#endif
