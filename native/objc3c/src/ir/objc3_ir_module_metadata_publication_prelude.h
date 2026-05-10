#pragma once

#include <cstddef>
#include <iosfwd>

struct Objc3IRFrontendMetadata;
struct Objc3LoweringIRBoundary;

struct Objc3IRModuleMetadataPreludePublicationOptions {
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3LoweringIRBoundary &lowering_ir_boundary;
  std::size_t synthesized_property_accessor_count = 0;
};

void EmitObjc3IRModuleMetadataPreludePublication(
    const Objc3IRModuleMetadataPreludePublicationOptions &options,
    std::ostringstream &out);
