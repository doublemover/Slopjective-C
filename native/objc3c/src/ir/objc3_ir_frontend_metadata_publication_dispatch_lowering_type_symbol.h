#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRTypeSymbolDispatchLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
