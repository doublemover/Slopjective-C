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
    // M261-D001 block-runtime API/object-layout freeze anchor: the compilation
    // driver keeps block runtime mechanics behind the native objc3 path only;
    // no extra CLI surface or public runtime ABI routing is introduced here.
    return RunObjc3LanguagePath(cli_options);
  }
  return RunObjectiveCPath(cli_options);
}
