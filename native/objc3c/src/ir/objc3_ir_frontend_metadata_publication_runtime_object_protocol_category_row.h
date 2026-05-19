#pragma once

#include <iosfwd>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category_category_counts.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category_protocol_counts.h"

struct Objc3IRFrontendMetadata;

void EmitObjc3IRRuntimeProtocolCategoryMetadataRow(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3IRRuntimeProtocolCategoryProtocolCounts &protocol_counts,
    const Objc3IRRuntimeProtocolCategoryCategoryCounts &category_counts,
    std::ostringstream &out);
