#pragma once

#include <ostream>

#include "io/objc3_process.h"

void AppendObjc3RuntimeRegistrationManifestAccessorAbiSurfacesJson(
    std::ostream &out,
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs);
