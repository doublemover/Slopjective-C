#include "tools/objc3c_frontend_c_api_runner_dump_payloads_internal.h"

void SeedFrontendCApiRunnerSummaryDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication) {
  if (options.dump_summary_json) {
    payloads.push_back(publication.summary_json);
  }
}
