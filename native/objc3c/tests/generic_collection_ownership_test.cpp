#include "owned_payload_ledger.h"
#include "runtime/public/generic_associative_service.h"
#include "runtime/public/generic_collection_service.h"

#include <array>
#include <cstddef>
#include <vector>

using namespace owned_payload_test;

extern "C" void *__real__Znwm(std::size_t);
extern "C" void *__wrap__Znwm(std::size_t size) {
  auto expected = ledger.fail_allocation_after_copies.load();
  if (expected >= 0 && ledger.copies >= static_cast<unsigned>(expected) &&
      ledger.fail_allocation_after_copies.exchange(-1) >= 0) throw std::bad_alloc();
  return __real__Znwm(size);
}

namespace {
constexpr auto ok = OBJC3_GENERIC_COLLECTION_OK;
auto Map() {
  return objc3_generic_map_descriptor{sizeof(objc3_generic_map_descriptor),
      OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION, Ops(true), Ops(), Hash, Equal};
}
auto Set() {
  return objc3_generic_set_descriptor{sizeof(objc3_generic_set_descriptor),
      OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION, Ops(true), Hash, Equal};
}
void CheckSources(const auto &sources) {
  for (const auto &source : sources) assert(source.instance == 0);
}

void Rollback() {
  const std::array<Payload, 4> keys = {{{1}, {2}, {1}, {3}}};
  const std::array<Payload, 4> values = {{{10}, {20}, {30}, {40}}};
  auto descriptor = Map();
  // Learn callback counts from the normal case, then fail every invocation,
  // including new-key/value and duplicate-value construction, under both status
  // mappings. No readback or output destruction can balance a missing rollback.
  ledger.Reset();
  objc3_generic_map_t map = nullptr;
  assert(objc3_generic_map_create(&descriptor, keys.data(), values.data(), 4, &map) == ok);
  const std::array<unsigned, 3> calls = {ledger.copies.load(), ledger.hashes, ledger.equals};
  assert(objc3_generic_map_release(map) == ok);
  ledger.Balanced();
  for (unsigned kind = 0; kind != 3; ++kind)
    for (unsigned position = 0; position != calls[kind]; ++position)
      for (bool oom : {false, true}) {
        ledger.Reset();
        ledger.FailAt(static_cast<Callback>(kind), position, oom);
        map = reinterpret_cast<objc3_generic_map_t>(1);
        const auto status = objc3_generic_map_create(&descriptor, keys.data(), values.data(), 4, &map);
        assert(status == (oom ? OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY : OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT));
        assert(map == nullptr);
        ledger.Balanced();
        CheckSources(keys); CheckSources(values);
      }

  auto ops = Ops();
  for (unsigned position = 0; position != values.size(); ++position) {
    ledger.Reset(); ledger.FailAt(Callback::Copy, position);
    objc3_generic_array_t array = reinterpret_cast<objc3_generic_array_t>(1);
    assert(objc3_generic_array_create(&ops, values.data(), values.size(), &array) == OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT);
    assert(array == nullptr); ledger.Balanced(); CheckSources(values);
  }
  bool saw_failure = false, saw_success = false;
  for (unsigned position = 0; position != 8 && !saw_success; ++position) {
    ledger.Reset();
    objc3_generic_array_t array = nullptr;
    assert(objc3_generic_array_create(&ops, values.data(), 3, &array) == ok);
    const auto before = ledger.Live();
    ledger.FailAt(Callback::Copy, position);
    const auto status = objc3_generic_array_append(array, &values.back());
    ledger.FailAt(Callback::None, -1);
    if (status == OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT) {
      saw_failure = true;
      assert(ledger.Live() == before && "failed mutation leaked candidates or retired published payloads");
    } else { assert(status == ok); saw_success = true; }
    assert(objc3_generic_array_release(array) == ok);
    ledger.Balanced(); CheckSources(values);
  }
  assert(saw_failure && saw_success);

  // The first ordinary allocation after all payload callbacks fails before
  // publication. The ledger itself allocates nothing in callbacks/destructors.
  for (bool associative : {false, true}) {
    ledger.Reset(); ledger.fail_allocation_after_copies = associative ? 8 : 4;
    if (associative) {
      const std::array<Payload, 4> distinct = {{{1}, {2}, {3}, {4}}};
      map = nullptr;
      assert(objc3_generic_map_create(&descriptor, distinct.data(), values.data(), 4, &map) == OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY);
      assert(!map);
    } else {
      objc3_generic_array_t array = nullptr;
      assert(objc3_generic_array_create(&ops, values.data(), 4, &array) == OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY);
      assert(!array);
    }
    assert(ledger.fail_allocation_after_copies == -1);
    ledger.Balanced();
  }
}

void BulkCopiesAndOrder() {
  auto map_descriptor = Map(); auto set_descriptor = Set();
  for (unsigned count : {0u, 1u, 32u, 128u, 512u, 2048u}) {
    ledger.Reset();
    std::vector<Payload> keys(count), values(count);
    for (unsigned i = 0; i != count; ++i) { keys[i].value = i; values[i].value = 3 * i; }
    objc3_generic_map_t map = nullptr;
    assert(objc3_generic_map_create(&map_descriptor, keys.data(), values.data(), count, &map) == ok);
    assert(ledger.key_copies == count && ledger.value_copies == count && ledger.copies == 2 * count);
    assert(objc3_generic_map_release(map) == ok); ledger.Balanced();
    ledger.Reset();
    objc3_generic_set_t set = nullptr;
    assert(objc3_generic_set_create(&set_descriptor, keys.data(), count, &set) == ok);
    assert(ledger.key_copies == count && ledger.value_copies == 0);
    assert(objc3_generic_set_release(set) == ok); ledger.Balanced();
    CheckSources(keys); CheckSources(values);
  }
  const std::array<Payload, 6> keys = {{{2}, {0}, {2}, {1}, {0}, {3}}};
  const std::array<Payload, 6> values = {{{10}, {20}, {30}, {40}, {50}, {0}}};
  for (bool collision : {false, true}) {
    ledger.Reset(); ledger.collisions = collision;
    objc3_generic_map_t map = nullptr;
    assert(objc3_generic_map_create(&map_descriptor, keys.data(), values.data(), 6, &map) == ok);
    assert(ledger.key_copies == 4 && ledger.value_copies == 6);
    objc3_generic_associative_iterator_t iterator = nullptr;
    assert(objc3_generic_map_iterator(map, &iterator) == ok);
    const std::array<std::int64_t, 4> expected_keys = {2, 0, 1, 3}, expected_values = {30, 50, 40, 0};
    for (unsigned i = 0; i != expected_keys.size(); ++i) {
      Payload key{}, value{}; int has = 0;
      assert(objc3_generic_associative_iterator_next(iterator, &has, &key, &value) == ok && has);
      assert(key.value == expected_keys[i] && value.value == expected_values[i]);
      Destroy(&key); Destroy(&value);
    }
    assert(objc3_generic_associative_iterator_release(iterator) == ok);
    assert(objc3_generic_map_release(map) == ok); ledger.Balanced();
    ledger.Reset(); ledger.collisions = collision;
    objc3_generic_set_t set = nullptr;
    assert(objc3_generic_set_create(&set_descriptor, keys.data(), keys.size(), &set) == ok);
    assert(ledger.copies == 4);
    assert(objc3_generic_set_release(set) == ok); ledger.Balanced();
  }
  ledger.Reset(); ledger.incoherent = true;
  const std::array<Payload, 4> incoherent = {{{1}, {2}, {3}, {11}}};
  objc3_generic_map_t map = nullptr;
  assert(objc3_generic_map_create(&map_descriptor, incoherent.data(), values.data(), 4, &map) == OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT);
  assert(!map); ledger.Balanced();
}

objc3_generic_map_t unrelated = nullptr;
bool entered = false;
void ReenterUnrelated() {
  if (entered) return;
  entered = true;
  std::uint64_t count = 0;
  assert(objc3_generic_map_count(unrelated, &count) == ok);
  Payload key{99}, value{100}; int inserted = 0;
  assert(objc3_generic_map_insert(unrelated, &key, &value, &inserted) == ok);
}
void Reentry() {
  ledger.Reset();
  auto descriptor = Map();
  assert(objc3_generic_map_create_empty(&descriptor, &unrelated) == ok);
  ledger.on_callback = ReenterUnrelated; entered = false;
  Payload key{1}, value{2}; objc3_generic_map_t map = nullptr;
  assert(objc3_generic_map_create(&descriptor, &key, &value, 1, &map) == ok);
  assert(entered); ledger.on_callback = nullptr;
  assert(objc3_generic_map_release(map) == ok);
  assert(objc3_generic_map_release(unrelated) == ok);
  unrelated = nullptr; ledger.Balanced();
}
} // namespace

int main() { Rollback(); BulkCopiesAndOrder(); Reentry(); ledger.Balanced(); }
