#include "artifacts/objc3_frontend_artifact_runtime_metadata_contract_metadata.h"

#include <cstddef>

#include "artifacts/identity/artifact_identity.h"
#include "lower/contracts/runtime_bootstrap_link_discovery_contracts.h"
#include "lower/contracts/runtime_metadata_source_record_contracts.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendRuntimeMetadataContractMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement,
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const std::filesystem::path &input_path,
    const std::string &parse_artifact_replay_key,
    const std::string &lowering_boundary_replay_key) {
  ir_frontend_metadata.runtime_metadata_source_ownership_contract_id =
      runtime_metadata_source_ownership.contract_id;
  ir_frontend_metadata.runtime_metadata_source_schema =
      runtime_metadata_source_ownership.canonical_source_schema;
  ir_frontend_metadata.runtime_metadata_ivar_source_model =
      runtime_metadata_source_ownership.ivar_record_source_model;
  ir_frontend_metadata.runtime_metadata_class_record_count =
      runtime_metadata_source_ownership.class_record_count;
  ir_frontend_metadata.runtime_metadata_protocol_record_count =
      runtime_metadata_source_ownership.protocol_record_count;
  ir_frontend_metadata.runtime_metadata_category_interface_record_count =
      runtime_metadata_source_ownership.category_interface_record_count;
  ir_frontend_metadata.runtime_metadata_category_implementation_record_count =
      runtime_metadata_source_ownership.category_implementation_record_count;
  ir_frontend_metadata.runtime_metadata_property_record_count =
      runtime_metadata_source_ownership.property_record_count;
  ir_frontend_metadata.runtime_metadata_method_record_count =
      runtime_metadata_source_ownership.method_record_count;
  ir_frontend_metadata.runtime_metadata_ivar_record_count =
      runtime_metadata_source_ownership.ivar_record_count;
  ir_frontend_metadata.frontend_owns_runtime_metadata_source_records =
      runtime_metadata_source_ownership
          .frontend_owns_runtime_metadata_source_records;
  ir_frontend_metadata.runtime_metadata_source_records_ready_for_lowering =
      runtime_metadata_source_ownership
          .runtime_metadata_source_records_ready_for_lowering;
  ir_frontend_metadata.native_runtime_library_present =
      runtime_metadata_source_ownership.native_runtime_library_present;
  ir_frontend_metadata.runtime_metadata_source_boundary_fail_closed =
      runtime_metadata_source_ownership.fail_closed;
  ir_frontend_metadata.runtime_link_test_only =
      runtime_metadata_source_ownership.runtime_link_test_only;
  ir_frontend_metadata.deterministic_runtime_metadata_source_schema =
      runtime_metadata_source_ownership.deterministic_source_schema;

  ir_frontend_metadata.runtime_export_legality_contract_id =
      runtime_export_legality.contract_id;
  ir_frontend_metadata.runtime_export_semantic_boundary_frozen =
      runtime_export_legality.semantic_boundary_frozen;
  ir_frontend_metadata.runtime_export_metadata_export_enforcement_ready =
      runtime_export_legality.metadata_export_enforcement_ready;
  ir_frontend_metadata.runtime_export_fail_closed =
      runtime_export_legality.fail_closed;
  ir_frontend_metadata
      .runtime_export_duplicate_runtime_identity_enforcement_pending =
      runtime_export_legality.duplicate_runtime_identity_enforcement_pending;
  ir_frontend_metadata
      .runtime_export_incomplete_declaration_export_blocking_pending =
      runtime_export_legality.incomplete_declaration_export_blocking_pending;
  ir_frontend_metadata
      .runtime_export_illegal_redeclaration_mix_export_blocking_pending =
      runtime_export_legality
          .illegal_redeclaration_mix_export_blocking_pending;
  ir_frontend_metadata.runtime_export_class_record_count =
      runtime_export_legality.class_record_count;
  ir_frontend_metadata.runtime_export_protocol_record_count =
      runtime_export_legality.protocol_record_count;
  ir_frontend_metadata.runtime_export_category_record_count =
      runtime_export_legality.category_record_count;
  ir_frontend_metadata.runtime_export_property_record_count =
      runtime_export_legality.property_record_count;
  ir_frontend_metadata.runtime_export_method_record_count =
      runtime_export_legality.method_record_count;
  ir_frontend_metadata.runtime_export_ivar_record_count =
      runtime_export_legality.ivar_record_count;
  ir_frontend_metadata.runtime_export_invalid_protocol_composition_sites =
      runtime_export_legality.invalid_protocol_composition_sites;
  ir_frontend_metadata.runtime_export_property_attribute_invalid_entries =
      runtime_export_legality.property_attribute_invalid_entries;
  ir_frontend_metadata.runtime_export_property_attribute_contract_violations =
      runtime_export_legality.property_attribute_contract_violations;
  ir_frontend_metadata.runtime_export_invalid_type_annotation_sites =
      runtime_export_legality.invalid_type_annotation_sites;
  ir_frontend_metadata.runtime_export_property_ivar_binding_missing =
      runtime_export_legality.property_ivar_binding_missing;
  ir_frontend_metadata.runtime_export_property_ivar_binding_conflicts =
      runtime_export_legality.property_ivar_binding_conflicts;
  ir_frontend_metadata.runtime_export_implementation_resolution_misses =
      runtime_export_legality.implementation_resolution_misses;
  ir_frontend_metadata.runtime_export_method_resolution_misses =
      runtime_export_legality.method_resolution_misses;
  ir_frontend_metadata.runtime_export_boundary_ready =
      IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality);

  ir_frontend_metadata.runtime_export_enforcement_contract_id =
      runtime_export_enforcement.contract_id;
  ir_frontend_metadata.runtime_export_metadata_completeness_enforced =
      runtime_export_enforcement.metadata_completeness_enforced;
  ir_frontend_metadata
      .runtime_export_duplicate_runtime_identity_suppression_enforced =
      runtime_export_enforcement
          .duplicate_runtime_identity_suppression_enforced;
  ir_frontend_metadata
      .runtime_export_illegal_redeclaration_mix_blocking_enforced =
      runtime_export_enforcement
          .illegal_redeclaration_mix_blocking_enforced;
  ir_frontend_metadata.runtime_export_metadata_shape_drift_blocking_enforced =
      runtime_export_enforcement.metadata_shape_drift_blocking_enforced;
  ir_frontend_metadata.runtime_export_enforcement_fail_closed =
      runtime_export_enforcement.fail_closed;
  ir_frontend_metadata.runtime_export_ready_for_runtime_export =
      runtime_export_enforcement.ready_for_runtime_export;
  ir_frontend_metadata.runtime_export_duplicate_runtime_identity_sites =
      runtime_export_enforcement.duplicate_runtime_identity_sites;
  ir_frontend_metadata.runtime_export_incomplete_declaration_sites =
      runtime_export_enforcement.incomplete_declaration_sites;
  ir_frontend_metadata.runtime_export_illegal_redeclaration_mix_sites =
      runtime_export_enforcement.illegal_redeclaration_mix_sites;
  ir_frontend_metadata.runtime_export_metadata_shape_drift_sites =
      runtime_export_enforcement.metadata_shape_drift_sites;

  ir_frontend_metadata.runtime_metadata_section_abi_contract_id =
      runtime_metadata_section_abi.contract_id;
  ir_frontend_metadata.runtime_metadata_section_boundary_frozen =
      runtime_metadata_section_abi.boundary_frozen;
  ir_frontend_metadata.runtime_metadata_section_fail_closed =
      runtime_metadata_section_abi.fail_closed;
  ir_frontend_metadata.runtime_metadata_section_object_file_inventory_frozen =
      runtime_metadata_section_abi.object_file_section_inventory_frozen;
  ir_frontend_metadata.runtime_metadata_section_symbol_policy_frozen =
      runtime_metadata_section_abi.symbol_policy_frozen;
  ir_frontend_metadata.runtime_metadata_section_visibility_model_frozen =
      runtime_metadata_section_abi.visibility_model_frozen;
  ir_frontend_metadata.runtime_metadata_section_retention_policy_frozen =
      runtime_metadata_section_abi.retention_policy_frozen;
  ir_frontend_metadata.runtime_metadata_section_ready_for_scaffold =
      runtime_metadata_section_abi.ready_for_section_scaffold;
  ir_frontend_metadata.runtime_metadata_section_logical_image_info_section =
      runtime_metadata_section_abi.logical_image_info_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_class_descriptor_section =
      runtime_metadata_section_abi.logical_class_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_protocol_descriptor_section =
      runtime_metadata_section_abi.logical_protocol_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_category_descriptor_section =
      runtime_metadata_section_abi.logical_category_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_property_descriptor_section =
      runtime_metadata_section_abi.logical_property_descriptor_section;
  ir_frontend_metadata.runtime_metadata_section_logical_ivar_descriptor_section =
      runtime_metadata_section_abi.logical_ivar_descriptor_section;
  ir_frontend_metadata.runtime_metadata_section_descriptor_symbol_prefix =
      runtime_metadata_section_abi.descriptor_symbol_prefix;
  ir_frontend_metadata.runtime_metadata_section_aggregate_symbol_prefix =
      runtime_metadata_section_abi.aggregate_symbol_prefix;
  ir_frontend_metadata.runtime_metadata_section_image_info_symbol =
      runtime_metadata_section_abi.image_info_symbol;
  ir_frontend_metadata.runtime_metadata_section_descriptor_linkage =
      runtime_metadata_section_abi.descriptor_linkage;
  ir_frontend_metadata.runtime_metadata_section_aggregate_linkage =
      runtime_metadata_section_abi.aggregate_linkage;
  ir_frontend_metadata.runtime_metadata_section_visibility =
      runtime_metadata_section_abi.metadata_visibility;
  ir_frontend_metadata.runtime_metadata_section_retention_root =
      runtime_metadata_section_abi.retention_root;

  ir_frontend_metadata.runtime_metadata_section_publication_contract_id =
      runtime_metadata_section_publication.contract_id;
  ir_frontend_metadata.runtime_metadata_section_publication_abi_contract_id =
      runtime_metadata_section_publication.abi_contract_id;
  ir_frontend_metadata.runtime_metadata_section_publication_emitted =
      runtime_metadata_section_publication.publication_emitted;
  ir_frontend_metadata.runtime_metadata_section_publication_fail_closed =
      runtime_metadata_section_publication.fail_closed;
  ir_frontend_metadata.runtime_metadata_section_publication_uses_llvm_used =
      runtime_metadata_section_publication.uses_llvm_used;
  ir_frontend_metadata.runtime_metadata_section_publication_image_info_emitted =
      runtime_metadata_section_publication.image_info_emitted;
  ir_frontend_metadata
      .runtime_metadata_section_publication_class_descriptor_count =
      runtime_metadata_section_publication.class_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_protocol_descriptor_count =
      runtime_metadata_section_publication.protocol_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_category_descriptor_count =
      runtime_metadata_section_publication.category_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_property_descriptor_count =
      runtime_metadata_section_publication.property_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_ivar_descriptor_count =
      runtime_metadata_section_publication.ivar_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_total_descriptor_count =
      runtime_metadata_section_publication.total_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_total_retained_global_count =
      runtime_metadata_section_publication.total_retained_global_count;
  ir_frontend_metadata.runtime_metadata_section_publication_image_info_symbol =
      runtime_metadata_section_publication.image_info_symbol;
  ir_frontend_metadata.runtime_metadata_section_publication_class_aggregate_symbol =
      runtime_metadata_section_publication.class_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_protocol_aggregate_symbol =
      runtime_metadata_section_publication.protocol_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_category_aggregate_symbol =
      runtime_metadata_section_publication.category_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_property_aggregate_symbol =
      runtime_metadata_section_publication.property_aggregate_symbol;
  ir_frontend_metadata.runtime_metadata_section_publication_ivar_aggregate_symbol =
      runtime_metadata_section_publication.ivar_aggregate_symbol;

  const auto &source_graph =
      executable_metadata_typed_lowering_handoff.source_graph;
  ir_frontend_metadata.runtime_metadata_class_metaclass_emission_contract_id =
      kObjc3RuntimeClassMetaclassEmissionContractId;
  ir_frontend_metadata.runtime_metadata_class_metaclass_payload_model =
      kObjc3RuntimeClassMetaclassEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_name_model =
      kObjc3RuntimeClassMetaclassEmissionNameModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_super_link_model =
      kObjc3RuntimeClassMetaclassEmissionSuperLinkModel;
  ir_frontend_metadata
      .runtime_metadata_class_metaclass_method_list_reference_model =
      kObjc3RuntimeClassMetaclassEmissionMethodListReferenceModel;
  ir_frontend_metadata.executable_class_metaclass_source_closure_contract_id =
      source_graph.class_metaclass_source_closure_contract_id;
  ir_frontend_metadata.executable_class_metaclass_parent_identity_model =
      source_graph.class_metaclass_parent_identity_model;
  ir_frontend_metadata
      .executable_class_metaclass_method_owner_identity_model =
      source_graph.class_metaclass_method_owner_identity_model;
  ir_frontend_metadata.executable_class_metaclass_object_identity_model =
      source_graph.class_metaclass_object_identity_model;
  ir_frontend_metadata.executable_class_metaclass_source_closure_ready =
      source_graph.class_metaclass_declaration_closure_complete &&
      source_graph.class_metaclass_parent_identity_closure_complete &&
      source_graph.class_metaclass_method_owner_identity_closure_complete &&
      source_graph.class_metaclass_object_identity_closure_complete;
  ir_frontend_metadata.executable_class_metaclass_declaration_node_count =
      source_graph.interface_nodes_lexicographic.size() +
      source_graph.implementation_nodes_lexicographic.size() +
      source_graph.class_nodes_lexicographic.size() +
      source_graph.metaclass_nodes_lexicographic.size();
  ir_frontend_metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;

  ir_frontend_metadata.executable_protocol_category_source_closure_contract_id =
      source_graph.protocol_category_source_closure_contract_id;
  ir_frontend_metadata.executable_protocol_inheritance_identity_model =
      source_graph.protocol_inheritance_identity_model;
  ir_frontend_metadata.executable_category_attachment_identity_model =
      source_graph.category_attachment_identity_model;
  ir_frontend_metadata
      .executable_protocol_category_conformance_identity_model =
      source_graph.protocol_category_conformance_identity_model;
  ir_frontend_metadata.executable_protocol_category_source_closure_ready =
      source_graph.protocol_category_declaration_closure_complete &&
      source_graph.protocol_inheritance_identity_closure_complete &&
      source_graph.category_attachment_identity_closure_complete &&
      source_graph.protocol_category_conformance_identity_closure_complete;
  ir_frontend_metadata.executable_protocol_category_protocol_node_count =
      source_graph.protocol_nodes_lexicographic.size();
  ir_frontend_metadata.executable_protocol_category_category_node_count =
      source_graph.category_nodes_lexicographic.size();

  std::size_t class_metaclass_parent_identity_edge_count = 0;
  std::size_t class_metaclass_method_owner_identity_edge_count = 0;
  std::size_t class_metaclass_object_identity_edge_count = 0;
  std::size_t protocol_inheritance_identity_edge_count = 0;
  std::size_t category_attachment_identity_edge_count = 0;
  std::size_t protocol_category_conformance_identity_edge_count = 0;
  for (const auto &edge : source_graph.owner_edges_lexicographic) {
    if (edge.edge_kind == "interface-to-superclass" ||
        edge.edge_kind == "interface-to-super-metaclass" ||
        edge.edge_kind == "implementation-to-superclass" ||
        edge.edge_kind == "implementation-to-super-metaclass" ||
        edge.edge_kind == "class-to-superclass" ||
        edge.edge_kind == "metaclass-to-super-metaclass") {
      ++class_metaclass_parent_identity_edge_count;
    }
    if (edge.edge_kind == "interface-to-instance-method-owner" ||
        edge.edge_kind == "interface-to-class-method-owner" ||
        edge.edge_kind == "implementation-to-instance-method-owner" ||
        edge.edge_kind == "implementation-to-class-method-owner") {
      ++class_metaclass_method_owner_identity_edge_count;
    }
    if (edge.edge_kind == "interface-to-class" ||
        edge.edge_kind == "interface-to-metaclass" ||
        edge.edge_kind == "implementation-to-class" ||
        edge.edge_kind == "implementation-to-metaclass" ||
        edge.edge_kind == "class-to-metaclass") {
      ++class_metaclass_object_identity_edge_count;
    }
    if (edge.edge_kind == "protocol-to-inherited-protocol") {
      ++protocol_inheritance_identity_edge_count;
    }
    if (edge.edge_kind == "category-to-class" ||
        edge.edge_kind == "category-to-interface" ||
        edge.edge_kind == "category-to-implementation") {
      ++category_attachment_identity_edge_count;
    }
    if (edge.edge_kind == "category-to-protocol") {
      ++protocol_category_conformance_identity_edge_count;
    }
  }
  ir_frontend_metadata.executable_class_metaclass_parent_identity_edge_count =
      class_metaclass_parent_identity_edge_count;
  ir_frontend_metadata
      .executable_class_metaclass_method_owner_identity_edge_count =
      class_metaclass_method_owner_identity_edge_count;
  ir_frontend_metadata.executable_class_metaclass_object_identity_edge_count =
      class_metaclass_object_identity_edge_count;
  ir_frontend_metadata.executable_protocol_inheritance_identity_edge_count =
      protocol_inheritance_identity_edge_count;
  ir_frontend_metadata.executable_category_attachment_identity_edge_count =
      category_attachment_identity_edge_count;
  ir_frontend_metadata.executable_protocol_category_conformance_identity_edge_count =
      protocol_category_conformance_identity_edge_count;

  ir_frontend_metadata.runtime_metadata_protocol_category_emission_contract_id =
      kObjc3RuntimeProtocolCategoryEmissionContractId;
  ir_frontend_metadata.runtime_metadata_protocol_emission_payload_model =
      kObjc3RuntimeProtocolEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_category_emission_payload_model =
      kObjc3RuntimeCategoryEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_protocol_reference_model =
      kObjc3RuntimeProtocolReferenceModel;
  ir_frontend_metadata.runtime_metadata_category_attachment_model =
      kObjc3RuntimeCategoryAttachmentModel;
  ir_frontend_metadata.runtime_metadata_protocol_category_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;

  ir_frontend_metadata.runtime_metadata_member_table_emission_contract_id =
      kObjc3RuntimeMemberTableEmissionContractId;
  ir_frontend_metadata.runtime_metadata_method_list_emission_payload_model =
      kObjc3RuntimeMethodListEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_method_list_grouping_model =
      kObjc3RuntimeMethodListEmissionGroupingModel;
  ir_frontend_metadata
      .runtime_metadata_property_descriptor_emission_payload_model =
      kObjc3RuntimePropertyDescriptorEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_ivar_descriptor_emission_payload_model =
      kObjc3RuntimeIvarDescriptorEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_member_table_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;

  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_contract_id =
      kObjc3RuntimeArchiveStaticLinkDiscoveryContractId;
  ir_frontend_metadata.runtime_metadata_archive_static_link_anchor_seed_model =
      kObjc3RuntimeArchiveStaticLinkAnchorSeedModel;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_translation_unit_identity_model =
      kObjc3RuntimeArchiveStaticLinkTranslationUnitIdentityModel;
  ir_frontend_metadata.runtime_metadata_archive_static_link_merge_model =
      kObjc3RuntimeArchiveStaticLinkMergeModel;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_response_artifact_suffix =
      kObjc3RuntimeMergedLinkerResponseArtifactSuffix;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_discovery_artifact_suffix =
      kObjc3RuntimeMergedDiscoveryArtifactSuffix;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_ready =
      true;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_translation_unit_identity_key =
      objc3::artifacts::identity::BuildObjc3TranslationUnitIdentityKey(
          objc3::artifacts::identity::Objc3TranslationUnitIdentityEvidence{
              input_path,
              parse_artifact_replay_key,
              lowering_boundary_replay_key,
          });
}

}  // namespace objc3::artifacts::frontend
