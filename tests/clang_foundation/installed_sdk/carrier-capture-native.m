#include "carrier-capture-call.h"
// Always compiled as Objective-C C17, including the C++ configurations.
void capture_native_throw(int key) { @throw capture_native_object(key); }
int capture_native_catch(int key) {
  @try { capture_unwind_bridge(key); }
  @catch (id object) { return object != (id)0; }
  return 0;
}
