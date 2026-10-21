#include "runtime/public/objc3_collections.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <thread>
#include <vector>

int main() {
  objc3_map_i32_i32_v2_t map = nullptr;
  assert(objc3_runtime_stdlib_map_i32_i32_v2_create(&map) == OBJC3_STDLIB_V2_OK);
  constexpr unsigned workers = 4, per_worker = 64;
  std::array<uint64_t, workers * per_worker> counts{};
  std::vector<std::thread> threads;
  for (unsigned worker = 0; worker != workers; ++worker) {
    threads.emplace_back([&, worker] {
      for (unsigned item = 0; item != per_worker; ++item) {
        const unsigned key = worker * per_worker + item;
        assert(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(
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
    assert(counts[index] == index + 1);
  uint64_t count = 0;
  assert(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, 0, 43, &count) == 0);
  assert(count == counts.size());
  assert(objc3_runtime_stdlib_map_i32_i32_v2_release(map) == 0);
  count = 99;
  assert(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, 0, 44, &count) ==
         OBJC3_STDLIB_V2_STALE_HANDLE);
  assert(count == 0);
}
