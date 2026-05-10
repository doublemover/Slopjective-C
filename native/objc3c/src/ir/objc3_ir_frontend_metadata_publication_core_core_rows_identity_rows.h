#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRFrontendInterfaceImplementationCoreCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
void EmitObjc3IRFrontendProtocolCategoryCoreCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
