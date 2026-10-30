#include "runtime/stdlib/stdlib_service_contract.h"

#include <cassert>
#include <cstdint>
#include <vector>
#include <atomic>
#include <new>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#ifdef OBJC3_TEXT_CONSTRUCTION_TESTING
extern "C" void objc3_test_text_construction_reset();
extern "C" uint64_t objc3_test_text_copied_bytes();
extern "C" uint64_t objc3_test_text_decode_calls();
extern "C" uint64_t objc3_test_text_decoded_input_bytes();
static std::atomic<long> fail_text_allocation{-1};
static std::atomic<std::size_t> pause_text_allocation{0};
static std::size_t observed_allocation_size = 0;
static unsigned observed_allocations = 0;
static std::mutex text_gate_mutex;
static std::condition_variable text_gate_changed;
static bool text_gate_entered = false, text_gate_continue = false;
extern "C" void *__real__Znwm(std::size_t);
extern "C" void *__wrap__Znwm(std::size_t size) {
  if (observed_allocation_size && size == observed_allocation_size) ++observed_allocations;
  if (fail_text_allocation.load() >= 0 && fail_text_allocation.fetch_sub(1) == 0)
    throw std::bad_alloc();
  if (size && size == pause_text_allocation.load()) {
    std::unique_lock lock(text_gate_mutex);
    pause_text_allocation = 0;
    text_gate_entered = true;
    text_gate_changed.notify_all();
    text_gate_changed.wait(lock, [] { return text_gate_continue; });
  }
  return __real__Znwm(size);
}
#endif

namespace {
std::vector<std::uint8_t> Bytes(objc3_text_v2_t text) {
  std::uint64_t count = 0;
  assert(objc3_runtime_stdlib_text_v2_byte_count_checked(text, &count) ==
         OBJC3_STDLIB_V2_OK);
  std::vector<std::uint8_t> result(static_cast<std::size_t>(count));
  for (std::uint64_t index = 0; index < count; ++index)
    assert(objc3_runtime_stdlib_text_v2_byte_at_checked(
               text, index, &result[static_cast<std::size_t>(index)]) ==
           OBJC3_STDLIB_V2_OK);
  return result;
}
#ifdef OBJC3_TEXT_CONSTRUCTION_TESTING
void Observe(std::size_t payload_size) {
  observed_allocation_size = payload_size;
  observed_allocations = 0;
  objc3_test_text_construction_reset();
}
void TrustedCopy(std::size_t bytes) {
  assert(objc3_test_text_decode_calls() == 0);
  assert(objc3_test_text_copied_bytes() == bytes);
  assert(!bytes || observed_allocations == 1);
}
void ConstructionTransactions() {
  const std::vector<char> sample = {'A', char(0xc2), char(0xa2), char(0xe2), char(0x82), char(0xac),
      char(0xf0), char(0x9f), char(0x99), char(0x82), 0};
  const unsigned offsets[] = {0, 1, 3, 6, 10, 11};
  objc3_text_v2_t source = nullptr, empty = nullptr;
  Observe(sample.size());
  assert(objc3_runtime_stdlib_text_v2_create_utf8_checked(sample.data(), sample.size(), &source) == 0);
  assert(objc3_test_text_decode_calls() == 1 && objc3_test_text_decoded_input_bytes() == sample.size());
  assert(objc3_test_text_copied_bytes() == sample.size() && observed_allocations == 1);
  assert(objc3_runtime_stdlib_text_v2_create_utf8_checked(nullptr, 0, &empty) == 0);
  for (bool equal : {false, true}) {
    objc3_text_v2_t joined = nullptr;
    const auto size = sample.size() * (equal ? 2 : 1);
    Observe(size);
    assert(objc3_runtime_stdlib_text_v2_concat(source, equal ? source : empty, &joined) == 0);
    TrustedCopy(size);
    auto expected = std::vector<std::uint8_t>(sample.begin(), sample.end());
    if (equal) expected.insert(expected.end(), sample.begin(), sample.end());
    assert(Bytes(joined) == expected);
    assert(objc3_runtime_stdlib_text_v2_release_checked(joined) == 0);
  }
  for (unsigned start = 0; start <= 5; ++start) for (unsigned count = 0; count <= 5 - start; ++count) {
    objc3_text_v2_t slice = nullptr;
    const auto length = offsets[start + count] - offsets[start];
    Observe(length);
    assert(objc3_runtime_stdlib_text_v2_slice_scalars(source, start, count, &slice) == 0);
    TrustedCopy(length);
    assert(Bytes(slice) == std::vector<std::uint8_t>(sample.begin() + offsets[start], sample.begin() + offsets[start + count]));
    uint64_t actual = 0;
    assert(objc3_runtime_stdlib_text_v2_scalar_count_checked(slice, &actual) == 0 && actual == count);
    assert(objc3_runtime_stdlib_text_v2_release_checked(slice) == 0);
  }
  objc3_text_builder_v2_t builder = nullptr;
  assert(objc3_runtime_stdlib_text_builder_v2_create(&builder) == 0);
  Observe(0);
  assert(objc3_runtime_stdlib_text_builder_v2_append_text(builder, source) == 0);
  assert(objc3_test_text_decode_calls() == 0 && objc3_test_text_copied_bytes() == sample.size());
  std::vector<objc3_text_v2_t> snapshots;
  for (unsigned i = 0; i != 3; ++i) {
    objc3_text_v2_t snapshot = nullptr;
    Observe(sample.size());
    assert(objc3_runtime_stdlib_text_builder_v2_finish(builder, &snapshot) == 0);
    TrustedCopy(sample.size());
    snapshots.push_back(snapshot);
  }
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 'Z') == 0);
  assert(objc3_runtime_stdlib_text_builder_v2_reset(builder) == 0);
  assert(objc3_runtime_stdlib_text_builder_v2_release(builder) == 0);
  for (auto *snapshot : snapshots) {
    assert(Bytes(snapshot) == std::vector<std::uint8_t>(sample.begin(), sample.end()));
    assert(objc3_runtime_stdlib_text_v2_release_checked(snapshot) == 0);
  }
  // Offset collection may run out of memory before a malformed final byte.
  // Invalid UTF-8 retains precedence and cannot publish a partial value.
  auto invalid = sample; invalid.push_back(char(0xff));
  bool saw_success = false, saw_failure = false;
  for (long fail_at = 0; fail_at < 32; ++fail_at) {
    auto *out = reinterpret_cast<objc3_text_v2_t>(1);
    fail_text_allocation = fail_at;
    const auto rejected = objc3_runtime_stdlib_text_v2_create_utf8_checked(invalid.data(), invalid.size(), &out);
    fail_text_allocation = -1;
    assert(rejected == OBJC3_STDLIB_V2_INVALID_UTF8 && !out);
    fail_text_allocation = fail_at;
    const auto status = objc3_runtime_stdlib_text_v2_create_utf8_checked(sample.data(), sample.size(), &out);
    fail_text_allocation = -1;
    if (status == 0) { saw_success = true; assert(objc3_runtime_stdlib_text_v2_release_checked(out) == 0); }
    else { saw_failure = true; assert(status == OBJC3_STDLIB_V2_OUT_OF_MEMORY && !out); }
    assert(Bytes(source) == std::vector<std::uint8_t>(sample.begin(), sample.end()));
  }
  assert(saw_success && saw_failure);
  // Fail byte, boundary and publication allocations for every trusted route.
  for (unsigned operation = 0; operation != 4; ++operation) {
    bool passed = false, failed = false;
    for (long fail_at = 0; fail_at != 16; ++fail_at) {
      objc3_text_builder_v2_t scratch = nullptr;
      assert(objc3_runtime_stdlib_text_builder_v2_create(&scratch) == 0);
      if (operation == 2) assert(objc3_runtime_stdlib_text_builder_v2_append_text(scratch, source) == 0);
      objc3_text_v2_t out = nullptr;
      fail_text_allocation = fail_at;
      const int status = operation == 0 ? objc3_runtime_stdlib_text_v2_concat(source, source, &out) :
          operation == 1 ? objc3_runtime_stdlib_text_v2_slice_scalars(source, 1, 3, &out) :
          operation == 2 ? objc3_runtime_stdlib_text_builder_v2_finish(scratch, &out) :
                           objc3_runtime_stdlib_text_builder_v2_append_text(scratch, source);
      fail_text_allocation = -1;
      if (status == 0) { passed = true; if (out) assert(objc3_runtime_stdlib_text_v2_release_checked(out) == 0); }
      else { failed = true; assert(status == OBJC3_STDLIB_V2_OUT_OF_MEMORY && !out); }
      objc3_text_v2_t snapshot = nullptr;
      assert(objc3_runtime_stdlib_text_builder_v2_finish(scratch, &snapshot) == 0);
      assert(Bytes(snapshot).size() == (operation == 2 || (operation == 3 && status == 0) ? sample.size() : 0));
      assert(objc3_runtime_stdlib_text_v2_release_checked(snapshot) == 0);
      assert(objc3_runtime_stdlib_text_builder_v2_release(scratch) == 0);
    }
    assert(passed && failed);
  }
  assert(objc3_runtime_stdlib_text_v2_release_checked(source) == 0);
  assert(objc3_runtime_stdlib_text_v2_release_checked(empty) == 0);
  observed_allocation_size = 0;
}
void ConcurrentSourceRelease() {
  const std::vector<char> bytes(4096, 'x');
  objc3_text_v2_t input = nullptr, result = nullptr;
  assert(objc3_runtime_stdlib_text_v2_create_utf8_checked(bytes.data(), bytes.size(), &input) == 0);
  pause_text_allocation = bytes.size() * 2;
  std::thread worker([&] { assert(objc3_runtime_stdlib_text_v2_concat(input, input, &result) == 0); });
  {
    std::unique_lock lock(text_gate_mutex);
    assert(text_gate_changed.wait_for(lock, std::chrono::seconds(5), [] { return text_gate_entered; }));
    assert(objc3_runtime_stdlib_text_v2_release_checked(input) == 0);
    text_gate_continue = true;
  }
  text_gate_changed.notify_all();
  worker.join();
  assert(Bytes(result) == std::vector<std::uint8_t>(bytes.size() * 2, 'x'));
  assert(objc3_runtime_stdlib_text_v2_release_checked(result) == 0);
}
#endif
}  // namespace

int main() {
#ifdef OBJC3_TEXT_CONSTRUCTION_TESTING
  ConstructionTransactions();
  ConcurrentSourceRelease();
#endif
  const char sample[] = {'A', static_cast<char>(0xf0), static_cast<char>(0x9f),
                         static_cast<char>(0x99), static_cast<char>(0x82),
                         '\0', 'B'};
  objc3_text_v2_t text = nullptr;
  assert(objc3_runtime_stdlib_text_v2_create_utf8_checked(
             sample, sizeof(sample), &text) == OBJC3_STDLIB_V2_OK);

  std::uint64_t count = 0;
  assert(objc3_runtime_stdlib_text_v2_scalar_count_checked(text, &count) ==
         OBJC3_STDLIB_V2_OK);
  assert(count == 4);  // A, U+1F642, embedded NUL, B.

  const char face[] = {static_cast<char>(0xf0), static_cast<char>(0x9f),
                       static_cast<char>(0x99), static_cast<char>(0x82)};
  int found = 0;
  std::uint64_t scalar_index = UINT64_MAX;
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, face, sizeof(face), 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_OK);
  assert(found == 1 && scalar_index == 1);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, face, sizeof(face), 2, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_OK);
  assert(found == 0 && scalar_index == 0);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, face, sizeof(face), count, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_OK);
  assert(found == 0 && scalar_index == 0);

  const char nul[] = {'\0'};
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, nul, sizeof(nul), 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_OK);
  assert(found == 1 && scalar_index == 2);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, nullptr, 0, count, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_OK);
  assert(found == 1 && scalar_index == count);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, nullptr, 0, count + 1, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_OUT_OF_BOUNDS);
  assert(found == 0 && scalar_index == 0);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, nullptr, 0, UINT64_MAX, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_OUT_OF_BOUNDS);
  assert(found == 0 && scalar_index == 0);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, face, UINT64_MAX, 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_CAPACITY_EXCEEDED);
  assert(found == 0 && scalar_index == 0);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, nullptr, 1, 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_INVALID_ARGUMENT);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, face, sizeof(face), 0, nullptr, &scalar_index) ==
         OBJC3_STDLIB_V2_INVALID_ARGUMENT);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, face, sizeof(face), 0, &found, nullptr) ==
         OBJC3_STDLIB_V2_INVALID_ARGUMENT);

  const char malformed[] = {static_cast<char>(0x80)};
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, malformed, sizeof(malformed), 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_INVALID_UTF8);
  assert(found == 0 && scalar_index == 0);
  // Neither a leading continuation byte nor a truncated scalar can be a
  // scalar-search needle, even when those bytes occur in the haystack.
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, face + 1, sizeof(face) - 1, 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_INVALID_UTF8);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, face, sizeof(face) - 1, 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_INVALID_UTF8);

  objc3_text_v2_t slice = nullptr;
  assert(objc3_runtime_stdlib_text_v2_slice_scalars(text, 1, 1, &slice) ==
         OBJC3_STDLIB_V2_OK);
  assert((Bytes(slice) == std::vector<std::uint8_t>{0xf0, 0x9f, 0x99, 0x82}));
  objc3_text_v2_t empty = nullptr;
  assert(objc3_runtime_stdlib_text_v2_slice_scalars(text, count, 0, &empty) ==
         OBJC3_STDLIB_V2_OK);
  assert(Bytes(empty).empty());
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             empty, nullptr, 0, 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_OK);
  assert(found == 1 && scalar_index == 0);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             empty, face, sizeof(face), 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_OK);
  assert(found == 0 && scalar_index == 0);
  objc3_text_v2_t invalid_slice = text;
  assert(objc3_runtime_stdlib_text_v2_slice_scalars(
             text, 1, UINT64_MAX, &invalid_slice) ==
         OBJC3_STDLIB_V2_OUT_OF_BOUNDS);
  assert(invalid_slice == nullptr);
  invalid_slice = text;
  assert(objc3_runtime_stdlib_text_v2_slice_scalars(
             text, UINT64_MAX, 0, &invalid_slice) ==
         OBJC3_STDLIB_V2_OUT_OF_BOUNDS);
  assert(invalid_slice == nullptr);

  objc3_text_builder_v2_t builder = nullptr;
  assert(objc3_runtime_stdlib_text_builder_v2_create(&builder) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 0x1f642) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 0xd800) ==
         OBJC3_STDLIB_V2_INVALID_ARGUMENT);
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 0xdfff) ==
         OBJC3_STDLIB_V2_INVALID_ARGUMENT);
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 0x110000) ==
         OBJC3_STDLIB_V2_INVALID_ARGUMENT);
  objc3_text_v2_t built = nullptr;
  assert(objc3_runtime_stdlib_text_builder_v2_finish(builder, &built) ==
         OBJC3_STDLIB_V2_OK);
  assert((Bytes(built) == std::vector<std::uint8_t>{0xf0, 0x9f, 0x99, 0x82}));
  // Finish snapshots the contents rather than consuming/resetting the builder.
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 'B') ==
         OBJC3_STDLIB_V2_OK);
  objc3_text_v2_t continued = nullptr;
  assert(objc3_runtime_stdlib_text_builder_v2_finish(builder, &continued) ==
         OBJC3_STDLIB_V2_OK);
  assert((Bytes(continued) ==
          std::vector<std::uint8_t>{0xf0, 0x9f, 0x99, 0x82, 'B'}));
  assert((Bytes(built) == std::vector<std::uint8_t>{0xf0, 0x9f, 0x99, 0x82}));
  assert(objc3_runtime_stdlib_text_builder_v2_reset(builder) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 0) ==
         OBJC3_STDLIB_V2_OK);
  objc3_text_v2_t reused = nullptr;
  assert(objc3_runtime_stdlib_text_builder_v2_finish(builder, &reused) ==
         OBJC3_STDLIB_V2_OK);
  assert((Bytes(reused) == std::vector<std::uint8_t>{0}));

  assert(objc3_runtime_stdlib_text_builder_v2_reset(builder) ==
         OBJC3_STDLIB_V2_OK);
  const std::uint32_t scalar_edges[] = {
      0x7f, 0x80, 0x7ff, 0x800, 0xd7ff, 0xe000, 0xffff, 0x10000, 0x10ffff};
  for (const auto scalar : scalar_edges)
    assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, scalar) ==
           OBJC3_STDLIB_V2_OK);
  objc3_text_v2_t edges = nullptr;
  assert(objc3_runtime_stdlib_text_builder_v2_finish(builder, &edges) ==
         OBJC3_STDLIB_V2_OK);
  assert((Bytes(edges) == std::vector<std::uint8_t>{
      0x7f, 0xc2, 0x80, 0xdf, 0xbf, 0xe0, 0xa0, 0x80,
      0xed, 0x9f, 0xbf, 0xee, 0x80, 0x80, 0xef, 0xbf, 0xbf,
      0xf0, 0x90, 0x80, 0x80, 0xf4, 0x8f, 0xbf, 0xbf}));
  std::uint64_t edge_count = 0;
  assert(objc3_runtime_stdlib_text_v2_scalar_count_checked(edges, &edge_count) ==
         OBJC3_STDLIB_V2_OK);
  assert(edge_count == sizeof(scalar_edges) / sizeof(scalar_edges[0]));
  const char last_scalar[] = {static_cast<char>(0xf4), static_cast<char>(0x8f),
                            static_cast<char>(0xbf), static_cast<char>(0xbf)};
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             edges, last_scalar, sizeof(last_scalar), 1, &found,
             &scalar_index) == OBJC3_STDLIB_V2_OK);
  assert(found == 1 && scalar_index == edge_count - 1);

  assert(objc3_runtime_stdlib_text_builder_v2_reset(builder) ==
         OBJC3_STDLIB_V2_OK);
  const std::vector<char> capacity_bytes((1U << 20) - 1, 'a');
  assert(objc3_runtime_stdlib_text_builder_v2_append_utf8(
             builder, capacity_bytes.data(), capacity_bytes.size()) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 0x80) ==
         OBJC3_STDLIB_V2_CAPACITY_EXCEEDED);
  // A rejected two-byte scalar must leave enough room for a one-byte scalar.
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 'b') ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 0) ==
         OBJC3_STDLIB_V2_CAPACITY_EXCEEDED);
  objc3_text_v2_t capacity_text = nullptr;
  assert(objc3_runtime_stdlib_text_builder_v2_finish(builder, &capacity_text) ==
         OBJC3_STDLIB_V2_OK);
  std::uint64_t capacity_count = 0;
  assert(objc3_runtime_stdlib_text_v2_byte_count_checked(
             capacity_text, &capacity_count) == OBJC3_STDLIB_V2_OK);
  assert(capacity_count == 1U << 20);
  std::uint8_t last_byte = 0;
  assert(objc3_runtime_stdlib_text_v2_byte_at_checked(
             capacity_text, capacity_count - 1, &last_byte) ==
         OBJC3_STDLIB_V2_OK);
  assert(last_byte == 'b');

  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(nullptr, 'a') ==
         OBJC3_STDLIB_V2_INVALID_HANDLE);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             nullptr, nullptr, 0, 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_INVALID_HANDLE);
  assert(found == 0 && scalar_index == 0);

  assert(objc3_runtime_stdlib_text_v2_release_checked(reused) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_release_checked(built) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_release_checked(continued) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_release_checked(edges) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_release_checked(capacity_text) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_builder_v2_release(builder) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_builder_v2_append_scalar(builder, 'a') ==
         OBJC3_STDLIB_V2_STALE_HANDLE);
  assert(objc3_runtime_stdlib_text_v2_release_checked(empty) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_release_checked(slice) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_release_checked(text) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_find_scalars(
             text, nullptr, 0, 0, &found, &scalar_index) ==
         OBJC3_STDLIB_V2_STALE_HANDLE);
  assert(found == 0 && scalar_index == 0);
}
