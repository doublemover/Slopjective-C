#include "artifacts/objc3_frontend_artifact_runtime_metadata_typed_bundles.h"

#include <cstddef>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "ir/objc3_ir_frontend_metadata.h"

namespace objc3::artifacts::frontend {
namespace {

inline constexpr const char *kArtifactExecutableIvarLayoutEmissionContractId =
    "objc3c.executable.ivar.layout.emission.v1";
inline constexpr const char *kArtifactExecutableIvarLayoutDescriptorModel =
    "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering";
inline constexpr const char *kArtifactExecutableIvarOffsetGlobalModel =
    "one-retained-i64-offset-global-per-emitted-ivar-binding";
inline constexpr const char *kArtifactExecutableIvarLayoutTableModel =
    "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size";

}  // namespace

void ApplyObjc3FrontendRuntimeMetadataTypedLoweringBundles(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  if (!IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          executable_metadata_typed_lowering_handoff)) {
    return;
  }

  const auto &source_graph =
      executable_metadata_typed_lowering_handoff.source_graph;
  ApplyObjc3FrontendRuntimeMetadataClassMetaclassBundles(
      ir_frontend_metadata, source_graph, runtime_metadata_section_publication);

  // Keep protocol/category success as the gate for the member-table projection:
  // member payload records depend on the same owner identities.
  const bool protocol_category_payload_complete =
      ApplyObjc3FrontendRuntimeMetadataProtocolCategoryBundles(
          ir_frontend_metadata, source_graph,
          runtime_metadata_section_publication);
  ApplyObjc3FrontendRuntimeMetadataMemberTableBundles(
      ir_frontend_metadata, source_graph, runtime_metadata_section_publication,
      protocol_category_payload_complete);
}

void ApplyObjc3FrontendRuntimeMetadataMemberTableBundles(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    bool protocol_category_payload_complete) {
  bool member_table_payload_complete = protocol_category_payload_complete;

  std::vector<Objc3IRRuntimeMetadataPropertyBundle> property_bundles;
  if (member_table_payload_complete) {
    member_table_payload_complete =
        BuildObjc3FrontendRuntimeMetadataPropertyBundles(source_graph,
                                                         property_bundles);
  }

  std::vector<Objc3IRRuntimeMetadataIvarBundle> ivar_bundles;
  if (member_table_payload_complete) {
    member_table_payload_complete =
        BuildObjc3FrontendRuntimeMetadataIvarBundles(source_graph,
                                                     ivar_bundles);
  }

  std::vector<Objc3IRRuntimeMetadataMethodListBundle> method_list_bundles;
  if (member_table_payload_complete) {
    member_table_payload_complete =
        BuildObjc3FrontendRuntimeMetadataMethodListBundles(
            ir_frontend_metadata, source_graph, property_bundles,
            method_list_bundles);
  }

  member_table_payload_complete =
      member_table_payload_complete &&
      property_bundles.size() ==
          runtime_metadata_section_publication.property_descriptor_count &&
      ivar_bundles.size() ==
          runtime_metadata_section_publication.ivar_descriptor_count;
  if (member_table_payload_complete) {
    std::size_t property_attribute_profiles = 0;
    std::size_t accessor_ownership_profiles = 0;
    std::size_t synthesized_binding_entries = 0;
    for (const auto &bundle : property_bundles) {
      if (!bundle.property_attribute_profile.empty()) {
        ++property_attribute_profiles;
      }
      if (!bundle.accessor_ownership_profile.empty()) {
        ++accessor_ownership_profiles;
      }
      if (!bundle.executable_synthesized_binding_kind.empty()) {
        ++synthesized_binding_entries;
      }
    }
    ir_frontend_metadata.runtime_metadata_method_list_bundles_lexicographic =
        std::move(method_list_bundles);
    ir_frontend_metadata.executable_property_attribute_profile_entries =
        property_attribute_profiles;
    ir_frontend_metadata.executable_accessor_ownership_profile_entries =
        accessor_ownership_profiles;
    ir_frontend_metadata.executable_synthesized_binding_entries =
        synthesized_binding_entries;
    ir_frontend_metadata.executable_ivar_layout_entries = ivar_bundles.size();
    ir_frontend_metadata.executable_property_ivar_source_model_replay_key =
        "property_attribute_profiles=" +
        std::to_string(property_attribute_profiles) +
        ";accessor_ownership_profiles=" +
        std::to_string(accessor_ownership_profiles) +
        ";synthesized_bindings=" + std::to_string(synthesized_binding_entries) +
        ";ivar_layout_entries=" + std::to_string(ivar_bundles.size()) +
        ";deterministic=true;lane_contract=objc3c.property.ivar.source.model.v1";
    ir_frontend_metadata.executable_ivar_layout_emission_contract_id =
        kArtifactExecutableIvarLayoutEmissionContractId;
    ir_frontend_metadata.executable_ivar_layout_descriptor_model =
        kArtifactExecutableIvarLayoutDescriptorModel;
    ir_frontend_metadata.executable_ivar_offset_global_model =
        kArtifactExecutableIvarOffsetGlobalModel;
    ir_frontend_metadata.executable_ivar_layout_table_model =
        kArtifactExecutableIvarLayoutTableModel;
    std::set<std::string> ivar_layout_owner_identities;
    bool ivar_layout_emission_complete = true;
    std::size_t ivar_offset_global_entries = 0;
    for (const auto &bundle : ivar_bundles) {
      if (bundle.declaration_owner_identity.empty() ||
          bundle.executable_ivar_layout_symbol.empty() ||
          bundle.executable_ivar_layout_alignment_bytes == 0u ||
          bundle.executable_ivar_layout_size_bytes == 0u ||
          !bundle.executable_ivar_layout_valid ||
          bundle.executable_ivar_layout_replay_key.empty() ||
          bundle.ivar_binding_symbol.empty()) {
        ivar_layout_emission_complete = false;
        break;
      }
      ivar_layout_owner_identities.insert(bundle.declaration_owner_identity);
      ++ivar_offset_global_entries;
    }
    ir_frontend_metadata.executable_ivar_offset_global_entries =
        ivar_offset_global_entries;
    ir_frontend_metadata.executable_ivar_layout_table_entries =
        ivar_layout_owner_identities.size();
    ir_frontend_metadata.executable_ivar_layout_owner_entries =
        ivar_layout_owner_identities.size();
    ir_frontend_metadata.executable_ivar_layout_emission_ready =
        ivar_layout_emission_complete;
    ir_frontend_metadata.executable_ivar_layout_emission_fail_closed =
        ivar_layout_emission_complete;
    if (ivar_layout_emission_complete) {
      ir_frontend_metadata.executable_ivar_layout_emission_replay_key =
          "offset_globals=" + std::to_string(ivar_offset_global_entries) +
          ";layout_tables=" +
          std::to_string(ivar_layout_owner_identities.size()) +
          ";owner_entries=" +
          std::to_string(ivar_layout_owner_identities.size()) +
          ";deterministic=true;lane_contract=objc3c.ivar.layout.emission.v1";
    } else {
      ir_frontend_metadata.executable_ivar_layout_emission_replay_key.clear();
    }
    ir_frontend_metadata.runtime_metadata_property_bundles_lexicographic =
        std::move(property_bundles);
    ir_frontend_metadata.runtime_metadata_ivar_bundles_lexicographic =
        std::move(ivar_bundles);
  }
  ir_frontend_metadata.runtime_metadata_member_table_emission_ready =
      member_table_payload_complete;
  ir_frontend_metadata.runtime_metadata_member_table_emission_fail_closed =
      member_table_payload_complete;
}

}  // namespace objc3::artifacts::frontend
