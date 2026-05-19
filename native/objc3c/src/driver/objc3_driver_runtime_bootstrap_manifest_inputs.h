#pragma once

#include "io/objc3_process.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

void PopulateObjc3DriverRuntimeBootstrapSemanticsInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts);
void PopulateObjc3DriverRuntimeBootstrapApiInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts);
void PopulateObjc3DriverRuntimeBootstrapRegistrarInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs);
void PopulateObjc3DriverRuntimeBootstrapResetInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs);
void PopulateObjc3DriverRuntimeBootstrapLoweringInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts);
