#include "owned_payload_ledger.h"
#include "runtime/public/generic_associative_service.h"
#include "runtime/public/generic_collection_service.h"

#include <array>
#include <cstddef>
#include <cstdio>
#include <limits>
#include <algorithm>
#include <condition_variable>
#include <thread>
#include <vector>

using namespace owned_payload_test;

extern "C" void *__real__Znwm(std::size_t);
extern "C" void *__wrap__Znwm(std::size_t size) {
  if (ledger.FailAllocation()) throw std::bad_alloc();
  auto expected = ledger.fail_allocation_after_copies.load();
  if (expected >= 0 && ledger.copies >= static_cast<unsigned>(expected) &&
      ledger.fail_allocation_after_copies.exchange(-1) >= 0) throw std::bad_alloc();
  return __real__Znwm(size);
}

extern "C" void *__real__ZnwmSt11align_val_tRKSt9nothrow_t(
    std::size_t, std::align_val_t, const std::nothrow_t &) noexcept;
extern "C" void *__wrap__ZnwmSt11align_val_tRKSt9nothrow_t(
    std::size_t size, std::align_val_t alignment, const std::nothrow_t &tag) noexcept {
  if (ledger.FailAllocation()) return nullptr;
  return __real__ZnwmSt11align_val_tRKSt9nothrow_t(size, alignment, tag);
}

#ifdef OBJC3_GENERIC_ARRAY_STORAGE_TESTING
extern "C" void objc3_test_array_storage_reset();
extern "C" std::uint64_t objc3_test_array_metadata_slots();
extern "C" std::uint64_t objc3_test_array_nodes();
extern "C" std::uint64_t objc3_test_array_chunks();
#endif

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

template <bool Key>
objc3_service_status CopyZero(void *destination, const void *source) {
  assert(destination && source && reinterpret_cast<std::uintptr_t>(destination) % 64 == 0 &&
         reinterpret_cast<std::uintptr_t>(source) % 64 == 0);
  if (ledger.Fail(Callback::Copy)) return ledger.Failure();
  ledger.RegisterZero(destination);
  if constexpr (Key) ++ledger.key_copies; else ++ledger.value_copies;
  return OBJC3_SERVICE_OK;
}
void DestroyZero(void *destination) { ledger.DestroyZero(destination); }
objc3_service_status MoveZero(void *destination, void *source) { return CopyZero<false>(destination, source); }
objc3_service_status HashZero(const void *source, std::uint64_t *out) {
  assert(source && reinterpret_cast<std::uintptr_t>(source) % 64 == 0);
  if (ledger.Fail(Callback::Hash)) return ledger.Failure();
  *out = 0; return OBJC3_SERVICE_OK;
}
objc3_service_status EqualZero(const void *left, const void *right, int *out) {
  assert(left && right && reinterpret_cast<std::uintptr_t>(left) % 64 == 0 &&
         reinterpret_cast<std::uintptr_t>(right) % 64 == 0);
  if (ledger.Fail(Callback::Equal)) return ledger.Failure();
  *out = 1; return OBJC3_SERVICE_OK;
}
void ZeroSizedAndBounds() {
  const objc3_value_ops key_ops = {sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION, 0, 64,
      CopyZero<true>, MoveZero, DestroyZero};
  auto value_ops = key_ops; value_ops.copy_init = CopyZero<false>;
  const objc3_generic_map_descriptor descriptor = {sizeof(objc3_generic_map_descriptor),
      OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION, key_ops, value_ops, HashZero, EqualZero};
  const objc3_generic_set_descriptor set_descriptor = {sizeof(objc3_generic_set_descriptor),
      OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION, key_ops, HashZero, EqualZero};
  ledger.Reset(); objc3_generic_map_t map = nullptr;
  assert(objc3_generic_map_create(&descriptor, nullptr, nullptr, 3, &map) == ok);
  assert(ledger.key_copies == 1 && ledger.value_copies == 3 && ledger.Live().size() == 2);
  assert(objc3_generic_map_release(map) == ok); ledger.Balanced();
  ledger.Reset(); objc3_generic_set_t set = nullptr;
  assert(objc3_generic_set_create(&set_descriptor, nullptr, 3, &set) == ok);
  assert(ledger.key_copies == 1 && ledger.Live().size() == 1);
  assert(objc3_generic_set_release(set) == ok); ledger.Balanced();
  ledger.Reset(); objc3_generic_array_t array = nullptr;
  assert(objc3_generic_array_create(&value_ops, nullptr, 33, &array) == ok);
  assert(ledger.Live().size() == 33);
  assert(objc3_generic_array_set(array, 31, nullptr) == ok);
  assert(objc3_generic_array_append(array, nullptr) == ok);
  assert(ledger.Live().size() == 34);
  assert(objc3_generic_array_get(array, 32, nullptr) == ok);
  assert(ledger.Live().size() == 34); // internal copy-out is also destroyed
  assert(objc3_generic_array_release(array) == ok); ledger.Balanced();
  for (unsigned position = 0; position != 4; ++position) {
    ledger.Reset(); ledger.FailAt(Callback::Copy, position);
    map = nullptr;
    assert(objc3_generic_map_create(&descriptor, nullptr, nullptr, 3, &map) == OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT);
    assert(!map); ledger.Balanced();
  }
  ledger.Reset();
  constexpr std::uint64_t cap = 1u << 20;
  assert(objc3_generic_map_create(&descriptor, nullptr, nullptr, cap + 1, &map) == OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED && !map);
  assert(objc3_generic_set_create(&set_descriptor, nullptr, cap + 1, &set) == OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED && !set);
  assert(objc3_generic_array_create(&value_ops, nullptr, cap + 1, &array) == OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED && !array);
  // At the legal cap, controlled callback rejection occurs after admission.
  ledger.FailAt(Callback::Hash, 0);
  assert(objc3_generic_map_create(&descriptor, nullptr, nullptr, cap, &map) == OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT && !map);
  ledger.FailAt(Callback::Hash, 0);
  assert(objc3_generic_set_create(&set_descriptor, nullptr, cap, &set) == OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT && !set);
  auto invalid = value_ops; invalid.payload_alignment = std::numeric_limits<std::uint64_t>::max();
  assert(objc3_generic_array_create(&invalid, nullptr, 1, &array) == OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT && !array);
  invalid = value_ops; invalid.payload_size = std::numeric_limits<std::uint64_t>::max();
  assert(objc3_generic_array_create(&invalid, nullptr, 2, &array) != ok && !array);
  ledger.Balanced();
}

void AllocationSweeps() {
  auto ops = Ops(); auto map_descriptor = Map(); auto set_descriptor = Set();
  const std::array<Payload, 4> sources = {{{1}, {2}, {3}, {4}}};
  for (unsigned family = 0; family != 3; ++family) {
    bool completed = false; unsigned failures = 0;
    for (unsigned position = 0; position != 64 && !completed; ++position) {
      ledger.Reset(); ledger.allocation_failure_position = position;
      objc3_generic_map_t map = nullptr; objc3_generic_set_t set = nullptr; objc3_generic_array_t array = nullptr;
      auto status = family == 0 ? objc3_generic_map_create(&map_descriptor, sources.data(), sources.data(), sources.size(), &map)
          : family == 1 ? objc3_generic_set_create(&set_descriptor, sources.data(), sources.size(), &set)
                        : objc3_generic_array_create(&ops, sources.data(), sources.size(), &array);
      ledger.allocation_failure_position = -1;
      if (status == ok) {
        completed = true;
        if (family == 0) assert(objc3_generic_map_release(map) == ok);
        if (family == 1) assert(objc3_generic_set_release(set) == ok);
        if (family == 2) assert(objc3_generic_array_release(array) == ok);
      } else {
        ++failures;
        assert(status == OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY && !map && !set && !array);
      }
      ledger.Balanced(); CheckSources(sources);
    }
    assert(completed && failures > 0);
  }
  for (unsigned operation = 0; operation != 3; ++operation) {
    bool completed = false; unsigned failures = 0;
    for (unsigned position = 0; position != 64 && !completed; ++position) {
      ledger.Reset();
      std::array<Payload, 33> initial{};
      objc3_generic_array_t array = nullptr;
      assert(objc3_generic_array_create(&ops, initial.data(), initial.size(), &array) == ok);
      objc3_generic_iterator_t iterator = nullptr;
      assert(objc3_generic_array_iterator(array, &iterator) == ok);
      const auto published = ledger.Live(); Payload replacement{99};
      ledger.allocation_failure_position = position;
      const auto status = operation == 0 ? objc3_generic_array_set(array, 0, &replacement)
          : operation == 1 ? objc3_generic_array_append(array, &replacement)
                           : objc3_generic_array_remove(array, 31);
      ledger.allocation_failure_position = -1;
      if (status == ok) completed = true;
      else {
        ++failures;
        assert(status == OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY);
        assert(ledger.Live() == published && "allocation rollback changed published ownership");
      }
      Payload output{}; int has = 0;
      const auto read = objc3_generic_iterator_next(iterator, &has, &output);
      if (completed) assert(read == OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION && !has && !output.instance);
      else { assert(read == ok && has && output.value == 0); Destroy(&output); }
      assert(objc3_generic_iterator_release(iterator) == ok);
      assert(objc3_generic_array_release(array) == ok); ledger.Balanced(); CheckSources(initial);
    }
    assert(completed && failures > 0);
  }
}

void ArrayScaling() {
  auto ops = Ops();
  for (unsigned count : {32u, 128u, 512u, 2048u}) {
    ledger.Reset();
    objc3_generic_array_t array = nullptr;
    assert(objc3_generic_array_create(&ops, nullptr, 0, &array) == ok);
#ifdef OBJC3_GENERIC_ARRAY_STORAGE_TESTING
    objc3_test_array_storage_reset();
#endif
    for (unsigned i = 0; i != count; ++i) {
      Payload source{static_cast<std::int64_t>(i)};
      assert(objc3_generic_array_append(array, &source) == ok && source.instance == 0);
    }
    const auto append_copies = ledger.copies.load();
    assert(append_copies >= count && append_copies <= 32 * count);
#ifdef OBJC3_GENERIC_ARRAY_STORAGE_TESTING
    assert(objc3_test_array_metadata_slots() <= 192 * count);
    assert(objc3_test_array_nodes() == 3 * count);
    assert(objc3_test_array_chunks() == count);
    objc3_test_array_storage_reset();
#endif
    const auto before = ledger.Live();
    const auto copies = ledger.copies.load();
    Payload replacement{-1};
    assert(objc3_generic_array_set(array, count / 2, &replacement) == ok);
    const auto set_copies = ledger.copies.load() - copies;
    assert(set_copies <= 32);
    std::printf("array-count=%u append-payload-copies=%u replacement-payload-copies=%u\n", count, append_copies, set_copies);
    const auto after = ledger.Live();
    unsigned preserved = 0;
    for (auto id : before) preserved += std::binary_search(after.begin(), after.end(), id);
    assert(preserved >= count - 32);
#ifdef OBJC3_GENERIC_ARRAY_STORAGE_TESTING
    assert(objc3_test_array_metadata_slots() <= 192);
    assert(objc3_test_array_nodes() == 3 && objc3_test_array_chunks() == 1);
#endif
    for (unsigned i : {0u, 31u, 32u, count / 2, count - 1}) {
      if (i >= count) continue;
      Payload output{};
      assert(objc3_generic_array_get(array, i, &output) == ok);
      assert(output.value == (i == count / 2 ? -1 : static_cast<std::int64_t>(i)));
      Destroy(&output);
    }
    assert(objc3_generic_array_release(array) == ok); ledger.Balanced();
    // Bulk input remains one ownership copy per element, with the same layout.
    ledger.Reset(); std::vector<Payload> source(count);
    assert(objc3_generic_array_create(&ops, source.data(), count, &array) == ok);
    assert(ledger.copies == count);
    assert(objc3_generic_array_release(array) == ok); ledger.Balanced();
  }
  ledger.Reset();
  std::vector<Payload> source(65);
  std::vector<std::int64_t> expected;
  for (unsigned i = 0; i != source.size(); ++i) { source[i].value = i; expected.push_back(i); }
  objc3_generic_array_t array = nullptr;
  assert(objc3_generic_array_create(&ops, source.data(), source.size(), &array) == ok);
  while (!expected.empty()) {
    const auto index = expected.size() > 32 ? 31 : expected.size() / 2;
    assert(objc3_generic_array_remove(array, index) == ok);
    expected.erase(expected.begin() + index);
    for (unsigned i = 0; i != expected.size(); ++i) {
      Payload output{};
      assert(objc3_generic_array_get(array, i, &output) == ok && output.value == expected[i]);
      Destroy(&output);
    }
  }
  std::uint64_t count = 99;
  assert(objc3_generic_array_count(array, &count) == ok && count == 0);
  assert(objc3_generic_array_release(array) == ok); ledger.Balanced();
}

std::mutex reader_mutex;
std::condition_variable reader_changed;
bool reader_entered = false, reader_continue = false;
thread_local bool gated_reader = false;
void GateReader() {
  if (!gated_reader) return;
  gated_reader = false;
  std::unique_lock lock(reader_mutex);
  reader_entered = true; reader_changed.notify_all();
  reader_changed.wait(lock, [] { return reader_continue; });
}
void ReaderRetirement() {
  auto ops = Ops(); auto descriptor = Map();
  for (unsigned route = 0; route != 4; ++route) {
    ledger.Reset();
    std::array<Payload, 40> sources{};
    for (unsigned i = 0; i != sources.size(); ++i) sources[i].value = i;
    objc3_generic_array_t array = nullptr;
    objc3_generic_map_t map = nullptr;
    objc3_generic_view_t view = nullptr;
    objc3_generic_iterator_t iterator = nullptr;
    objc3_generic_associative_iterator_t associative = nullptr;
    if (route == 3) {
      assert(objc3_generic_map_create(&descriptor, sources.data(), sources.data(), sources.size(), &map) == ok);
      assert(objc3_generic_map_iterator(map, &associative) == ok);
    } else {
      assert(objc3_generic_array_create(&ops, sources.data(), sources.size(), &array) == ok);
      if (route == 1) assert(objc3_generic_array_slice(array, 0, 1, &view) == ok);
      if (route == 2) assert(objc3_generic_array_iterator(array, &iterator) == ok);
    }
    const auto old_ids = ledger.Live();
    reader_entered = reader_continue = false;
    ledger.on_callback = GateReader;
    Payload output{}, output_key{}; int has = 0;
    auto status = OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
    std::thread reader([&] {
      gated_reader = true;
      if (route == 0) status = objc3_generic_array_get(array, 0, &output);
      if (route == 1) status = objc3_generic_view_get(view, 0, &output);
      if (route == 2) status = objc3_generic_iterator_next(iterator, &has, &output);
      if (route == 3) status = objc3_generic_associative_iterator_next(associative, &has, &output_key, &output);
    });
    { std::unique_lock lock(reader_mutex); reader_changed.wait(lock, [] { return reader_entered; }); }
    Payload replacement{999};
    if (route == 3) {
      int inserted = 1;
      assert(objc3_generic_map_insert(map, &sources[0], &replacement, &inserted) == ok && inserted == 0);
      assert(objc3_generic_map_release(map) == ok);
      assert(objc3_generic_associative_iterator_release(associative) == ok);
    } else {
      assert(objc3_generic_array_set(array, 0, &replacement) == ok);
      assert(objc3_generic_array_release(array) == ok);
      if (view) assert(objc3_generic_view_release(view) == ok);
      if (iterator) assert(objc3_generic_iterator_release(iterator) == ok);
    }
    // All old logical instances stay alive while the callback pins its version.
    const auto during = ledger.Live();
    for (auto id : old_ids) assert(std::binary_search(during.begin(), during.end(), id));
    { std::lock_guard lock(reader_mutex); reader_continue = true; }
    reader_changed.notify_all(); reader.join(); ledger.on_callback = nullptr;
    if (route == 0) {
      assert(status == ok && output.value == 0 && ledger.Live().size() == 1);
      Destroy(&output);
    } else {
      assert(status == OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION && !has);
      assert(!output.instance && !output_key.instance);
    }
    ledger.Balanced(); CheckSources(sources);
  }
}

objc3_generic_array_t reentrant_array = nullptr;
bool reentered_array = false;
void AppendReentrantly() {
  if (reentered_array) return;
  reentered_array = true;
  Payload next{777};
  assert(objc3_generic_array_append(reentrant_array, &next) == ok);
}
void ArrayReentry() {
  for (bool destruction : {false, true}) {
    ledger.Reset(); reentered_array = false;
    auto ops = Ops(); std::array<Payload, 33> source{};
    assert(objc3_generic_array_create(&ops, source.data(), source.size(), &reentrant_array) == ok);
    if (destruction) ledger.on_destroy = AppendReentrantly;
    else ledger.on_callback = AppendReentrantly;
    Payload replacement{123};
    const auto status = objc3_generic_array_set(reentrant_array, 0, &replacement);
    ledger.on_callback = ledger.on_destroy = nullptr;
    assert(reentered_array);
    assert(status == (destruction ? ok : OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION));
    std::uint64_t count = 0; Payload output{};
    assert(objc3_generic_array_count(reentrant_array, &count) == ok && count == 34);
    assert(objc3_generic_array_get(reentrant_array, 0, &output) == ok && output.value == (destruction ? 123 : 0));
    Destroy(&output);
    assert(objc3_generic_array_get(reentrant_array, 33, &output) == ok && output.value == 777);
    Destroy(&output);
    assert(objc3_generic_array_release(reentrant_array) == ok);
    reentrant_array = nullptr; ledger.Balanced();
  }
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

int main() { Rollback(); BulkCopiesAndOrder(); ZeroSizedAndBounds(); AllocationSweeps(); ArrayScaling(); ReaderRetirement(); ArrayReentry(); Reentry(); ledger.Balanced(); }
