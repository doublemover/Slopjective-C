#pragma once

#include "io/objc3_process.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

void PopulateObjc3DriverCrossModuleRuntimeLinkPlanExpectedContracts(
    Objc3CrossModuleRuntimeLinkPlanArtifactInputs &link_plan_inputs,
    const Objc3FrontendArtifactBundle &artifacts);
