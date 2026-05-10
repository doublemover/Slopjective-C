#pragma once

#include <iosfwd>

#include "io/objc3_process.h"

void EmitObjc3RuntimeRegistrationManifestBootstrapJson(
    std::ostream &out,
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record);
