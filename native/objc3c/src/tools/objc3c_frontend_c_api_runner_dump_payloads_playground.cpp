#include "tools/objc3c_frontend_c_api_runner_dump_payloads_internal.h"

#include "tools/objc3c_frontend_c_api_runner_playground_repro_json.h"

void AppendFrontendCApiRunnerPlaygroundReproDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication) {
  if (options.dump_playground_repro_json) {
    payloads.push_back(BuildFrontendCApiRunnerPlaygroundReproJson(
        options,
        FrontendCApiRunnerDumpCompileResult(publication),
        publication.summary_path));
  }
}
