#ifndef PERCY_RULES
#define PERCY_RULES

namespace percy {
struct end {};

template <char Symbol>
struct symbol {};

template <char Begin, char End>
struct range {};

template <typename StringProvider>
struct word {};

template <typename Rule, typename... FollowingRules>
struct sequence {};

template <typename Rule, typename... AlternativeRules>
struct either {};

template <typename Rule>
struct many {};
} // namespace percy

#endif
