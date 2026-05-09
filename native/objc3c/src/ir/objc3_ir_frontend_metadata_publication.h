#pragma once

#include <iosfwd>
#include <string>

#include "ir/objc3_ir_frontend_metadata.h"

std::string BuildObjc3IRFrontendProfileComment(
    const Objc3IRFrontendMetadata &metadata);

std::string BuildObjc3IRFrontendNamedMetadataTable();

std::string BuildObjc3IRFrontendMetadataNode(
    const Objc3IRFrontendMetadata &metadata);

void EmitObjc3IRFrontendCoreMetadataPublication(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRRuntimeMetadataBoundaryNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
