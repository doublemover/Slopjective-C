#pragma once

#include <string>
#include <vector>

#include "tools/objc3c_frontend_c_api_runner_dump_publication.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

void SeedFrontendCApiRunnerSummaryDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication);

void AppendFrontendCApiRunnerDumpPayloadPasses(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication);

void AppendFrontendCApiRunnerObservabilityDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication);

void AppendFrontendCApiRunnerPlaygroundReproDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication);

void AppendFrontendCApiRunnerRuntimeInspectorDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication);

void AppendFrontendCApiRunnerStageTraceDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication);
