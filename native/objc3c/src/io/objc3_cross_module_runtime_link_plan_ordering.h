#pragma once

#include <string>
#include <vector>

#include "io/objc3_process.h"

std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
BuildOrderedObjc3CrossModuleRuntimeLinkPlanImportedInputs(
    const std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
        &imported_inputs);

std::vector<std::string>
BuildOrderedObjc3CrossModuleDirectImportSurfaceArtifactPaths(
    const std::vector<std::string> &direct_import_surface_artifact_paths);

std::string BuildObjc3CrossModuleRuntimeLinkerResponsePayload(
    const std::vector<std::string> &merged_driver_linker_flags);
