#ifndef PERCY_INPUT
#define PERCY_INPUT

#include "percy/input_span.hpp"

#include <string_view>

namespace percy {
class input {
  std::string_view content_;
  std::size_t cursor_;

public:
  constexpr explicit input(const char *content, std::size_t position = 0)
      : content_(content), cursor_(position) {}

  constexpr explicit input(std::string_view content, std::size_t position = 0)
      : content_(content), cursor_(position) {}

  constexpr char peek() const { return content_[cursor_]; }
  constexpr bool ended() const { return cursor_ >= content_.length(); }

  constexpr input_location loc() const { return input_location(cursor_); }

  constexpr input advanced_by(std::size_t offset) const {
    return input(content_, cursor_ + offset);
  }

  constexpr input advanced_to(input_location location) const {
    return input(content_, location.get());
  }
};
} // namespace percy

#endif
