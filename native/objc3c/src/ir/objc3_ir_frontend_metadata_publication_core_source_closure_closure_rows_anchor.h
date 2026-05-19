#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void BeginObjc3IRFrontendProtocolCategorySourceClosureAnchor(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
void EndObjc3IRFrontendProtocolCategorySourceClosureAnchor(
    std::ostringstream &out);
