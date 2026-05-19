#pragma once

#include <string>

#include "io/objc3_process.h"

bool BuildObjc3CrossModuleRuntimeLinkPlanDocument(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    std::string &plan_json,
    std::string &linker_response_payload,
    std::string &error);
