#pragma once

#include <string>

#include "io/objc3_process.h"

bool TryValidateObjc3CrossModuleRuntimeLinkPlanArtifactInputs(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    std::string &error);
