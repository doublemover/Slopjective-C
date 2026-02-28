#include "driver/objc3_driver_main.h"

#include <iostream>
#include <string>

#include "driver/objc3_cli_options.h"
#include "driver/objc3_compilation_driver.h"
#include "driver/objc3_llvm_capability_routing.h"

int RunObjc3DriverMain(int argc, char **argv) {
  Objc3CliOptions cli_options;
  std::string cli_error;
  if (!ParseObjc3CliOptions(argc, argv, cli_options, cli_error)) {
    std::cerr << cli_error << "\n";
    return 2;
  }
  if (!ApplyObjc3LLVMCabilityRouting(cli_options, cli_error)) {
    std::cerr << cli_error << "\n";
    return 2;
  }

  return RunObjc3CompilationDriver(cli_options);
}
