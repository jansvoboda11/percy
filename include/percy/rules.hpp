#ifndef PERCY_RULES
#define PERCY_RULES

namespace percy {
struct end {};

template <char Symbol>
struct symbol {};

template <typename StringProvider>
struct word {};

template <typename Rule, typename... FollowingRules>
struct sequence {};

template <typename Rule, typename... AlternativeRules>
struct one_of {};

template <typename Rule>
struct repeat {};
} // namespace percy

#endif
