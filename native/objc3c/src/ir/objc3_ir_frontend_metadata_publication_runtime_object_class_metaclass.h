#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRRuntimeClassMetaclassMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
