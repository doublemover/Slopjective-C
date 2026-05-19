#pragma once

#include <string>

struct Objc3IRFrontendMetadata;

std::string BuildObjc3IRFrontendNamedMetadataTable();

std::string BuildObjc3IRFrontendMetadataNode(
    const Objc3IRFrontendMetadata &metadata);
