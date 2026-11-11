#include "runtime/public/objc3_collections.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <thread>
#include <map>
#include <atomic>
#include <new>
#include <limits>
#include <vector>

static void check(bool condition) {
  if (!condition) {
    std::fputs("concurrent map-count contract failed\n", stderr);
    std::abort();
  }
}

#ifdef OBJC3_RUNTIME_I32_INDEX_TESTING
static std::atomic<long> fail_allocation_after{-1};
static unsigned allocations_before_failure = 0;
extern "C" void *__real__Znwm(std::size_t);
extern "C" void *__wrap__Znwm(std::size_t size) {
  const auto remaining = fail_allocation_after.load();
  if (remaining >= 0) {
    if (fail_allocation_after.fetch_sub(1) == 0) throw std::bad_alloc();
    ++allocations_before_failure;
  }
  return __real__Znwm(size);
}
static void index_allocation_rollback() {
  bool failed_after_preparation = false, succeeded = false;
  for (bool map_case : {false, true}) for (long fail_at = 0; fail_at != 8; ++fail_at) {
    objc3_map_i32_i32_v2_t map = nullptr;
    objc3_set_i32_v2_t set = nullptr;
    objc3_iterator_i32_v2_t iterator = nullptr;
    if (map_case) {
      check(objc3_runtime_stdlib_map_i32_i32_v2_create(&map) == 0);
      check(objc3_runtime_stdlib_map_i32_i32_v2_iterator(map, &iterator) == 0);
    } else {
      check(objc3_runtime_stdlib_set_i32_v2_create(&set) == 0);
      check(objc3_runtime_stdlib_set_i32_v2_iterator(set, &iterator) == 0);
    }
    uint64_t inserted_count = 99;
    int inserted = -1;
    allocations_before_failure = 0;
    fail_allocation_after = fail_at;
    const auto status = map_case ?
        objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, 42, 17, &inserted_count) :
        objc3_runtime_stdlib_set_i32_v2_insert(set, 42, &inserted);
    fail_allocation_after = -1;
    check(status == 0 || status == OBJC3_STDLIB_V2_OUT_OF_MEMORY);
    uint64_t count = 99;
    int found = -1, has_value = -1;
    int32_t key = -1, value = -1;
    if (map_case) {
      check(objc3_runtime_stdlib_map_i32_i32_v2_count(map, &count) == 0);
      check(objc3_runtime_stdlib_map_i32_i32_v2_contains(map, 42, &found) == 0);
    } else {
      check(objc3_runtime_stdlib_set_i32_v2_count(set, &count) == 0);
      check(objc3_runtime_stdlib_set_i32_v2_contains(set, 42, &found) == 0);
    }
    if (status == 0) {
      succeeded = true;
      check(count == 1 && found == 1);
    } else {
      failed_after_preparation |= allocations_before_failure != 0;
      check(count == 0 && found == 0);
      check(map_case ? inserted_count == 0 : inserted == 0);
      // Failed allocating preparation cannot publish a semantic mutation.
      check((map_case ? objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(iterator, &has_value, &key, &value) :
             objc3_runtime_stdlib_iterator_i32_v2_next(iterator, &has_value, &key)) == 0 && !has_value);
      if (map_case) check(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, 42, 17, &inserted_count) == 0 && inserted_count == 1);
      else check(objc3_runtime_stdlib_set_i32_v2_insert(set, 42, &inserted) == 0 && inserted == 1);
    }
    check((map_case ? objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(iterator, &has_value, &key, &value) :
           objc3_runtime_stdlib_iterator_i32_v2_next(iterator, &has_value, &key)) == OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION);
    check(objc3_runtime_stdlib_iterator_i32_v2_release(iterator) == 0);
    if (map_case) check(objc3_runtime_stdlib_map_i32_i32_v2_release(map) == 0);
    else check(objc3_runtime_stdlib_set_i32_v2_release(set) == 0);
  }
  check(failed_after_preparation && succeeded);
}
extern "C" void objc3_test_i32_index_reset();
extern "C" uint64_t objc3_test_i32_index_hash_calls();
extern "C" uint64_t objc3_test_i32_index_equal_calls();
static void indexed_work() {
  objc3_map_i32_i32_v2_t map = nullptr;
  objc3_set_i32_v2_t set = nullptr;
  check(objc3_runtime_stdlib_map_i32_i32_v2_create(&map) == 0);
  check(objc3_runtime_stdlib_set_i32_v2_create(&set) == 0);
  constexpr int32_t size = 16384;
  objc3_test_i32_index_reset();
  for (int32_t key = 0; key < size; ++key) {
    uint64_t count = 0;
    int inserted = 0;
    check(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, key, key ^ 42, &count) == 0);
    check(count == static_cast<uint64_t>(key + 1));
    check(objc3_runtime_stdlib_set_i32_v2_insert(set, key, &inserted) == 0 && inserted == 1);
  }
  const auto construction_work = objc3_test_i32_index_hash_calls() + objc3_test_i32_index_equal_calls();
  check(construction_work > size && construction_work < 64 * size);
  objc3_test_i32_index_reset();
  for (int32_t key = 0; key < size; ++key) {
    int found = 0;
    int32_t value = 0;
    check(objc3_runtime_stdlib_map_i32_i32_v2_get(map, key, &found, &value) == 0);
    check(found == 1 && value == (key ^ 42));
    check(objc3_runtime_stdlib_map_i32_i32_v2_contains(map, -key - 1, &found) == 0 && found == 0);
    check(objc3_runtime_stdlib_set_i32_v2_contains(set, key, &found) == 0 && found == 1);
    check(objc3_runtime_stdlib_set_i32_v2_contains(set, -key - 1, &found) == 0 && found == 0);
  }
  const auto query_work = objc3_test_i32_index_hash_calls() + objc3_test_i32_index_equal_calls();
  check(objc3_test_i32_index_hash_calls() >= size && query_work < 64 * size);
  std::printf("i32 index: %d entries, %llu construction probes, %llu query probes\n",
              size, static_cast<unsigned long long>(construction_work),
              static_cast<unsigned long long>(query_work));
  check(objc3_runtime_stdlib_map_i32_i32_v2_release(map) == 0);
  check(objc3_runtime_stdlib_set_i32_v2_release(set) == 0);
}
#endif
static void ordered_index_oracle() {
  objc3_map_i32_i32_v2_t map = nullptr;
  objc3_set_i32_v2_t set = nullptr;
  check(objc3_runtime_stdlib_map_i32_i32_v2_create(&map) == 0);
  check(objc3_runtime_stdlib_set_i32_v2_create(&set) == 0);
  std::map<int32_t, int32_t> expected;
  std::vector<int32_t> order;
  uint32_t random = 0x4f424a33;
  for (unsigned step = 0; step != 12000; ++step) {
    random ^= random << 13; random ^= random >> 17; random ^= random << 5;
    const auto key = step % 97 == 0 ? std::numeric_limits<int32_t>::min() :
        step % 89 == 0 ? std::numeric_limits<int32_t>::max() : static_cast<int32_t>(random % 257) - 128;
    const auto value = static_cast<int32_t>(random & 0xffff);
    if ((random & 3) < 2) {
      const bool inserted = !expected.count(key);
      if (inserted) order.push_back(key);
      expected[key] = value;
      uint64_t count = 0;
      int set_inserted = -1;
      check(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, key, value, &count) == 0);
      check(count == expected.size());
      check(objc3_runtime_stdlib_set_i32_v2_insert(set, key, &set_inserted) == 0);
      check(set_inserted == inserted);
    } else if ((random & 3) == 2) {
      const bool erased = expected.erase(key);
      if (erased) std::erase(order, key);
      int removed = -1;
      check(objc3_runtime_stdlib_map_i32_i32_v2_remove(map, key, &removed) == 0 && removed == erased);
      check(objc3_runtime_stdlib_set_i32_v2_remove(set, key, &removed) == 0 && removed == erased);
    } else {
      int found = -1;
      int32_t observed = -1;
      const auto item = expected.find(key);
      check(objc3_runtime_stdlib_map_i32_i32_v2_get(map, key, &found, &observed) == 0);
      check(found == (item != expected.end()));
      check(observed == (item == expected.end() ? 0 : item->second));
      check(objc3_runtime_stdlib_set_i32_v2_contains(set, key, &found) == 0 && found == (item != expected.end()));
    }
    uint64_t count = 0;
    check(objc3_runtime_stdlib_map_i32_i32_v2_count(map, &count) == 0 && count == expected.size());
    check(objc3_runtime_stdlib_set_i32_v2_count(set, &count) == 0 && count == expected.size());
    if (step % 67 != 0) continue;
    objc3_iterator_i32_v2_t map_it = nullptr, set_it = nullptr;
    check(objc3_runtime_stdlib_map_i32_i32_v2_iterator(map, &map_it) == 0);
    check(objc3_runtime_stdlib_set_i32_v2_iterator(set, &set_it) == 0);
    // All three no-op mutations preserve the iterators and insertion order.
    if (!order.empty()) {
      uint64_t ignored = 0;
      int inserted = -1;
      check(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, order[0], expected.at(order[0]), &ignored) == 0);
      check(objc3_runtime_stdlib_set_i32_v2_insert(set, order[0], &inserted) == 0 && inserted == 0);
    }
    int removed = -1;
    check(objc3_runtime_stdlib_map_i32_i32_v2_remove(map, 1000000, &removed) == 0 && removed == 0);
    check(objc3_runtime_stdlib_set_i32_v2_remove(set, 1000000, &removed) == 0 && removed == 0);
    for (int32_t key : order) {
      int has_value = 0;
      int32_t observed_key = 0, observed_value = 0;
      check(objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(map_it, &has_value, &observed_key, &observed_value) == 0);
      check(has_value == 1 && observed_key == key && observed_value == expected.at(key));
      check(objc3_runtime_stdlib_iterator_i32_v2_next(set_it, &has_value, &observed_key) == 0);
      check(has_value == 1 && observed_key == key);
    }
    int has_value = 1;
    int32_t observed_key = -1, observed_value = -1;
    check(objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(map_it, &has_value, &observed_key, &observed_value) == 0 && !has_value);
    check(objc3_runtime_stdlib_iterator_i32_v2_next(set_it, &has_value, &observed_key) == 0 && !has_value);
    uint64_t ignored = 0;
    check(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, 1000000, 42, &ignored) == 0);
    int inserted = 0;
    check(objc3_runtime_stdlib_set_i32_v2_insert(set, 1000000, &inserted) == 0 && inserted == 1);
    check(objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(map_it, &has_value, &observed_key, &observed_value) == OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION);
    check(objc3_runtime_stdlib_iterator_i32_v2_next(set_it, &has_value, &observed_key) == OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION);
    check(objc3_runtime_stdlib_map_i32_i32_v2_remove(map, 1000000, &removed) == 0 && removed == 1);
    check(objc3_runtime_stdlib_set_i32_v2_remove(set, 1000000, &removed) == 0 && removed == 1);
    check(objc3_runtime_stdlib_iterator_i32_v2_release(map_it) == 0);
    check(objc3_runtime_stdlib_iterator_i32_v2_release(set_it) == 0);
  }
  check(objc3_runtime_stdlib_map_i32_i32_v2_release(map) == 0);
  check(objc3_runtime_stdlib_set_i32_v2_release(set) == 0);
}

int main() {
  ordered_index_oracle();
#ifdef OBJC3_RUNTIME_I32_INDEX_TESTING
  index_allocation_rollback();
  indexed_work();
#endif
  objc3_map_i32_i32_v2_t map = nullptr;
  check(objc3_runtime_stdlib_map_i32_i32_v2_create(&map) == OBJC3_STDLIB_V2_OK);
  constexpr unsigned workers = 4, per_worker = 64;
  std::array<uint64_t, workers * per_worker> counts{};
  std::vector<std::thread> threads;
  for (unsigned worker = 0; worker != workers; ++worker) {
    threads.emplace_back([&, worker] {
      for (unsigned item = 0; item != per_worker; ++item) {
        const unsigned key = worker * per_worker + item;
        check(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(
                   map, static_cast<int32_t>(key), 42, &counts[key]) ==
               OBJC3_STDLIB_V2_OK);
      }
    });
  }
  for (auto &thread : threads) thread.join();
  // Each insertion's result must belong to its own locked mutation. Separate
  // insert/count operations can duplicate or omit counts under this schedule.
  std::sort(counts.begin(), counts.end());
  for (unsigned index = 0; index != counts.size(); ++index)
    check(counts[index] == index + 1);
  uint64_t count = 0;
  check(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, 0, 43, &count) == 0);
  check(count == counts.size());
  check(objc3_runtime_stdlib_map_i32_i32_v2_release(map) == 0);
  count = 99;
  check(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, 0, 44, &count) ==
         OBJC3_STDLIB_V2_STALE_HANDLE);
  check(count == 0);
}
