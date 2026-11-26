#include <objc3c/runtime/objc3_collections.h>
#include <objc3c/runtime/objc3_text.h>
#include <objc3c/runtime/generic_associative_service.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
extern "C" std::size_t objc3_test_i32_index_retained_bytes();
extern "C" void objc3_test_array_storage_reset();
extern "C" uint64_t objc3_test_array_metadata_slots();
extern "C" uint64_t objc3_test_array_nodes();
extern "C" void objc3_test_text_construction_reset();
extern "C" uint64_t objc3_test_text_decoded_input_bytes();
extern "C" uint64_t objc3_test_text_decode_calls();
extern "C" uint64_t objc3_test_text_copied_bytes();
#endif

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
  std::uint64_t key_copies = 0, value_copies = 0, destroys = 0, equals = 0;
  std::uint64_t peak_payload_bytes = 0;
  std::uint64_t retained_index_bytes = 0, index_bytes_before = 0;
  std::uint64_t metadata_slots = 0, metadata_nodes = 0;
  std::uint64_t decoded_bytes = 0, decoder_calls = 0, copied_bytes = 0;
#else
  std::uint64_t nanoseconds = 0;
#endif
};

#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
Sample *active_sample = nullptr;

void record_allocation(std::size_t bytes) {
  if (active_sample) {
    ++active_sample->allocations;
    active_sample->allocated_bytes += bytes;
  }
}
#endif

void require(bool condition, std::source_location site = std::source_location::current()) {
  if (!condition) {
    std::fprintf(stderr, "collection scaling workload invariant failed at line %u\n", site.line());
    std::exit(3);
  }
}

void checked(int status, std::source_location site = std::source_location::current()) {
  require(status == OBJC3_STDLIB_V2_OK, site);
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
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
  const auto full_index_bytes = objc3_test_i32_index_retained_bytes();
  require(full_index_bytes > 0);
  samples[construct].retained_index_bytes += full_index_bytes;
#endif

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
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
  require(objc3_test_i32_index_retained_bytes() == full_index_bytes);
#endif

  measure(samples[duplicate], [&] {
    for (int32_t key = 0; key < size; ++key) insert(key, true);
  });
  require(count() == static_cast<std::uint64_t>(size));
  samples[duplicate].operations += size;
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
  require(objc3_test_i32_index_retained_bytes() == full_index_bytes);
#endif

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
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
  require(objc3_test_i32_index_retained_bytes() == full_index_bytes);
#endif

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
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
  const auto remaining_index_bytes = objc3_test_i32_index_retained_bytes();
  require(remaining_index_bytes > 0 && remaining_index_bytes < full_index_bytes);
  samples[remove].retained_index_bytes += remaining_index_bytes;
  samples[remove].index_bytes_before += full_index_bytes;
#endif
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
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
  require(objc3_test_i32_index_retained_bytes() == 0);
  samples[teardown].index_bytes_before += remaining_index_bytes;
#endif
}

struct PayloadLedger {
  std::size_t live = 0;
  std::size_t live_bytes = 0;
  int fail_copy_after = -1;
  bool collide = false;

  void ObservePeak() const {
#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
    if (active_sample)
      active_sample->peak_payload_bytes =
          std::max<std::uint64_t>(active_sample->peak_payload_bytes, live_bytes);
#endif
  }
};
PayloadLedger payloads;

template <bool OwnsString>
struct Payload {
  std::int64_t value = 0;
  std::conditional_t<OwnsString, std::string, bool> resource;
  bool runtime_owned = false;

  explicit Payload(std::int64_t number) : value(number) {
    if constexpr (OwnsString) resource.assign(80, char('a' + number % 26));
    else resource = false;
  }
  bool valid() const {
    if constexpr (OwnsString)
      return resource.size() == 80 &&
             std::all_of(resource.begin(), resource.end(),
                         [&](char byte) { return byte == char('a' + value % 26); });
    return true;
  }
  std::size_t owned_bytes() const {
    if constexpr (OwnsString) return sizeof(Payload) + resource.capacity() + 1;
    return sizeof(Payload);
  }
};

template <typename Value, bool Key>
objc3_service_status CopyPayload(void *destination, const void *source) {
  if (payloads.fail_copy_after == 0) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (payloads.fail_copy_after > 0) --payloads.fail_copy_after;
  Value *copy;
  try {
    copy = new (destination) Value(*static_cast<const Value *>(source));
  } catch (const std::bad_alloc &) {
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  }
  copy->runtime_owned = true;
  ++payloads.live;
  payloads.live_bytes += copy->owned_bytes();
#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
  if (active_sample) {
    if constexpr (Key) ++active_sample->key_copies;
    else ++active_sample->value_copies;
  }
#endif
  payloads.ObservePeak();
  return OBJC3_SERVICE_OK;
}

template <typename Value>
void DestroyPayload(void *value) {
  auto &payload = *static_cast<Value *>(value);
  require(payload.runtime_owned && payloads.live > 0 &&
          payloads.live_bytes >= payload.owned_bytes());
  --payloads.live;
  payloads.live_bytes -= payload.owned_bytes();
#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
  if (active_sample) ++active_sample->destroys;
#endif
  payload.~Value();
}

template <typename Value, bool Key>
objc3_service_status MovePayload(void *destination, void *source) {
  const auto status = CopyPayload<Value, Key>(destination, source);
  if (status == OBJC3_SERVICE_OK) {
    auto &payload = *static_cast<Value *>(source);
    if (payload.runtime_owned) DestroyPayload<Value>(source);
    else payload.~Value();
  }
  return status;
}

template <typename Value, bool Key = false>
constexpr objc3_value_ops ops = {sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION,
    sizeof(Value), alignof(Value), CopyPayload<Value, Key>, MovePayload<Value, Key>,
    DestroyPayload<Value>};

template <typename Value>
objc3_service_status HashPayload(const void *key, std::uint64_t *hash) {
  *hash = payloads.collide ? 0 : static_cast<const Value *>(key)->value;
  return OBJC3_SERVICE_OK;
}

template <typename Value>
objc3_service_status EqualPayload(const void *left, const void *right, int *equal) {
  *equal = static_cast<const Value *>(left)->value == static_cast<const Value *>(right)->value;
#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
  if (active_sample) ++active_sample->equals;
#endif
  return OBJC3_SERVICE_OK;
}

void check_payloads_released() {
  require(!payloads.live && !payloads.live_bytes);
}

constexpr std::array associative_names = {"unique-create", "unique-release",
    "duplicate-create", "duplicate-release", "collision-create", "collision-release", "rollback"};
using AssociativeSamples = std::array<Sample, associative_names.size()>;

template <typename Value, bool Map>
void run_associative(AssociativeSamples &samples, int32_t size, int rounds) {
  using Handle = std::conditional_t<Map, objc3_generic_map_t, objc3_generic_set_t>;
  const objc3_generic_map_descriptor map_descriptor = {sizeof(objc3_generic_map_descriptor),
      OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION, ops<Value, true>, ops<Value>,
      HashPayload<Value>, EqualPayload<Value>};
  const objc3_generic_set_descriptor set_descriptor = {sizeof(objc3_generic_set_descriptor),
      OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION, ops<Value, true>,
      HashPayload<Value>, EqualPayload<Value>};
  const auto create = [&](const std::vector<Value> &keys, const std::vector<Value> &values,
                          Handle *out) {
    if constexpr (Map)
      return objc3_generic_map_create(&map_descriptor, keys.data(), values.data(), size, out);
    else
      return objc3_generic_set_create(&set_descriptor, keys.data(), size, out);
  };
  std::vector<Value> unique, duplicate, values;
  const int32_t repeated = (size + 3) / 4;
  for (int32_t key = 0; key < size; ++key) {
    unique.emplace_back(key);
    duplicate.emplace_back(key % repeated);
    if constexpr (Map) values.emplace_back(3 * key + 1);
  }
  for (int round = 0; round < rounds; ++round) {
    for (unsigned shape = 0; shape != 3; ++shape) {
      const int32_t distinct = shape == 1 ? repeated : size;
      const auto &keys = shape == 1 ? duplicate : unique;
      auto &created = samples[2 * shape];
      auto &released = samples[2 * shape + 1];
      Handle collection = nullptr;
      payloads.collide = shape == 2;
      measure(created, [&] { checked(create(keys, values, &collection)); });
      created.operations += size + 1;
      require(payloads.live == static_cast<std::size_t>(distinct * (Map ? 2 : 1)));
      std::uint64_t count = 0;
      objc3_generic_associative_iterator_t iterator = nullptr;
      if constexpr (Map) {
        checked(objc3_generic_map_count(collection, &count));
        checked(objc3_generic_map_iterator(collection, &iterator));
      } else {
        checked(objc3_generic_set_count(collection, &count));
        checked(objc3_generic_set_iterator(collection, &iterator));
      }
      require(count == static_cast<std::uint64_t>(distinct));
      alignas(Value) std::byte key_storage[sizeof(Value)], value_storage[sizeof(Value)];
      int has = 0;
      for (int32_t i = 0; i < distinct; ++i) {
        checked(objc3_generic_associative_iterator_next(iterator, &has, key_storage,
                                                       Map ? value_storage : nullptr));
        const auto &key = *reinterpret_cast<Value *>(key_storage);
        require(has && key.value == i && key.valid());
        created.checksum += key.value;
        DestroyPayload<Value>(key_storage);
        if constexpr (Map) {
          const auto &value = *reinterpret_cast<Value *>(value_storage);
          const auto last = i + (size - 1 - i) / distinct * distinct;
          require(value.value == 3 * last + 1 && value.valid());
          created.checksum += value.value;
          DestroyPayload<Value>(value_storage);
        }
      }
      checked(objc3_generic_associative_iterator_next(iterator, &has, key_storage,
                                                     Map ? value_storage : nullptr));
      require(!has);
      checked(objc3_generic_associative_iterator_release(iterator));
      measure(released, [&] {
        payloads.ObservePeak();
        if constexpr (Map) checked(objc3_generic_map_release(collection));
        else checked(objc3_generic_set_release(collection));
      });
      ++released.operations;
      check_payloads_released();
    }
    payloads.collide = false;
    payloads.fail_copy_after = size / 2 * (Map ? 2 : 1) + (Map ? 1 : 0);
    auto *rejected = reinterpret_cast<Handle>(1);
    measure(samples.back(), [&] {
      require(create(unique, values, &rejected) == OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT);
    });
    require(!rejected && payloads.fail_copy_after == 0);
    samples.back().operations += size / 2 + 1;
    payloads.fail_copy_after = -1;
    check_payloads_released();
  }
}

constexpr std::array array_names = {"append", "replace", "get", "iterate", "release",
                                    "bulk", "bulk-get", "bulk-iterate", "bulk-release"};
using ArraySamples = std::array<Sample, array_names.size()>;

template <typename Action>
void measure_array(Sample &sample, Action action) {
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
  objc3_test_array_storage_reset();
#endif
  measure(sample, action);
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
  sample.metadata_slots += objc3_test_array_metadata_slots();
  sample.metadata_nodes += objc3_test_array_nodes();
#endif
}

template <typename Value>
void run_array(ArraySamples &samples, int32_t size, int rounds) {
  std::vector<Value> values, replacements;
  const auto middle = size / 2;
  const auto writes = std::max(1, size / 4);
  for (int32_t i = 0; i != size; ++i) values.emplace_back(i);
  for (int32_t i = 0; i != writes; ++i) replacements.emplace_back(size + i);
  for (int round = 0; round < rounds; ++round) {
    objc3_generic_array_t array = nullptr;
    const auto read = [&](Sample &sample, bool replaced, bool iterating) {
      std::uint64_t count = 0;
      checked(objc3_generic_array_count(array, &count));
      require(count == static_cast<std::uint64_t>(size));
      alignas(Value) std::byte storage[sizeof(Value)];
      measure_array(sample, [&] {
        objc3_generic_iterator_t iterator = nullptr;
        int has = 0;
        if (iterating) checked(objc3_generic_array_iterator(array, &iterator));
        for (int32_t i = 0; i < size; ++i) {
          if (iterating) {
            checked(objc3_generic_iterator_next(iterator, &has, storage));
            require(has);
          } else {
            checked(objc3_generic_array_get(array, i, storage));
          }
          const auto &value = *reinterpret_cast<Value *>(storage);
          require(value.valid() && value.value == (replaced && i == middle ? size + writes - 1 : i));
          sample.checksum += value.value;
          DestroyPayload<Value>(storage);
        }
        if (iterating) {
          checked(objc3_generic_iterator_next(iterator, &has, storage));
          require(!has);
          checked(objc3_generic_iterator_release(iterator));
        }
      });
      sample.operations += size + (iterating ? 3 : 0);
    };
    const auto release = [&](Sample &sample) {
      measure_array(sample, [&] {
        payloads.ObservePeak();
        checked(objc3_generic_array_release(array));
      });
      ++sample.operations;
      check_payloads_released();
    };
    measure_array(samples[0], [&] {
      checked(objc3_generic_array_create(&ops<Value>, nullptr, 0, &array));
      for (const auto &value : values) checked(objc3_generic_array_append(array, &value));
    });
    samples[0].operations += size + 1;
    require(payloads.live == static_cast<std::size_t>(size));
    measure_array(samples[1], [&] {
      for (const auto &value : replacements) checked(objc3_generic_array_set(array, middle, &value));
    });
    samples[1].operations += writes;
    require(payloads.live == static_cast<std::size_t>(size));
    read(samples[2], true, false);
    read(samples[3], true, true);
    release(samples[4]);
    measure_array(samples[5], [&] {
      checked(objc3_generic_array_create(&ops<Value>, values.data(), size, &array));
    });
    samples[5].operations += size + 1;
    require(payloads.live == static_cast<std::size_t>(size));
    read(samples[6], false, false);
    read(samples[7], false, true);
    release(samples[8]);
  }
}

constexpr std::array text_names = {"raw-create", "concat", "concat-self", "scalar-slice",
    "append-text", "finish", "release", "reject-metadata"};
using TextSamples = std::array<Sample, text_names.size()>;

template <typename Action>
void measure_text(Sample &sample, Action action) {
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
  objc3_test_text_construction_reset();
#endif
  measure(sample, action);
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
  sample.decoded_bytes += objc3_test_text_decoded_input_bytes();
  sample.decoder_calls += objc3_test_text_decode_calls();
  sample.copied_bytes += objc3_test_text_copied_bytes();
#endif
}

std::uint64_t check_text(objc3_text_v2_t text, std::string_view expected, std::size_t scalars) {
  std::uint64_t byte_count = 0, scalar_count = 0;
  checked(objc3_runtime_stdlib_text_v2_byte_count_checked(text, &byte_count));
  checked(objc3_runtime_stdlib_text_v2_scalar_count_checked(text, &scalar_count));
  require(byte_count == expected.size() && scalar_count == scalars && !expected.empty());
  std::uint64_t checksum = byte_count + scalar_count;
  for (auto offset : {size_t{0}, expected.size() / 4, expected.size() / 2,
                      3 * expected.size() / 4, expected.size() - 1}) {
    std::uint8_t byte = 0;
    checked(objc3_runtime_stdlib_text_v2_byte_at_checked(text, offset, &byte));
    require(byte == static_cast<std::uint8_t>(expected[offset]));
    checksum += byte;
  }
  return checksum;
}

void run_text(TextSamples &samples, std::size_t size, int rounds, bool mixed) {
  const std::string_view pattern = mixed ? std::string_view("\0\xc2\xa2\xe2\x82\xac\xf0\x9f\x99\x82", 10)
                                         : std::string_view("\0ABC", 4);
  std::string input;
  input.reserve(size);
  const auto groups = size / pattern.size();
  for (std::size_t group = 0; group != groups; ++group) input += pattern;
  input.resize(size, 'x');
  const std::string_view bytes = input;
  const auto scalars = mixed ? groups * 4 + size % pattern.size() : size;
  const auto offset = [&](std::size_t scalar) {
    if (!mixed) return scalar;
    if (scalar >= groups * 4) return groups * 10 + scalar - groups * 4;
    constexpr std::size_t starts[] = {0, 1, 3, 6};
    return scalar / 4 * 10 + starts[scalar % 4];
  };
  const auto left_scalars = mixed ? size / 2 / 10 * 4 : scalars / 2;
  const auto split = offset(left_scalars);
  const auto start = scalars / 4, length = scalars / 2;
  const auto slice = bytes.substr(offset(start), offset(start + length) - offset(start));
  const auto first = bytes.substr(0, split), last = bytes.substr(split);
  std::string repeated(first);
  repeated += first;
  const auto release = [](objc3_text_v2_t text) {
    checked(objc3_runtime_stdlib_text_v2_release_checked(text));
  };
  objc3_text_v2_t left = nullptr, right = nullptr;
  checked(objc3_runtime_stdlib_text_v2_create_utf8_checked(first.data(), first.size(), &left));
  checked(objc3_runtime_stdlib_text_v2_create_utf8_checked(last.data(), last.size(), &right));
  for (int round = 0; round < rounds; ++round) {
    objc3_text_v2_t text = nullptr, concat = nullptr, self = nullptr, subtext = nullptr;
    objc3_text_v2_t snapshot1 = nullptr, snapshot2 = nullptr;
    objc3_text_builder_v2_t builder = nullptr;
    checked(objc3_runtime_stdlib_text_builder_v2_create(&builder));
    measure_text(samples[0], [&] {
      checked(objc3_runtime_stdlib_text_v2_create_utf8_checked(bytes.data(), size, &text));
    });
    ++samples[0].operations;
    samples[0].checksum += check_text(text, bytes, scalars);
    measure_text(samples[1], [&] {
      checked(objc3_runtime_stdlib_text_v2_concat(left, right, &concat));
    });
    ++samples[1].operations;
    samples[1].checksum += check_text(concat, bytes, scalars);
    measure_text(samples[2], [&] {
      checked(objc3_runtime_stdlib_text_v2_concat(left, left, &self));
    });
    ++samples[2].operations;
    samples[2].checksum += check_text(self, repeated, 2 * left_scalars);
    measure_text(samples[3], [&] {
      checked(objc3_runtime_stdlib_text_v2_slice_scalars(text, start, length, &subtext));
    });
    ++samples[3].operations;
    samples[3].checksum += check_text(subtext, slice, length);
    measure_text(samples[4], [&] {
      checked(objc3_runtime_stdlib_text_builder_v2_append_text(builder, text));
    });
    ++samples[4].operations;
    measure_text(samples[5], [&] {
      checked(objc3_runtime_stdlib_text_builder_v2_finish(builder, &snapshot1));
      checked(objc3_runtime_stdlib_text_builder_v2_finish(builder, &snapshot2));
    });
    samples[5].operations += 2;
    samples[5].checksum += check_text(snapshot1, bytes, scalars) + check_text(snapshot2, bytes, scalars);
    measure_text(samples[6], [&] {
      release(text);
      release(concat);
      release(self);
      release(subtext);
      release(snapshot1);
      release(snapshot2);
      checked(objc3_runtime_stdlib_text_builder_v2_release(builder));
    });
    samples[6].operations += 7;
    objc3_text_v2_t rejected = reinterpret_cast<objc3_text_v2_t>(1);
    measure_text(samples[7], [&] {
      require(objc3_runtime_stdlib_text_v2_create_utf8_record_checked(
          bytes.data(), size, scalars + 1, &rejected) == OBJC3_STDLIB_V2_INVALID_ARGUMENT);
    });
    require(!rejected);
    ++samples[7].operations;
  }
  release(left);
  release(right);
}

int argument(const char *text, long maximum) {
  char *end = nullptr;
  const long value = std::strtol(text, &end, 10);
  if (!*text || *end || value < 1 || value > maximum) std::exit(2);
  return static_cast<int>(value);
}
template <typename Names, std::size_t N>
void print_samples(const Names &names, const std::array<Sample, N> &samples,
                   std::string_view family) {
  std::putchar('{');
  for (std::size_t i = 0; i < samples.size(); ++i) {
    const auto &sample = samples[i];
    const std::string_view name = names[i];
    std::printf("%s\"%s\":{\"completed_operations\":%llu,\"checksum\":%llu,",
        i ? "," : "", name.data(),
        static_cast<unsigned long long>(sample.operations),
        static_cast<unsigned long long>(sample.checksum));
#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
    std::printf("\"allocations\":%llu,\"allocated_bytes\":%llu",
        static_cast<unsigned long long>(sample.allocations), static_cast<unsigned long long>(sample.allocated_bytes));
    if (family.starts_with("generic-") || family.starts_with("array-")) {
      std::printf(",\"key_copies\":%llu,\"value_copies\":%llu,\"destroys\":%llu,\"equals\":%llu,\"peak_payload_bytes\":%llu",
          static_cast<unsigned long long>(sample.key_copies), static_cast<unsigned long long>(sample.value_copies),
          static_cast<unsigned long long>(sample.destroys), static_cast<unsigned long long>(sample.equals),
          static_cast<unsigned long long>(sample.peak_payload_bytes));
    }
#ifdef OBJC3_BENCH_SERVICE_DIAGNOSTICS
    if ((family == "map" || family == "set") &&
        (name == "construct" || name == "remove" || name == "teardown")) {
      std::printf(",\"index_bytes_before\":%llu,\"retained_index_bytes\":%llu",
          static_cast<unsigned long long>(sample.index_bytes_before),
          static_cast<unsigned long long>(sample.retained_index_bytes));
    }
    if (family.starts_with("array-")) {
      std::printf(",\"metadata_slots\":%llu,\"metadata_nodes\":%llu",
          static_cast<unsigned long long>(sample.metadata_slots),
          static_cast<unsigned long long>(sample.metadata_nodes));
    }
    if (family.starts_with("text-")) {
      std::printf(",\"decoded_bytes\":%llu,\"decoder_calls\":%llu,\"copied_bytes\":%llu",
          static_cast<unsigned long long>(sample.decoded_bytes), static_cast<unsigned long long>(sample.decoder_calls),
          static_cast<unsigned long long>(sample.copied_bytes));
    }
#endif
    std::putchar('}');
#else
    (void)family;
    std::printf("\"nanoseconds\":%llu}", static_cast<unsigned long long>(sample.nanoseconds));
#endif
  }
  std::puts("}");
}

template <typename Value>
void run_managed(std::string_view family, int size, int rounds) {
  if (family.starts_with("array-")) {
    ArraySamples samples;
    run_array<Value>(samples, size, rounds);
    print_samples(array_names, samples, family);
    return;
  }
  AssociativeSamples samples;
  if (family.starts_with("generic-map-")) run_associative<Value, true>(samples, size, rounds);
  else if (family.starts_with("generic-set-")) run_associative<Value, false>(samples, size, rounds);
  else std::exit(2);
  print_samples(associative_names, samples, family);
}
}  // namespace

#ifdef OBJC3_BENCH_COLLECT_ALLOCATIONS
extern "C" void *__real__Znwm(std::size_t);
extern "C" void *__wrap__Znwm(std::size_t size) {
  record_allocation(size);
  return __real__Znwm(size);
}
extern "C" void *__real__ZnwmSt11align_val_tRKSt9nothrow_t(
    std::size_t, std::align_val_t, const std::nothrow_t &) noexcept;
extern "C" void *__wrap__ZnwmSt11align_val_tRKSt9nothrow_t(
    std::size_t size, std::align_val_t alignment, const std::nothrow_t &tag) noexcept {
  record_allocation(size);
  return __real__ZnwmSt11align_val_tRKSt9nothrow_t(size, alignment, tag);
}
#endif

int main(int argc, char **argv) {
  if (argc != 4) return 2;
  const std::string_view family = argv[1];
  const bool text = family == "text-ascii" || family == "text-mixed";
  const int size = argument(argv[2], text ? 1 << 20 : 2048);
  const int rounds = argument(argv[3], 128);
  if (family == "map" || family == "set") {
    std::array<Sample, phase_count> samples;
    for (int round = 0; round < rounds; ++round) {
      if (family == "map") run<true>(samples, size);
      else run<false>(samples, size);
    }
    print_samples(phases, samples, family);
  } else if (text && size >= 32) {
    TextSamples samples;
    run_text(samples, size, rounds, family == "text-mixed");
    print_samples(text_names, samples, family);
  } else if (family.ends_with("-int")) {
    run_managed<Payload<false>>(family, size, rounds);
  } else if (family.ends_with("-string")) {
    run_managed<Payload<true>>(family, size, rounds);
  } else {
    return 2;
  }
}
