#ifndef PERCY_UTILS
#define PERCY_UTILS

#include <variant>

namespace percy {
template <std::size_t Index, typename... Us, typename... Ts>
constexpr std::variant<Ts..., Us...> append_types_impl(std::variant<Ts...> value) {
  if (Index == value.index()) {
    return std::variant<Ts..., Us...>(std::in_place_index<Index>, std::get<Index>(value));
  }

  if constexpr (Index > 1) {
    return append_types_impl<Index - 1, Ts..., Us...>(value);
  }

  std::abort(); // unreachable
}

template <typename... Us, typename... Ts>
constexpr std::variant<Ts..., Us...> append_types(std::variant<Ts...> value) {
  return append_types_impl<sizeof...(Ts) - 1, Us...>(value);
}
} // namespace percy

#endif
