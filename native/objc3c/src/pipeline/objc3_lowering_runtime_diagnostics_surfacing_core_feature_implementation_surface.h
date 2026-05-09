#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface);

Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface(
    const Objc3FrontendPipelineResult &pipeline_result);

bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface,
    std::string &reason);
