#ifndef PERCY_RULES
#define PERCY_RULES

namespace percy {
struct end {};

template <char Symbol>
struct symbol {};

template <typename Rule, typename ...Rules>
struct sequence {};

template <typename Rule>
struct repeat {};

template <typename Rule, typename ...Rules>
struct one_of {};

template <typename Rule>
struct unwrap {};
}

#endif
