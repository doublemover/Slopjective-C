#pragma once

#include "io/objc3_process.h"
#include "pipeline/objc3_runtime_import_surface.h"

Objc3CrossModuleRuntimeLinkPlanImportedInput
BuildObjc3DriverCrossModuleRuntimeImportedInputCore(
    const Objc3ImportedRuntimeModuleSurface &imported_surface,
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &peer_artifacts);
