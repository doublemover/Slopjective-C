#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface);

Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(
    const Objc3FrontendPipelineResult &pipeline_result);

bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface,
    std::string &reason);
