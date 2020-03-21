#ifndef PERCY_VARIANT
#define PERCY_VARIANT

#include "type_traits.hpp"
#include "variadic_union.hpp"

namespace percy {
template <typename... Ts>
class variant {
  static_assert(are_unique_v<Ts...>, "Each type in the variant must be unique.");

private:
  variadic_union<Ts...> union_;
  std::size_t index_;

public:
  //
};
} // namespace percy

#endif
