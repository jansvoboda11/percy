#ifndef PERCY_INPUT_SPAN
#define PERCY_INPUT_SPAN

namespace percy {
class input_location {
  std::size_t location_;

public:
  constexpr explicit input_location(std::size_t location) : location_(location) {}

  constexpr std::size_t get() const { return location_; }

  constexpr bool operator==(std::size_t value) const { return value == location_; }

  constexpr input_location operator+(std::size_t offset) {
    return input_location(location_ + offset);
  }
};

class input_span {
  input_location begin_;
  input_location end_;

public:
  constexpr input_span(input_location begin, input_location end) : begin_(begin), end_(end) {}
  constexpr input_span(input_location begin, std::size_t length)
      : begin_(begin), end_(begin + length) {}

  constexpr input_location begin() const { return begin_; }
  constexpr input_location end() const { return end_; }
};
} // namespace percy

#endif
