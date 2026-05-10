#pragma once

#include "tools/objc3c_frontend_c_api_runner_dump_publication.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

bool ShouldEmitFrontendCApiRunnerDumpActions(
    const FrontendCApiRunnerOptions &options);

void EmitFrontendCApiRunnerDumpActions(
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication);
