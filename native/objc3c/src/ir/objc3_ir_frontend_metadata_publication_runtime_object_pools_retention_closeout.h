#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRRuntimePoolRetentionCloseoutMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out);
