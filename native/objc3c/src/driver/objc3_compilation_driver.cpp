#include "driver/objc3_compilation_driver.h"

#include <iostream>
#include <string>

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
    return RunObjc3LanguagePath(cli_options);
  }
  return RunObjectiveCPath(cli_options);
}
