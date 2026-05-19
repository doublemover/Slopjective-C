#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

#include "lower/objc3_lowering_contract.h"

struct Objc3IRFrontendMetadata;

struct Objc3IRModuleMetadataPublicationOptions {
  const std::string &module_name;
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3LoweringIRBoundary &lowering_ir_boundary;
  std::size_t synthesized_property_accessor_count = 0;
  std::size_t vector_signature_function_count = 0;
};

void EmitObjc3IRModuleMetadataPublication(
    const Objc3IRModuleMetadataPublicationOptions &options,
    std::ostringstream &out);
