#include "arc_debug_instrumentation_probe/main_orchestration.h"

int main() {
  return objc3c::runtime::probe::arc_debug_instrumentation::
      RunArcDebugInstrumentationProbe();
}
