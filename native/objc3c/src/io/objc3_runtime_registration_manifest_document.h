#pragma once

#include <cstddef>
#include <string>

#include "io/objc3_process.h"

std::string BuildObjc3RuntimeTranslationUnitRegistrationManifestDocumentJson(
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record,
    std::size_t runtime_metadata_binary_byte_count);
