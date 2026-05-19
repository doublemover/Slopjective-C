#pragma once

#include "runtime/metadata/class_metadata.h"
#include "runtime/metadata/runtime_metadata_typed_bundles.h"

#include <vector>

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendRuntimeMetadataClassMetaclassBundles(
    Objc3IRFrontendRuntimeSourceClosureMetadata &runtime_source_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication);

[[nodiscard]] bool ApplyObjc3FrontendRuntimeMetadataProtocolCategoryBundles(
    Objc3IRFrontendRuntimeSourceClosureMetadata &runtime_source_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication);

void ApplyObjc3FrontendRuntimeMetadataMemberTableBundles(
    Objc3IRFrontendRuntimeMemberStorageMetadata &runtime_member_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
        &derived_method_bundles,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    bool protocol_category_payload_complete);

void ApplyObjc3FrontendRuntimeMetadataTypedLoweringBundles(
    Objc3IRFrontendRuntimeSourceClosureMetadata &runtime_source_metadata,
    Objc3IRFrontendRuntimeMemberStorageMetadata &runtime_member_metadata,
    const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
        &derived_method_bundles,
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication);

[[nodiscard]] bool BuildObjc3FrontendRuntimeMetadataPropertyBundles(
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    std::vector<Objc3IRRuntimeMetadataPropertyBundle> &property_bundles);

[[nodiscard]] bool BuildObjc3FrontendRuntimeMetadataIvarBundles(
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    std::vector<Objc3IRRuntimeMetadataIvarBundle> &ivar_bundles);

[[nodiscard]] bool BuildObjc3FrontendRuntimeMetadataMethodListBundles(
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
        &derived_method_bundles,
    const std::vector<Objc3IRRuntimeMetadataPropertyBundle> &property_bundles,
    std::vector<Objc3IRRuntimeMetadataMethodListBundle>
        &method_list_bundles);

}  // namespace objc3::artifacts::frontend
