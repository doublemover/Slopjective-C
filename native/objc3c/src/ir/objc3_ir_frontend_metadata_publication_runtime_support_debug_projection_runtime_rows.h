#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void BeginObjc3IRExecutableDebugProjectionMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
void EndObjc3IRExecutableDebugProjectionMetadataNode(std::ostringstream &out);
