#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRErrorHandlingLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRSafetyConcurrencyLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRAsyncDiagnosticLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
