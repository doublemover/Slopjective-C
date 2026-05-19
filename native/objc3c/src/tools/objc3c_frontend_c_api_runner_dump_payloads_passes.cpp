#include "tools/objc3c_frontend_c_api_runner_dump_payloads_internal.h"

void AppendFrontendCApiRunnerDumpPayloadPasses(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication) {
  AppendFrontendCApiRunnerObservabilityDumpPayload(payloads,
                                                   options,
                                                   publication);
  AppendFrontendCApiRunnerPlaygroundReproDumpPayload(payloads,
                                                     options,
                                                     publication);
  AppendFrontendCApiRunnerRuntimeInspectorDumpPayload(payloads,
                                                      options,
                                                      publication);
  AppendFrontendCApiRunnerStageTraceDumpPayload(payloads,
                                                options,
                                                publication);
}
