#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void BeginObjc3IRRuntimeSupportLibraryBaseMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
void EndObjc3IRRuntimeSupportLibraryBaseMetadataNode(std::ostringstream &out);
