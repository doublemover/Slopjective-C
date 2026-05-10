#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRExecutableDebugProjectionSurfaceFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
void EmitObjc3IRExecutableDebugProjectionReplayFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
