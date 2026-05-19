#pragma once

#include "io/objc3_process.h"
#include "pipeline/objc3_runtime_import_surface.h"

void PopulateObjc3DriverCrossModuleRuntimeImportedInputStorageReflection(
    Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    const Objc3ImportedRuntimeModuleSurface &imported_surface);
