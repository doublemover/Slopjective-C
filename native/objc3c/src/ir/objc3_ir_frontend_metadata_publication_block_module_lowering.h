#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRBlockLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRTypeModuleLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRModuleGovernanceLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
