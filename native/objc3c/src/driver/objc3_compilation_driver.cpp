#include "driver/objc3_compilation_driver.h"

#include <iostream>
#include <string>

#include "diag/objc3_diag_utils.h"
#include "driver/objc3_driver_shell.h"
#include "driver/objc3_objectivec_path.h"
#include "driver/objc3_objc3_path.h"

int RunObjc3CompilationDriver(const Objc3CliOptions &cli_options) {
  const Objc3DriverInputKind input_kind = ClassifyObjc3DriverInput(cli_options.input);
  std::string shell_error;
  if (!ValidateObjc3DriverShellInputs(cli_options, input_kind, shell_error)) {
    std::cerr << shell_error << "\n";
    return 2;
  }

  if (input_kind == Objc3DriverInputKind::kObjc3Language) {
    // M262-D001 runtime ARC helper API surface anchor: driver orchestration
    // continues to toggle ARC lowering and runtime linkage without widening
    // the private ARC helper ABI into new CLI-facing surface.
    // M261-D001 block-runtime API/object-layout freeze anchor: the compilation
    // driver keeps block runtime mechanics behind the native objc3 path only;
    // no extra CLI surface or public runtime ABI routing is introduced here.
    // M261-D002 block-runtime allocation/copy-dispose/invoke anchor: driver
    // orchestration now also republishes the live runtime helper capability
    // without changing the public CLI surface or source acceptance model.
    // M261-D003 byref-forwarding/heap-promotion/ownership-interop anchor:
    // driver orchestration still widens only the internal runtime behavior for
    // escaping pointer-capture blocks; there is no new CLI or public ABI knob.
    return RunObjc3LanguagePath(cli_options);
  }
  return RunObjectiveCPath(cli_options);
}
