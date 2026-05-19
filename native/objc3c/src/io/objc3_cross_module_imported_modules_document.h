#pragma once

#include <string>
#include <vector>

#include "io/objc3_process.h"

std::string BuildObjc3CrossModuleImportedModulesJson(
    const std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
        &imported_inputs);
