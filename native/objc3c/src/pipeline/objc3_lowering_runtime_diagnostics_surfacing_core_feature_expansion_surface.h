#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

// The C004 surface type is declared in objc3_frontend_types.h; this header
// owns deterministic key/derivation/readiness declarations for that type.
std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
        &surface);

Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface(
    const Objc3FrontendPipelineResult &pipeline_result);

bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
        &surface,
    std::string &reason);
