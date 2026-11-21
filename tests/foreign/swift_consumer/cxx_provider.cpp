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
int unique_allocations;
int unique_destructions;
int unique_moves;
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

UniqueReading::UniqueReading(int amount) noexcept : storage(new int(amount)) {
  ++unique_allocations;
}

UniqueReading::UniqueReading(UniqueReading &&other) noexcept : storage(other.storage) {
  other.storage = nullptr;
  ++unique_moves;
}

UniqueReading::~UniqueReading() noexcept {
  if (storage) {
    delete storage;
    ++unique_destructions;
  }
}

int UniqueReading::amount() const noexcept { return storage ? *storage : 0; }

void UniqueReading::add(int increment) noexcept {
  if (storage) *storage += increment;
}

UniqueReading initialUniqueReading(int amount) noexcept { return UniqueReading(amount); }

UniqueReading adjustedUniqueReading(UniqueReading value, int increment) noexcept {
  value.add(increment);
  return value;
}

int uniqueAllocations() noexcept { return unique_allocations; }
int uniqueDestructions() noexcept { return unique_destructions; }
int uniqueMoves() noexcept { return unique_moves; }
}
