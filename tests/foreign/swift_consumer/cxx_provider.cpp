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

namespace MeterAPI {
namespace {
int allocations;
int destructions;
int copies;
}

ReadingOwner::ReadingOwner(int amount) noexcept : storage(new int(amount)) {
  ++allocations;
}

ReadingOwner::ReadingOwner(const ReadingOwner &other) noexcept
    : ReadingOwner(other.amount()) {
  ++copies;
}

ReadingOwner::ReadingOwner(ReadingOwner &&other) noexcept : storage(other.storage) {
  other.storage = nullptr;
}

ReadingOwner::~ReadingOwner() noexcept {
  if (storage) {
    delete storage;
    ++destructions;
  }
}

int ReadingOwner::amount() const noexcept { return storage ? *storage : 0; }

ReadingOwner initialOwner(int amount) noexcept { return ReadingOwner(amount); }

ReadingOwner adjustedOwner(ReadingOwner value, int increment) noexcept {
  return ReadingOwner(value.amount() + increment);
}

int ownerAllocations() noexcept { return allocations; }
int ownerDestructions() noexcept { return destructions; }
int ownerCopies() noexcept { return copies; }
}
