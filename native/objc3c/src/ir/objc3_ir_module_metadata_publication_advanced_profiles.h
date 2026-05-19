#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRModuleMetadataAdvancedProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata, std::ostringstream &out);
