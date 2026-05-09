#pragma once

#include <cstddef>
#include <iosfwd>

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

struct Objc3IRPropertyMetadataCommentEmissionOptions {
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3LoweringIRBoundary &lowering_ir_boundary;
  std::size_t synthesized_property_accessor_count = 0;
};

void EmitObjc3IRPropertyMetadataCommentEmission(
    const Objc3IRPropertyMetadataCommentEmissionOptions &options,
    std::ostringstream &out);
