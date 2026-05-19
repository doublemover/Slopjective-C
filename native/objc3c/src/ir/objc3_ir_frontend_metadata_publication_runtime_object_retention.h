#pragma once

#include <iosfwd>
#include <string>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRRuntimeObjectRetentionMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::ostringstream &out);
