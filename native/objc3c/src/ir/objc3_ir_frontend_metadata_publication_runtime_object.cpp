#include "ir/objc3_ir_frontend_metadata_publication_runtime_object.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRRuntimeMetadataObjectPublicationNodes(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out) {
  out << "!55 = !{!\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.contract_id)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.abi_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.scaffold_contract_id)
      << "\", i1 " << (runtime_metadata_layout_policy.ready ? 1 : 0)
      << ", i1 " << (runtime_metadata_layout_policy.fail_closed ? 1 : 0)
      << ", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.family_ordering_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.descriptor_ordering_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.aggregate_relocation_policy)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.comdat_policy)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.visibility_spelling_policy)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.retention_ordering_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.object_format_policy_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.object_format_surface_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.object_format)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.section_spelling_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.retention_anchor_model)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.descriptor_linkage)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.aggregate_linkage)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.metadata_visibility)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.retention_root)
      << "\", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_layout_policy.total_retained_global_count)
      << ", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataLayoutPolicyReplayKey(
             runtime_metadata_layout_policy))
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.failure_reason)
      << "\"}\n";

  std::size_t runtime_metadata_class_bundle_count = 0;
  std::size_t runtime_metadata_instance_method_reference_total = 0;
  std::size_t runtime_metadata_class_method_reference_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_class_metaclass_bundles_lexicographic) {
    ++runtime_metadata_class_bundle_count;
    runtime_metadata_instance_method_reference_total +=
        bundle.instance_method_count;
    runtime_metadata_class_method_reference_total += bundle.class_method_count;
  }
  out << "!56 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_class_metaclass_name_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_super_link_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_method_list_reference_model)
      << "\", i1 "
      << (metadata.runtime_metadata_class_metaclass_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_class_metaclass_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_class_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_instance_method_reference_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_class_method_reference_total)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key)
      << "\"}\n";

  std::size_t runtime_metadata_protocol_bundle_count = 0;
  std::size_t runtime_metadata_protocol_inherited_reference_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_protocol_bundles_lexicographic) {
    ++runtime_metadata_protocol_bundle_count;
    runtime_metadata_protocol_inherited_reference_total +=
        bundle.inherited_protocol_owner_identities_lexicographic.size();
  }
  std::size_t runtime_metadata_category_bundle_count = 0;
  std::size_t runtime_metadata_category_adopted_reference_total = 0;
  std::size_t runtime_metadata_category_attachment_reference_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_category_bundles_lexicographic) {
    ++runtime_metadata_category_bundle_count;
    runtime_metadata_category_adopted_reference_total +=
        bundle.adopted_protocol_owner_identities_lexicographic.size();
    runtime_metadata_category_attachment_reference_total += 3u;
  }
  out << "!57 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_protocol_category_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_protocol_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_category_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_protocol_reference_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_category_attachment_model)
      << "\", i1 "
      << (metadata.runtime_metadata_protocol_category_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_protocol_category_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_protocol_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_protocol_inherited_reference_total)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_category_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_category_adopted_reference_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_category_attachment_reference_total)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_protocol_category_typed_handoff_replay_key)
      << "\"}\n";

  std::size_t runtime_metadata_method_list_bundle_count = 0;
  std::size_t runtime_metadata_method_entry_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_method_list_bundles_lexicographic) {
    ++runtime_metadata_method_list_bundle_count;
    runtime_metadata_method_entry_total += bundle.entries_lexicographic.size();
  }
  out << "!58 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_member_table_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_method_list_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_method_list_grouping_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_property_descriptor_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_ivar_descriptor_emission_payload_model)
      << "\", i1 "
      << (metadata.runtime_metadata_member_table_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_member_table_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_method_list_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_method_entry_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_property_bundles_lexicographic.size())
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_ivar_bundles_lexicographic.size())
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_member_table_typed_handoff_replay_key)
      << "\"}\n";

  out << "!59 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeSelectorStringPoolEmissionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeSelectorPoolEmissionPayloadModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStringPoolEmissionPayloadModel)
      << "\", i64 "
      << static_cast<unsigned long long>(selector_pool_global_count)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_string_pool_global_count)
      << ", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
             kObjc3RuntimeSelectorPoolLogicalSection))
      << "\", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
             kObjc3RuntimeStringPoolLogicalSection))
      << "\"}\n";
  out << "!60 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionHarnessContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionPositiveCorpusModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionNegativeCorpusModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSectionCommand)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSymbolCommand)
      << "\", i64 4, i64 1}\n";
  out << "!61 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionBoundaryModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionAnchorModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionArtifact)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionSymbolPrefix)
      << "\"}\n";
  out << "!62 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeLinkerRetentionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerRetentionAnchorModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerAnchorLogicalSection)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryRootLogicalSection)
      << "\", !\"" << EscapeCStringLiteral(runtime_metadata_linker_anchor_symbol)
      << "\", !\"" << EscapeCStringLiteral(runtime_metadata_discovery_root_symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerResponseArtifactSuffix)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryArtifactSuffix)
      << "\"}\n";
  out << "!63 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_discovery_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_anchor_seed_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .runtime_metadata_archive_static_link_translation_unit_identity_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_merge_model)
      << "\", i1 "
      << (metadata.runtime_metadata_archive_static_link_discovery_ready ? 1 : 0)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_response_artifact_suffix)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_discovery_artifact_suffix)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .runtime_metadata_archive_static_link_translation_unit_identity_key)
      << "\"}\n";
  out << "!64 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateFailureModel)
      << "\"}\n";
  out << "!65 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataObjectEmissionCloseoutContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel)
      << "\"}\n";
}
