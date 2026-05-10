#include "tools/objc3c_frontend_c_api_runner_dump_payloads_internal.h"

#include "tools/objc3c_frontend_c_api_runner_playground_repro_json.h"

void AppendFrontendCApiRunnerPlaygroundReproDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result) {
  if (options.dump_playground_repro_json) {
    payloads.push_back(BuildFrontendCApiRunnerPlaygroundReproJson(
        options,
        result,
        summary_path));
  }
}
