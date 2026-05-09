#pragma once

#include <iosfwd>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRInteropLoweringMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRMetaprogrammingLoweringMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRActorDispatchControlMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRDispatchMetadataPreservationNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IROwnershipExtensionMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRLoweringExtensionCommentPublication(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
