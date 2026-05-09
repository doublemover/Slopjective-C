#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
        &surface);

Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface(
    const Objc3FrontendPipelineResult &pipeline_result);

bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
        &surface,
    std::string &reason);
