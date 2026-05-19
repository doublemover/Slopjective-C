#include "runtime/storage/current_property_context.h"

#include "runtime/memory/arc_debug_events.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int objc3_runtime_load_weak_current_property_i32(void) {
  objc3c::runtime::RecordRuntimeArcWeakCurrentPropertyLoadCall();
  const int result = objc3_runtime_read_current_property_i32();
  objc3c::runtime::RecordRuntimeArcLastWeakCurrentPropertyLoadedValue(result);
  return result;
}

extern "C" void objc3_runtime_store_weak_current_property_i32(int value) {
  objc3c::runtime::RecordRuntimeArcWeakCurrentPropertyStoreCall(value);
  objc3_runtime_write_current_property_i32(value);
}
