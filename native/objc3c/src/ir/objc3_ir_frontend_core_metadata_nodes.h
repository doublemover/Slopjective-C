#pragma once

#include <string>

#include "ir/objc3_ir_frontend_metadata.h"

std::string BuildObjc3IRFrontendNamedMetadataTable();

std::string BuildObjc3IRFrontendMetadataNode(
    const Objc3IRFrontendMetadata &metadata);
