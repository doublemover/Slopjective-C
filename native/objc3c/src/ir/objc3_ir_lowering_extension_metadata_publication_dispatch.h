#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRActorDispatchControlMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRDispatchMetadataPreservationNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
