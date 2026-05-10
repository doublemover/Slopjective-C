#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering.h"
#include "ir/objc3_ir_frontend_metadata_publication_concurrency.h"
#include "ir/objc3_ir_frontend_metadata_publication_core.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support.h"

struct Objc3IRFrontendMetadata;
struct Objc3RuntimeMetadataLayoutPolicy;
struct Objc3IRRuntimeMetadataSymbols;

std::string BuildObjc3IRFrontendProfileComment(
    const Objc3IRFrontendMetadata &metadata);

void EmitObjc3IRFrontendMetadataPublication(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out);
