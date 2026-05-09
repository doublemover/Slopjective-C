#include "driver/objc3_driver_command_dispatch.h"

#include "driver/objc3_objectivec_path.h"
#include "driver/objc3_objc3_path.h"

int DispatchObjc3DriverCommand(const Objc3CliOptions &cli_options,
                               Objc3DriverInputKind input_kind) {
  if (input_kind == Objc3DriverInputKind::kObjc3Language) {
    // runtime ARC helper API surface anchor: driver orchestration
    // continues to toggle ARC lowering and runtime linkage without widening
    // the private ARC helper ABI into new CLI-facing surface.
    // runtime ARC helper implementation anchor: driver orchestration
    // now proves the private ARC helper ABI is both internal and linked as a
    // live runtime capability for the supported ARC execution slice.
    // ownership-debug/runtime-validation anchor: driver
    // orchestration still does not widen the CLI or public ABI surface; lane-D
    // adds only private runtime-debug probes and validation hooks above the
    // same native objc3 path.
    // block-runtime API/object-layout freeze anchor: the compilation
    // driver keeps block runtime mechanics behind the native objc3 path only;
    // no extra CLI surface or public runtime ABI routing is introduced here.
    // block-runtime allocation/copy-dispose/invoke anchor: driver
    // orchestration now also republishes the live runtime helper capability
    // without changing the public CLI surface or source acceptance model.
    // byref-forwarding/heap-promotion/ownership-interop anchor:
    // driver orchestration still widens only the internal runtime behavior for
    // escaping pointer-capture blocks; there is no new CLI or public ABI knob.
    return RunObjc3LanguagePath(cli_options);
  }
  return RunObjectiveCPath(cli_options);
}
