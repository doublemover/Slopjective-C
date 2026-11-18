#include "cxx_api.h"

static_assert(__is_trivially_copyable(MeterAPI::Reading));
static_assert(sizeof(MeterAPI::Reading) == 16);
static_assert(alignof(MeterAPI::Reading) == 8);
static_assert(__builtin_offsetof(MeterAPI::Reading, total) == 8);

MeterAPI::Reading MeterAPI::initialReading() noexcept {
  return {40, 2147483688LL};
}

MeterAPI::Reading MeterAPI::adjustedReading(Reading value, int increment) noexcept {
  value.count += increment;
  value.total += increment;
  return value;
}
