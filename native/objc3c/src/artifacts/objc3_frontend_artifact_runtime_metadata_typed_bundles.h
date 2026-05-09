#pragma once

#include "ir/objc3_ir_frontend_metadata.h"
#include "runtime/metadata/class_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendRuntimeMetadataClassMetaclassBundles(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication);

[[nodiscard]] bool ApplyObjc3FrontendRuntimeMetadataProtocolCategoryBundles(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication);

}  // namespace objc3::artifacts::frontend
