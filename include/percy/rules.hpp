#ifndef PERCY_RULES
#define PERCY_RULES

namespace percy {
struct end {};

template <char Symbol>
struct symbol {};

template <typename ...Rules>
struct sequence {};

template <typename Rule, typename ...AlternativeRules>
struct one_of {};

template <typename Rule>
struct repeat {};
}

#endif
