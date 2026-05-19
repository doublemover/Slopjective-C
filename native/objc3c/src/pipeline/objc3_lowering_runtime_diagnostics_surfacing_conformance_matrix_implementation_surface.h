#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface);

Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface(
    const Objc3FrontendPipelineResult &pipeline_result);

bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface,
    std::string &reason);
