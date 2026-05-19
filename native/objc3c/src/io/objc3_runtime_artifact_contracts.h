#pragma once

#include <filesystem>
#include <string>

#include "io/objc3_process.h"

namespace objc3c::io {

Objc3RuntimeRegistrationSymbolOwnerRecord
BuildRuntimeTranslationUnitRegistrationSymbolOwnerRecord(
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts);

Objc3RuntimeRegistrationSymbolOwnerRecord
BuildRuntimeRegistrationDescriptorSymbolOwnerRecord(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts);

bool ValidateRuntimeTranslationUnitRegistrationManifestArtifactInputs(
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    std::string &error);

bool ValidateRuntimeRegistrationDescriptorArtifactInputs(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    std::string &error);

bool ValidateMetaprogrammingMacroHostProcessCacheArtifactInputs(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    std::string &error);

}  // namespace objc3c::io
