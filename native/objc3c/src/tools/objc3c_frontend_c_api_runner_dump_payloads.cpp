#include "tools/objc3c_frontend_c_api_runner_dump_payloads.h"

#include "tools/objc3c_frontend_c_api_runner_dump_payloads_internal.h"

std::vector<std::string> BuildFrontendCApiRunnerDumpPayloads(
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication) {
  std::vector<std::string> payloads;
  SeedFrontendCApiRunnerSummaryDumpPayload(payloads, options, publication);
  AppendFrontendCApiRunnerDumpPayloadPasses(payloads, options, publication);
  return payloads;
}
