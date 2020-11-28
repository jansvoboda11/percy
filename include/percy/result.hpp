#ifndef PERCY_RESULT
#define PERCY_RESULT

#include "percy/input_span.hpp"

#include <percy/variant.hpp>

namespace percy {
class failure_t {
  std::string_view message_;
  input_location location_;

public:
  constexpr failure_t(std::string_view message, input_location location)
      : message_(message), location_(location) {}

  constexpr input_location loc() const { return location_; }
};

constexpr failure_t fail(std::string_view message, input_location location) {
  return failure_t(message, location);
}

template <typename Node>
class success_t {
  struct empty {};

  percy::variant<Node, empty> node_;
  input_span span_;

public:
  using value_type = Node;

  constexpr success_t(Node &&node, input_span span)
      : node_(std::forward<Node>(node)), span_(span) {}

  constexpr Node get() {
    Node node = percy::get<Node>(node_);
    node_ = empty{};
    return std::move(node);
  }

  constexpr Node get() const {
    return percy::get<Node>(node_);
  }

  constexpr input_span span() const { return span_; }
  constexpr input_location begin() const { return span_.begin(); }
  constexpr input_location end() const { return span_.end(); }

  constexpr ~success_t() = default;
};

template <typename Node>
constexpr success_t<Node> succeed(Node &&node, input_span span) {
  return success_t<Node>(std::forward<Node>(node), span);
}

template <typename Node>
class result {
public:
  using success_type = success_t<Node>;
  using failure_type = failure_t;

  constexpr result(success_type&& value) : value_(std::move(value)) {}
  constexpr result(failure_type value) : value_(value) {}

  constexpr operator bool() const { return is_success(); }
  constexpr bool is_success() const { return percy::holds_alternative<success_type>(value_); }
  constexpr bool is_failure() const { return !is_success(); }

  constexpr const success_type *operator->() const { return &percy::get<success_type>(value_); }
  constexpr success_type *operator->() { return &percy::get<success_type>(value_); }
  constexpr failure_t failure() const { return percy::get<failure_type>(value_); }

  constexpr ~result() = default;

private:
  percy::variant<success_type, failure_type> value_;
};
} // namespace percy

#endif
