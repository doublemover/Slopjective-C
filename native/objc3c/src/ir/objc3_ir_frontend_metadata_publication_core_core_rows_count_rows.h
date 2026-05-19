#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRFrontendSelectorNormalizationCoreCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
void EmitObjc3IRFrontendPropertyAttributeCoreCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
