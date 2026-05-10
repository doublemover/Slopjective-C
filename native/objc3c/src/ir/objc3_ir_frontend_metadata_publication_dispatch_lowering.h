#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRTypeSymbolDispatchCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRDispatchOwnershipLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
