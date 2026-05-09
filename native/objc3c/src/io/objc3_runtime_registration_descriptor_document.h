#pragma once

#include <string>

#include "io/objc3_process.h"

std::string BuildObjc3RuntimeRegistrationDescriptorArtifactDocumentJson(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    const std::string &constructor_init_stub_symbol,
    const std::string &bootstrap_registration_table_symbol,
    const std::string &bootstrap_image_local_init_state_symbol);

