#include "driver/objc3_driver_main.h"

#include <string>

#include "driver/objc3_cli_parse.h"
#include "driver/objc3_driver_command_result.h"
#include "driver/objc3_compilation_driver.h"
#include "driver/objc3_driver_status_codes.h"
#include "driver/objc3_llvm_capability_routing.h"

int RunObjc3DriverMain(int argc, char **argv) {
  Objc3CliOptions cli_options;
  std::string cli_error;
  if (!ParseObjc3CliOptions(argc, argv, cli_options, cli_error)) {
    return EmitObjc3DriverCommandResult(Objc3DriverCommandFailed(
        Objc3DriverStatusCode::kInputUnavailable, cli_error));
  }
  if (!ApplyObjc3LLVMCapabilityRouting(cli_options, cli_error)) {
    return EmitObjc3DriverCommandResult(Objc3DriverCommandFailed(
        Objc3DriverStatusCode::kInputUnavailable, cli_error));
  }

  return RunObjc3CompilationDriver(cli_options);
}
