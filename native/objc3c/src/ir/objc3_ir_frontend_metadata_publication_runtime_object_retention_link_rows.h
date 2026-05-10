#pragma once

#include <iosfwd>
#include <string>

void EmitObjc3IRRuntimeObjectLinkerRetentionMetadataNode(
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::ostringstream &out);
