#ifndef PERCY_RULES
#define PERCY_RULES

#include "percy/type_traits.hpp"

namespace percy {
struct end {};

template <char Symbol>
struct symbol {};

template <char Begin, char End>
struct range {
  static_assert(Begin <= End,
                "The `range` rule requires the `Begin` char not be greater than the `End` char.");
};

template <typename StringProvider>
struct word {};

template <typename Rule, typename... FollowingRules>
struct sequence {};

template <typename Rule, typename... AlternativeRules>
struct either {
  static_assert(are_same_v<parser_result_t<Rule>, parser_result_t<AlternativeRules>...>,
                "The `either` rule requires each alternative to produce the same type.");
};

template <typename Rule, typename... AlternativeRules>
struct one_of {
  static_assert(are_unique_v<parser_result_t<Rule>, parser_result_t<AlternativeRules>...>,
                "The `one_of` rule requires each alternative to produce an unique type.");
};

template <typename Rule>
struct many {};
} // namespace percy

#endif
