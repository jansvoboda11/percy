#ifndef PERCY_RESULT
#define PERCY_RESULT

#include <percy/variant.hpp>

namespace percy {
class input_span {
  std::size_t begin_;
  std::size_t end_;

public:
  constexpr input_span(std::size_t begin, std::size_t end) : begin_(begin), end_(end) {}

  constexpr std::size_t begin() const { return begin_; }

  constexpr std::size_t end() const { return end_; }
};

class failure {
  input_span span_;

public:
  constexpr explicit failure(input_span span) : span_(span) {}

  constexpr input_span span() const { return span_; }
};

class error {};

template <typename T>
class result {
  percy::variant<T, error> value_;
  input_span span_;

  template <typename U>
  friend class result;

public:
  using value_type = T;

  constexpr explicit result(T value, input_span span) : value_(value), span_(span) {}

  constexpr explicit result(error error, input_span span) : value_(error), span_(span) {}

  constexpr result(const failure &fail) : value_(error()), span_(fail.span()) {}

  constexpr bool is_success() const { return percy::holds_alternative<T>(value_); }

  constexpr bool is_failure() const { return percy::holds_alternative<error>(value_); }

  constexpr operator bool() const { return is_success(); }

  constexpr input_span span() const { return span_; }

  constexpr std::size_t begin() const { return span_.begin(); }

  constexpr std::size_t end() const { return span_.end(); }

  constexpr auto get() const { return percy::get<T>(value_); }
};

template <typename T>
constexpr result<T> success(T value, input_span span) {
  return result<T>(value, span);
}
} // namespace percy

#endif
