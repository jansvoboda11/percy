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
  Node node_;
  input_span span_;

public:
  using value_type = Node;

  constexpr success_t(Node node, input_span span) : node_(node), span_(span) {}

  constexpr Node get() { return node_; }

  constexpr input_span span() const { return span_; }
  constexpr input_location begin() const { return span_.begin(); }
  constexpr input_location end() const { return span_.end(); }
};

template <typename Node>
constexpr success_t<Node> succeed(Node node, input_span span) {
  return success_t<Node>(node, span);
}

template <typename Node>
class result {
public:
  using success_type = success_t<Node>;
  using failure_type = failure_t;

  constexpr result(success_type value) : value_(value) {}
  constexpr result(failure_type value) : value_(value) {}

  constexpr operator bool() const { return is_success(); }
  constexpr bool is_success() const { return percy::holds_alternative<success_type>(value_); }
  constexpr bool is_failure() const { return !is_success(); }

  constexpr success_type *operator->() { return &percy::get<success_type>(value_); }
  constexpr failure_t failure() const { return percy::get<failure_type>(value_); }

private:
  percy::variant<success_type, failure_type> value_;
};
} // namespace percy

#endif
