#include "runtime/values/value_optional.h"

#include <cstdint>
#include <cstdlib>

namespace {

constexpr std::uint64_t kObjc3ValueOptionalPresenceMask = 1u;
constexpr unsigned kObjc3ValueOptionalPayloadShift = 1u;

std::int64_t PackOptionalI32(bool has_value, int payload) {
  if (!has_value) {
    return 0;
  }
  const auto bits = static_cast<std::uint32_t>(payload);
  const std::uint64_t packed =
      (static_cast<std::uint64_t>(bits) << kObjc3ValueOptionalPayloadShift) |
      kObjc3ValueOptionalPresenceMask;
  return static_cast<std::int64_t>(packed);
}

bool HasValue(std::int64_t value) {
  return (static_cast<std::uint64_t>(value) &
          kObjc3ValueOptionalPresenceMask) != 0u;
}

int Payload(std::int64_t value) {
  const std::uint64_t payload =
      static_cast<std::uint64_t>(value) >> kObjc3ValueOptionalPayloadShift;
  return static_cast<int>(static_cast<std::uint32_t>(payload));
}

}  // namespace

extern "C" std::int64_t objc3_runtime_optional_absent_i64(void) {
  return PackOptionalI32(false, 0);
}

extern "C" std::int64_t objc3_runtime_optional_present_i32(int payload) {
  return PackOptionalI32(true, payload);
}

extern "C" int objc3_runtime_optional_has_value_i32(std::int64_t value) {
  return HasValue(value) ? 1 : 0;
}

extern "C" int objc3_runtime_optional_payload_or_i32(std::int64_t value,
                                                     int fallback) {
  return HasValue(value) ? Payload(value) : fallback;
}

extern "C" int objc3_runtime_optional_unwrap_i32(std::int64_t value) {
  if (!HasValue(value)) {
    std::abort();
  }
  return Payload(value);
}
