#include "tools/objc3c_frontend_c_api_runner_repro_command.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_repro_command_segments.h"

std::string BuildFrontendCApiRunnerReproCommand(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    bool dump_playground_repro_json) {
  std::ostringstream command;
  AppendFrontendCApiRunnerReproBaseInvocationAndPathArgs(command, options);
  AppendFrontendCApiRunnerReproToolchainArgs(command, options);
  AppendFrontendCApiRunnerReproSummaryPathArg(command, summary_path);
  AppendFrontendCApiRunnerReproBackendArg(command, options);
  AppendFrontendCApiRunnerReproRuntimeArgs(command, options);
  AppendFrontendCApiRunnerReproEmissionDumpFlags(
      command,
      options,
      dump_playground_repro_json);
  return command.str();
}
