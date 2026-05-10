#pragma once

#include "ir/objc3_ir_frontend_metadata.h"
#include "runtime/metadata/class_metadata.h"

#include <vector>

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

void ApplyObjc3FrontendRuntimeMetadataMemberTableBundles(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    bool protocol_category_payload_complete);

[[nodiscard]] bool BuildObjc3FrontendRuntimeMetadataPropertyBundles(
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    std::vector<Objc3IRRuntimeMetadataPropertyBundle> &property_bundles);

[[nodiscard]] bool BuildObjc3FrontendRuntimeMetadataIvarBundles(
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    std::vector<Objc3IRRuntimeMetadataIvarBundle> &ivar_bundles);

[[nodiscard]] bool BuildObjc3FrontendRuntimeMetadataMethodListBundles(
    const Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const std::vector<Objc3IRRuntimeMetadataPropertyBundle> &property_bundles,
    std::vector<Objc3IRRuntimeMetadataMethodListBundle>
        &method_list_bundles);

}  // namespace objc3::artifacts::frontend
