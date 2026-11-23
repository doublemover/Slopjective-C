#include <objc3c/runtime/objc3_collections.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string_view>
#include <type_traits>

namespace {
enum Phase { construct, lookup, duplicate, iterate, remove, teardown, phase_count };
constexpr std::string_view phases[] = {
    "construct", "lookup", "duplicate", "iterate", "remove", "teardown"};
static_assert(std::size(phases) == phase_count);

struct Sample {
  std::uint64_t operations = 0;
  std::uint64_t checksum = 0;
#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
  std::uint64_t allocations = 0;
  std::uint64_t allocated_bytes = 0;
#else
  std::uint64_t nanoseconds = 0;
#endif
};

#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
Sample *active_sample = nullptr;
#endif

void require(bool condition) {
  if (!condition) {
    std::fputs("collection scaling workload invariant failed\n", stderr);
    std::exit(3);
  }
}

void checked(int status) {
  require(status == OBJC3_STDLIB_V2_OK);
}

template <typename Action>
void measure(Sample &sample, Action action) {
#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
  active_sample = &sample;
  action();
  active_sample = nullptr;
#else
  const auto start = std::chrono::steady_clock::now();
  action();
  sample.nanoseconds += std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now() - start).count();
#endif
}

template <bool Map>
void run(std::array<Sample, phase_count> &samples, int32_t size) {
  std::conditional_t<Map, objc3_map_i32_i32_v2_t, objc3_set_i32_v2_t> collection = nullptr;
  const auto count = [&] {
    std::uint64_t result = 0;
    if constexpr (Map) checked(objc3_runtime_stdlib_map_i32_i32_v2_count(collection, &result));
    else checked(objc3_runtime_stdlib_set_i32_v2_count(collection, &result));
    return result;
  };
  const auto insert = [&](int32_t key, bool duplicate) {
    if constexpr (Map) {
      std::uint64_t result = 0;
      checked(objc3_runtime_stdlib_map_i32_i32_v2_insert_count(
          collection, key, 3 * key + duplicate, &result));
      require(result == static_cast<std::uint64_t>(duplicate ? size : key + 1));
    } else {
      int inserted = -1;
      checked(objc3_runtime_stdlib_set_i32_v2_insert(collection, key, &inserted));
      require(inserted == !duplicate);
    }
  };
  measure(samples[construct], [&] {
    if constexpr (Map) checked(objc3_runtime_stdlib_map_i32_i32_v2_create(&collection));
    else checked(objc3_runtime_stdlib_set_i32_v2_create(&collection));
    for (int32_t key = 0; key < size; ++key) insert(key, false);
  });
  require(count() == static_cast<std::uint64_t>(size));
  samples[construct].operations += size + 1;

  measure(samples[lookup], [&] {
    for (int32_t key = 0; key < size; ++key) {
      int found = -1;
      if constexpr (Map) {
        int32_t value = -1;
        checked(objc3_runtime_stdlib_map_i32_i32_v2_get(collection, key, &found, &value));
        require(found == 1 && value == 3 * key);
        samples[lookup].checksum += value;
        checked(objc3_runtime_stdlib_map_i32_i32_v2_get(collection, -key - 1, &found, &value));
        require(found == 0 && value == 0);
      } else {
        checked(objc3_runtime_stdlib_set_i32_v2_contains(collection, key, &found));
        require(found == 1);
        samples[lookup].checksum += found;
        checked(objc3_runtime_stdlib_set_i32_v2_contains(collection, -key - 1, &found));
        require(found == 0);
      }
    }
  });
  samples[lookup].operations += 2 * size;

  measure(samples[duplicate], [&] {
    for (int32_t key = 0; key < size; ++key) insert(key, true);
  });
  require(count() == static_cast<std::uint64_t>(size));
  samples[duplicate].operations += size;

  measure(samples[iterate], [&] {
    objc3_iterator_i32_v2_t iterator = nullptr;
    if constexpr (Map) checked(objc3_runtime_stdlib_map_i32_i32_v2_iterator(collection, &iterator));
    else checked(objc3_runtime_stdlib_set_i32_v2_iterator(collection, &iterator));
    for (int32_t key = 0; key <= size; ++key) {
      int32_t item = -1, value = -1;
      int has = -1;
      if constexpr (Map) checked(objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(
          iterator, &has, &item, &value));
      else checked(objc3_runtime_stdlib_iterator_i32_v2_next(iterator, &has, &item));
      require(has == (key != size));
      if (has) {
        require(item == key);
        if constexpr (Map) require(value == 3 * key + 1);
        samples[iterate].checksum += item + (Map ? value : 0);
      }
    }
    checked(objc3_runtime_stdlib_iterator_i32_v2_release(iterator));
  });
  samples[iterate].operations += size + 3;

  const auto removals = std::max(1, size / 4);
  measure(samples[remove], [&] {
    for (int32_t key = 0; key < removals; ++key) {
      int removed = -1;
      if constexpr (Map) checked(objc3_runtime_stdlib_map_i32_i32_v2_remove(collection, key, &removed));
      else checked(objc3_runtime_stdlib_set_i32_v2_remove(collection, key, &removed));
      require(removed == 1);
    }
  });
  require(count() == static_cast<std::uint64_t>(size - removals));
  samples[remove].operations += removals;
  for (int32_t key = 0; key < size; ++key) {
    int found = -1;
    if constexpr (Map) checked(objc3_runtime_stdlib_map_i32_i32_v2_contains(collection, key, &found));
    else checked(objc3_runtime_stdlib_set_i32_v2_contains(collection, key, &found));
    require(found == (key >= removals));
  }
  measure(samples[teardown], [&] {
    if constexpr (Map) checked(objc3_runtime_stdlib_map_i32_i32_v2_release(collection));
    else checked(objc3_runtime_stdlib_set_i32_v2_release(collection));
  });
  ++samples[teardown].operations;
}

int argument(const char *text, long maximum) {
  char *end = nullptr;
  const long value = std::strtol(text, &end, 10);
  if (!*text || *end || value < 1 || value > maximum) std::exit(2);
  return static_cast<int>(value);
}
}  // namespace

#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
extern "C" void *__real__Znwm(std::size_t);
extern "C" void *__wrap__Znwm(std::size_t size) {
  if (active_sample) {
    ++active_sample->allocations;
    active_sample->allocated_bytes += size;
  }
  return __real__Znwm(size);
}
#endif

int main(int argc, char **argv) {
  if (argc != 4) return 2;
  const std::string_view target = argv[1];
  const int size = argument(argv[2], 2048), rounds = argument(argv[3], 128);
  if (target != "map" && target != "set") return 2;
  std::array<Sample, phase_count> samples;
  for (int round = 0; round < rounds; ++round) {
    if (target == "map") run<true>(samples, size);
    else run<false>(samples, size);
  }
  std::putchar('{');
  for (std::size_t i = 0; i < samples.size(); ++i) {
    const auto &sample = samples[i];
    std::printf("%s\"%s\":{\"completed_operations\":%llu,\"checksum\":%llu,",
        i ? "," : "", phases[i].data(),
        static_cast<unsigned long long>(sample.operations),
        static_cast<unsigned long long>(sample.checksum));
#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
    std::printf("\"allocations\":%llu,\"allocated_bytes\":%llu}",
        static_cast<unsigned long long>(sample.allocations),
        static_cast<unsigned long long>(sample.allocated_bytes));
#else
    std::printf("\"nanoseconds\":%llu}", static_cast<unsigned long long>(sample.nanoseconds));
#endif
  }
  std::puts("}");
}
