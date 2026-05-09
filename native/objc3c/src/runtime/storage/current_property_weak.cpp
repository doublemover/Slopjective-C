#include "runtime/storage/current_property_context.h"

#include "runtime/memory/arc_debug_state.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int objc3_runtime_load_weak_current_property_i32(void) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.weak_current_property_load_count;
  const int result = objc3_runtime_read_current_property_i32();
  arc_debug.last_weak_loaded_value = result;
  return result;
}

extern "C" void objc3_runtime_store_weak_current_property_i32(int value) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.weak_current_property_store_count;
  arc_debug.last_weak_stored_value = value;
  objc3_runtime_write_current_property_i32(value);
}
