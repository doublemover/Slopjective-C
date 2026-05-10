#pragma once

#include <ostream>

#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_context.h"

void WriteFrontendCApiRunnerRuntimeInspectorContractAvailabilitySection(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerRuntimeInspectorContext &context);

void WriteFrontendCApiRunnerRuntimeInspectorObjectAbiSections(
    std::ostream &out,
    const FrontendCApiRunnerRuntimeInspectorContext &context);

void WriteFrontendCApiRunnerRuntimeInspectorDumpTailSections(
    std::ostream &out,
    const FrontendCApiRunnerRuntimeInspectorContext &context);
