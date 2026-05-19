#pragma once

#include "pipeline/objc3_frontend_types.h"
#include "sema/objc3_sema_pass_manager_contract.h"

bool ShouldRunObjc3FrontendSemaStage(
    const Objc3FrontendPipelineResult &result);

Objc3SemaPassManagerResult RunObjc3FrontendSemaStage(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options,
    bool allow_error_handling_error_runtime_surface);
