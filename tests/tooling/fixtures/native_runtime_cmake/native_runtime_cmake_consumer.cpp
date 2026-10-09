#include "runtime/platform/objc3_native_object_adapter.h"

int main() {
  objc3_native_weak_slot slot{};
  if (objc3_native_object_retain(nullptr) != nullptr) return 1;
  objc3_native_weak_init(&slot, nullptr);
  if (objc3_native_weak_load_retained(&slot) != nullptr) return 2;
  if (objc3_native_weak_store(&slot, nullptr) != nullptr) return 3;
  objc3_native_weak_destroy(&slot);
  if (objc3_native_block_copy(nullptr) != nullptr) return 4;
  objc3_native_block_release(nullptr);
  objc3_native_object_release(nullptr);
  return 0;
}
