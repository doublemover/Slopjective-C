#include "driver/objc3_compilation_driver.h"

#include <string>

#include "diag/objc3_diag_utils.h"
#include "driver/objc3_driver_command_dispatch.h"
#include "driver/objc3_driver_command_result.h"
#include "driver/objc3_driver_shell.h"
#include "driver/objc3_driver_status_codes.h"
#include "driver/objc3_objc3_path.h"

int RunObjc3CompilationDriver(const Objc3CliOptions &cli_options) {
  if (cli_options.command_mode == Objc3CliCommandMode::kValidateConformance) {
    return RunObjc3ConformanceValidationPath(cli_options);
  }

  const Objc3DriverInputKind input_kind = ClassifyObjc3DriverInput(cli_options.input);
  std::string shell_error;
  if (!ValidateObjc3DriverShellInputs(cli_options, input_kind, shell_error)) {
    return EmitObjc3DriverCommandResult(Objc3DriverCommandFailed(
        Objc3DriverStatusCode::kInputUnavailable, shell_error));
  }

  return DispatchObjc3DriverCommand(cli_options, input_kind);
}
