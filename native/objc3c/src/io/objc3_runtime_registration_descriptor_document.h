#pragma once

#include <string>

#include "io/objc3_process.h"

std::string BuildObjc3RuntimeRegistrationDescriptorArtifactDocumentJson(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record);
