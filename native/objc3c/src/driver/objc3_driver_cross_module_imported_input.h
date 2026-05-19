#pragma once

#include "io/objc3_process.h"
#include "pipeline/objc3_runtime_import_surface.h"

void AppendObjc3DriverCrossModuleRuntimeImportedInput(
    Objc3CrossModuleRuntimeLinkPlanArtifactInputs &link_plan_inputs,
    const Objc3ImportedRuntimeModuleSurface &imported_surface,
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &peer_artifacts);
