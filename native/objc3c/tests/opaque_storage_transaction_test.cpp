#include "runtime/stdlib/opaque_service_storage.h"
#include <cassert>
#include <cstdlib>
#include <new>

static thread_local int fail_after = -1;
extern "C" void *__real__Znwm(std::size_t bytes);
extern "C" void *__wrap__Znwm(std::size_t bytes) {
  if (fail_after == 0) throw std::bad_alloc();
  if (fail_after > 0) --fail_after;
  return __real__Znwm(bytes);
}
struct Value;
using Storage = objc3c::runtime::stdlib_runtime::OpaqueServiceStorage<Value, 2>;
static Storage *active = nullptr;
static unsigned destroyed = 0;
struct Value {
  ~Value() {
    ++destroyed;
    // Destruction under the registry lock would deadlock this independent
    // lifetime oracle; CTest's watchdog must treat that as a failure.
    if (active) (void)active->LiveCount();
  }
};
int main() {
  unsigned rejected = 0;
  for (int position = 0; position != 5; ++position) {
    Storage registry;
    active = &registry;
    auto candidate = std::make_unique<Value>();
    const auto before = destroyed;
    fail_after = position;
    bool failed = false;
    try { (void)registry.Store(std::move(candidate), 1); }
    catch (const std::bad_alloc &) { failed = true; }
    fail_after = -1;
    assert(failed);
    ++rejected;
    assert(registry.LiveCount() == 0 && registry.TokenCount() == 0);
    assert(destroyed == before + 1);
    auto *first = registry.Store(std::make_unique<Value>(), 1);
    auto *second = registry.Store(std::make_unique<Value>(), 1);
    assert(first && second && first != second && registry.TokenCount() == 2);
    assert(registry.Release(first, 1) && registry.Release(second, 1));
    assert(registry.LiveCount() == 0 && registry.TokenCount() == 2);
    assert(registry.Inspect(first, 1) == Storage::Status::StaleHandle);
    assert(!registry.Store(std::make_unique<Value>(), 1));
    active = nullptr;
  }
  assert(rejected == 5);
}
