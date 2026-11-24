#include <objc3c/runtime/objc3_collections.h>
#include <objc3c/runtime/objc3_text.h>

#include <cstdio>

static bool check_map() {
  objc3_map_i32_i32_v2_t map = nullptr;
  if (objc3_runtime_stdlib_map_i32_i32_v2_create(&map) != OBJC3_STDLIB_V2_OK)
    return false;
  uint64_t count = 0;
  int32_t value = 0;
  int found = 0;
  const bool valid =
      objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, 7, 12, &count) == OBJC3_STDLIB_V2_OK && count == 1 &&
      objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, 7, 19, &count) == OBJC3_STDLIB_V2_OK && count == 1 &&
      objc3_runtime_stdlib_map_i32_i32_v2_get(map, 7, &found, &value) == OBJC3_STDLIB_V2_OK && found == 1 && value == 19;
  return objc3_runtime_stdlib_map_i32_i32_v2_release(map) == OBJC3_STDLIB_V2_OK && valid;
}

static bool check_text() {
  const char sample[] = {'A', static_cast<char>(0xc2), static_cast<char>(0xa2)};
  objc3_text_v2_t text = nullptr;
  if (objc3_runtime_stdlib_text_v2_create_utf8_checked(sample, sizeof(sample), &text) != OBJC3_STDLIB_V2_OK)
    return false;
  objc3_text_v2_t slice = nullptr;
  objc3_text_v2_t rejected = nullptr;
  uint64_t scalars = 0, bytes = 0;
  uint8_t first = 0, second = 0;
  const bool valid =
      objc3_runtime_stdlib_text_v2_scalar_count_checked(text, &scalars) == OBJC3_STDLIB_V2_OK && scalars == 2 &&
      objc3_runtime_stdlib_text_v2_slice_scalars(text, 1, 1, &slice) == OBJC3_STDLIB_V2_OK &&
      objc3_runtime_stdlib_text_v2_byte_count_checked(slice, &bytes) == OBJC3_STDLIB_V2_OK && bytes == 2 &&
      objc3_runtime_stdlib_text_v2_byte_at_checked(slice, 0, &first) == OBJC3_STDLIB_V2_OK && first == 0xc2 &&
      objc3_runtime_stdlib_text_v2_byte_at_checked(slice, 1, &second) == OBJC3_STDLIB_V2_OK && second == 0xa2 &&
      objc3_runtime_stdlib_text_v2_slice_scalars(text, 3, 1, &rejected) == OBJC3_STDLIB_V2_OUT_OF_BOUNDS && !rejected;
  const bool released_slice = slice && objc3_runtime_stdlib_text_v2_release_checked(slice) == OBJC3_STDLIB_V2_OK;
  const bool released_text = objc3_runtime_stdlib_text_v2_release_checked(text) == OBJC3_STDLIB_V2_OK;
  if (rejected) objc3_runtime_stdlib_text_v2_release_checked(rejected);
  return valid && released_slice && released_text;
}

int main() {
  if (!check_map()) {
    std::fputs("i32 map replacement contract failed\n", stderr);
    return 1;
  }
  if (!check_text()) {
    std::fputs("UTF-8 scalar slice contract failed\n", stderr);
    return 1;
  }
  std::puts("owned Objective-C 3 Darwin collections and text: PASS");
  return 0;
}
