#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void BeginObjc3IRRuntimeMetadataSectionPublicationNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
void EndObjc3IRRuntimeMetadataSectionPublicationNode(std::ostringstream &out);
