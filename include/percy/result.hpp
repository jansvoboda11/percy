#ifndef PERCY_RESULT
#define PERCY_RESULT

#include <variant>

#include <cassert>

namespace percy {
class error {
  //
};

class input_span {
  std::size_t begin_;
  std::size_t end_;

public:
  constexpr input_span(std::size_t begin, std::size_t end)
      : begin_(begin), end_(end) {
    //
  }

  constexpr std::size_t begin() const {
    return begin_;
  }

  constexpr std::size_t end() const {
    return end_;
  }
};

template<typename T>
class result {
  std::variant<T, error> value_;
  input_span span_;

  template <typename U>
  friend class result;

public:
  using value_type = T;

  constexpr result(const result<T>& other)
      : value_(other.value_), span_(other.span_) {
    //
  }

  constexpr static result<T> success(T value, input_span span) {
    return result<T>(value, span);
  }

  constexpr static result<T> success(input_span span) {
    return result<T>(T(), span);
  }

  constexpr static result<T> failure(input_span span) {
    return result<T>(error(), span);
  }

  constexpr bool is_success() const {
    return std::holds_alternative<T>(value_);
  }

  constexpr bool is_failure() const {
    return std::holds_alternative<error>(value_);
  }

  constexpr operator bool() const {
    return is_success();
  }

  constexpr auto get() const {
    assert(is_success());
    return std::get<T>(value_);
  }

  constexpr input_span span() const {
    return span_;
  }

  constexpr std::size_t begin() const {
    return span_.begin();
  }

  constexpr std::size_t end() const {
    return span_.end();
  }

  template <typename ...Us>
  constexpr result<std::tuple<T, Us...>> followed_by(const result<std::tuple<Us...>>& other) const {
    auto tuple = std::tuple_cat(std::tuple(get()), other.get());
    return result<std::tuple<T, Us...>>::success(tuple, {begin(), other.end()});
  }

private:
  constexpr explicit result(T value, input_span span)
      : value_(value), span_(span) {
    //
  }

  constexpr explicit result(error error, input_span span)
      : value_(error), span_(span) {
    //
  }
};
}

#endif
