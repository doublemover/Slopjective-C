#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRRuntimeExportBoundaryNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
