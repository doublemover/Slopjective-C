#include "runtime/stdlib/stdlib_service_contract.h"

#include <cassert>
#include <cstdint>
#include <vector>

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
}  // namespace

int main() {
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
