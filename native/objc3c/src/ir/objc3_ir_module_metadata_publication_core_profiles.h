#pragma once

#include <cstddef>
#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRModuleMetadataCoreProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out);
