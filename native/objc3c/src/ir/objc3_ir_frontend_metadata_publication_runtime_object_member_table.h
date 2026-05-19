#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRRuntimeMemberTableMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
