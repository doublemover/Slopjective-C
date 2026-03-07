#include "runtime/objc3_runtime.h"

#include <cstdint>
#include <cstring>

namespace {

constexpr std::int64_t kDispatchModulus = 2147483629LL;

std::int64_t ComputeSelectorScore(const char *selector) {
  if (selector == nullptr) {
    return 0;
  }

  std::int64_t score = 0;
  std::int64_t index = 1;
  const unsigned char *cursor =
      reinterpret_cast<const unsigned char *>(selector);
  while (*cursor != 0U) {
    score = (score + (static_cast<std::int64_t>(*cursor) * index)) %
            kDispatchModulus;
    ++cursor;
    ++index;
  }
  return score;
}

int ExpectedDispatch(int receiver, const char *selector, int a0, int a1, int a2,
                     int a3) {
  std::int64_t value = 41;
  value += static_cast<std::int64_t>(receiver) * 97;
  value += static_cast<std::int64_t>(a0) * 7;
  value += static_cast<std::int64_t>(a1) * 11;
  value += static_cast<std::int64_t>(a2) * 13;
  value += static_cast<std::int64_t>(a3) * 17;
  value += ComputeSelectorScore(selector) * 19;
  value %= kDispatchModulus;
  if (value < 0) {
    value += kDispatchModulus;
  }
  return static_cast<int>(value);
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  const objc3_runtime_image_descriptor image{
      "probe-module",
      1,
      2,
      3,
      4,
      5,
  };
  if (objc3_runtime_register_image(&image) != 0) {
    return 10;
  }

  const objc3_runtime_selector_handle *first =
      objc3_runtime_lookup_selector("alpha:beta:");
  const objc3_runtime_selector_handle *second =
      objc3_runtime_lookup_selector("alpha:beta:");
  const objc3_runtime_selector_handle *third =
      objc3_runtime_lookup_selector("gamma");
  if (first == nullptr || second == nullptr || third == nullptr) {
    return 11;
  }
  if (first != second) {
    return 12;
  }
  if (first->stable_id != 1 || third->stable_id != 2) {
    return 13;
  }
  if (std::strcmp(first->selector, "alpha:beta:") != 0) {
    return 14;
  }

  const int actual = objc3_runtime_dispatch_i32(5, "alpha:beta:", 1, 2, 3, 4);
  const int expected = ExpectedDispatch(5, "alpha:beta:", 1, 2, 3, 4);
  if (actual != expected) {
    return 15;
  }

  objc3_runtime_reset_for_testing();
  const objc3_runtime_selector_handle *after_reset =
      objc3_runtime_lookup_selector("alpha:beta:");
  if (after_reset == nullptr || after_reset->stable_id != 1) {
    return 16;
  }

  return 0;
}
