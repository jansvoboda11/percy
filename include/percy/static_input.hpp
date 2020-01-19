#ifndef PERCY_STATIC_INPUT
#define PERCY_STATIC_INPUT

#include <string_view>

namespace percy {
class static_input {
  std::string_view content_;
  std::size_t cursor_;

public:
  constexpr explicit static_input(const char *content, std::size_t position = 0)
      : content_(content), cursor_(position) {
    //
  }

  constexpr explicit static_input(std::string_view content, std::size_t position = 0)
      : content_(content), cursor_(position) {
    //
  }

  constexpr char peek() const {
    return content_[cursor_];
  }

  constexpr std::size_t position() const {
    return cursor_;
  }

  constexpr bool ended() const {
    return cursor_ >= content_.length();
  }

  constexpr static_input advanced_by(std::size_t offset) const {
    return static_input(content_, cursor_ + offset);
  }

  constexpr static_input advanced_to(std::size_t position) const {
    return static_input(content_, position);
  }

  template <typename Result>
  constexpr static_input advanced_after(const Result& result) const {
    return advanced_to(result.end());
  }
};
}

#endif
