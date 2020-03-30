#ifndef PERCY_PARSER
#define PERCY_PARSER

#include "result.hpp"
#include "rules.hpp"

#include <percy/variant.hpp>

#include <string_view>
#include <tuple>
#include <vector>

namespace percy {
template <typename Rule>
struct parser {
  using result_type = result<action_return_t<Rule>>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto raw_result = parser<typename Rule::rule>::parse(input);

    if (raw_result.is_failure()) {
      return failure(raw_result.span());
    }

    return success(Rule::action(raw_result), raw_result.span());
  }
};

template <>
struct parser<end> {
  using result_type = result<std::true_type>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    if (!input.ended()) {
      return failure({input.position(), input.position()});
    }

    return success(std::true_type(), {input.position(), input.position()});
  }
};

template <char Symbol>
struct parser<symbol<Symbol>> {
  using result_type = result<char>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    if (input.ended()) {
      return failure({input.position(), input.position()});
    }

    if (input.peek() != Symbol) {
      return failure({input.position(), input.position() + 1});
    }

    return success(Symbol, {input.position(), input.position() + 1});
  }
};

template <char Begin, char End>
struct parser<range<Begin, End>> {
  using result_type = result<char>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    if (input.ended()) {
      return failure({input.position(), input.position()});
    }

    if (auto symbol = input.peek(); Begin <= symbol && symbol <= End) {
      return success(symbol, {input.position(), input.position() + 1});
    }

    return failure({input.position(), input.position() + 1});
  }
};

template <typename StringProvider>
struct parser<word<StringProvider>> {
  using result_type = result<std::string_view>;

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto string = StringProvider::string;

    if (starts_with(input, string)) {
      return success(string, {input.position(), input.position() + string.length()});
    }

    return failure({input.position(), input.position() + string.length()});
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
      return failure(result.span());
    }

    return success(tuple_type(result.get()), result.span());
  }
};

template <typename Rule, typename FollowingRule, typename... FollowingRules>
struct parser<sequence<Rule, FollowingRule, FollowingRules...>> {
  // clang-format off
  using result_type = result<std::tuple<result_value_t<parser_result_t<Rule>>,
                                        result_value_t<parser_result_t<FollowingRule>>,
                                        result_value_t<parser_result_t<FollowingRules>>...>>;
  // clang-format on

  template <typename Input>
  constexpr static result_type parse(Input input) {
    auto result = parser<sequence<Rule>>::parse(input);

    if (result.is_failure()) {
      return failure(result.span());
    }

    auto following_result = parser<sequence<FollowingRule, FollowingRules...>>::parse(input.advanced_to(result.end()));

    if (following_result.is_failure()) {
      return failure({result.begin(), following_result.end()});
    }

    auto value = std::tuple_cat(result.get(), following_result.get());
    return success(value, {result.begin(), following_result.end()});
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

    return failure({input.position(), std::max(result.end(), alternative_result.end())});
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
      return failure(result.span());
    }

    return success(variant_type(result.get()), result.span());
  }
};

template <typename Rule, typename AlternativeRule, typename... AlternativeRules>
struct parser<one_of<Rule, AlternativeRule, AlternativeRules...>> {
  // clang-format off
  using result_type = result<percy::variant<result_value_t<parser_result_t<Rule>>,
                                            result_value_t<parser_result_t<AlternativeRule>>,
                                            result_value_t<parser_result_t<AlternativeRules>>...>>;
  // clang-format on

  template <typename Input>
  constexpr static result_type parse(Input input) {
    using variant_type = result_value_t<result_type>;

    auto cast_variant = [](auto small_variant) {
      return percy::visit([](auto item) { return variant_type(item); }, small_variant);
    };

    auto result = parser<one_of<Rule>>::parse(input);

    if (result.is_success()) {
      return success(cast_variant(result.get()), result.span());
    }

    auto alternative_result = parser<one_of<AlternativeRule, AlternativeRules...>>::parse(input);

    if (alternative_result.is_success()) {
      return success(cast_variant(alternative_result.get()), alternative_result.span());
    }

    return failure({input.position(), std::max(result.end(), alternative_result.end())});
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
      values.push_back(result.get());
      input = input.advanced_after(result);
    }

    return success(values, {start.position(), input.position()});
  }
};
} // namespace percy

#endif
