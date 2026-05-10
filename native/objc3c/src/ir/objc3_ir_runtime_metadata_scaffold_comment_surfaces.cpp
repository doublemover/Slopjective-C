#include "ir/objc3_ir_runtime_metadata_scaffold_comment_surfaces.h"

#include <cstddef>
#include <sstream>
#include <string>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_module_identity.h"
#include "ir/objc3_ir_runtime_bootstrap_global_emission.h"
#include "ir/objc3_ir_runtime_metadata_scaffold_emission.h"
#include "ir/objc3_ir_runtime_metadata_scaffold_comment_surfaces_executable_object.h"
#include "lower/contracts/conformance_runtime_capability_contracts.h"
#include "lower/contracts/conformance_tooling_report_contracts.h"
#include "lower/contracts/conformance_versioned_report_contracts.h"
#include "lower/contracts/manifest_truth_gate_contracts.h"
#include "lower/contracts/runtime_artifact_retention_contracts.h"
#include "lower/contracts/runtime_bootstrap_entry_lowering_contracts.h"
#include "lower/contracts/runtime_bootstrap_link_discovery_contracts.h"
#include "lower/contracts/runtime_metadata_gate_contracts.h"
#include "lower/contracts/runtime_metadata_layout_ordering_policy_contracts.h"
#include "lower/contracts/runtime_metadata_object_format_contracts.h"
#include "lower/contracts/runtime_metadata_section_publication_contracts.h"
#include "lower/contracts/runtime_metadata_source_record_contracts.h"
#include "lower/metadata/runtime_metadata_layout_policy.h"

namespace {

void EmitRuntimeMetadataCommentSurfaces(
    const Objc3IRRuntimeMetadataScaffoldEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    bool emit_class_metaclass_bundle_payloads,
    bool emit_protocol_category_bundle_payloads,
    bool emit_member_table_payloads, std::ostringstream &out) {
  const Objc3IRFrontendMetadata &frontend_metadata = options.frontend_metadata;

  out << "; runtime_metadata_layout_policy = "
      << Objc3RuntimeMetadataLayoutPolicyReplayKey(layout_policy) << "\n";
  out << "; runtime_metadata_section_emission_boundary = "
      << Objc3RuntimeMetadataSectionEmissionBoundarySummary() << "\n";
  out << "; runtime_bootstrap_lowering_boundary = "
      << Objc3RuntimeBootstrapLoweringBoundarySummary() << "\n";
  if (frontend_metadata.versioned_conformance_report_lowering_ready &&
      !frontend_metadata.versioned_conformance_report_lowering_replay_key
           .empty()) {
    out << "; versioned_conformance_report_lowering = "
        << Objc3VersionedConformanceReportLoweringContractSummary()
        << ";replay_key="
        << frontend_metadata.versioned_conformance_report_lowering_replay_key
        << "\n";
    out << "; runtime_capability_reporting = "
        << Objc3RuntimeCapabilityReportingContractSummary()
        << ";replay_key="
        << frontend_metadata.versioned_conformance_report_lowering_replay_key
        << "\n";
    out << "; tooling_machine_readable_conformance_report_contract = "
        << Objc3ToolingMachineReadableConformanceReportContractLoweringSummary()
        << ";replay_key="
        << frontend_metadata.versioned_conformance_report_lowering_replay_key
        << "\n";
    out << "; tooling_feature_aware_conformance_report_emission = "
        << Objc3ToolingFeatureAwareConformanceReportEmissionLoweringSummary()
        << ";replay_key="
        << frontend_metadata.versioned_conformance_report_lowering_replay_key
        << "\n";
    out << "; tooling_corpus_sharding_release_evidence_packaging = "
        << Objc3ToolingCorpusShardingReleaseEvidencePackagingLoweringSummary()
        << ";replay_key="
        << frontend_metadata.versioned_conformance_report_lowering_replay_key
        << "\n";
  }

  Objc3IRRuntimeBootstrapMetadataCommentOptions bootstrap_comment_options;
  bootstrap_comment_options.selector_pool_globals_empty =
      options.selector_pool_globals.empty();
  bootstrap_comment_options.runtime_string_pool_globals_empty =
      options.runtime_string_pool_globals.empty();
  EmitObjc3IRRuntimeBootstrapMetadataComments(
      frontend_metadata, options.runtime_metadata_symbols,
      bootstrap_comment_options, out);

  if (emit_class_metaclass_bundle_payloads) {
    std::size_t total_instance_method_refs = 0;
    std::size_t total_class_method_refs = 0;
    for (const auto &bundle :
         frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic) {
      total_instance_method_refs += bundle.instance_method_count;
      total_class_method_refs += bundle.class_method_count;
    }
    out << "; runtime_metadata_class_metaclass_emission = "
        << Objc3RuntimeMetadataClassMetaclassEmissionSummary()
        << ";bundle_count="
        << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
               .size()
        << ";instance_method_refs=" << total_instance_method_refs
        << ";class_method_refs=" << total_class_method_refs
        << ";typed_handoff_replay="
        << frontend_metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key
        << "\n";
  }
  if (emit_protocol_category_bundle_payloads) {
    std::size_t total_inherited_protocol_refs = 0;
    for (const auto &bundle :
         frontend_metadata.runtime_metadata_protocol_bundles_lexicographic) {
      total_inherited_protocol_refs +=
          bundle.inherited_protocol_owner_identities_lexicographic.size();
    }
    std::size_t total_adopted_protocol_refs = 0;
    std::size_t total_category_attachment_refs = 0;
    for (const auto &bundle :
         frontend_metadata.runtime_metadata_category_bundles_lexicographic) {
      total_adopted_protocol_refs +=
          bundle.adopted_protocol_owner_identities_lexicographic.size();
      total_category_attachment_refs += 3u;
    }
    out << "; runtime_metadata_protocol_category_emission = "
        << Objc3RuntimeMetadataProtocolCategoryEmissionSummary()
        << ";protocol_bundle_count="
        << frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size()
        << ";inherited_protocol_refs=" << total_inherited_protocol_refs
        << ";category_bundle_count="
        << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
        << ";adopted_protocol_refs=" << total_adopted_protocol_refs
        << ";attachment_refs=" << total_category_attachment_refs
        << ";typed_handoff_replay="
        << frontend_metadata.runtime_metadata_protocol_category_typed_handoff_replay_key
        << "\n";
  }
  if (emit_member_table_payloads) {
    std::size_t total_method_entries = 0;
    for (const auto &bundle :
         frontend_metadata.runtime_metadata_method_list_bundles_lexicographic) {
      total_method_entries += bundle.entries_lexicographic.size();
    }
    out << "; runtime_metadata_member_table_emission = "
        << Objc3RuntimeMetadataMemberTableEmissionSummary()
        << ";method_list_bundle_count="
        << frontend_metadata.runtime_metadata_method_list_bundles_lexicographic
               .size()
        << ";method_entry_count=" << total_method_entries
        << ";property_descriptor_count="
        << frontend_metadata.runtime_metadata_property_bundles_lexicographic.size()
        << ";ivar_descriptor_count="
        << frontend_metadata.runtime_metadata_ivar_bundles_lexicographic.size()
        << ";typed_handoff_replay="
        << frontend_metadata.runtime_metadata_member_table_typed_handoff_replay_key
        << "\n";
  }
}

void EmitRuntimeMetadataPackagingCommentSurfaces(
    const Objc3IRRuntimeMetadataScaffoldEmissionOptions &options,
    std::ostringstream &out) {
  const Objc3IRFrontendMetadata &frontend_metadata = options.frontend_metadata;
  if (!options.selector_pool_globals.empty() ||
      !options.runtime_string_pool_globals.empty()) {
    out << "; runtime_metadata_selector_string_pool_emission = "
        << Objc3RuntimeMetadataSelectorStringPoolEmissionSummary()
        << ";selector_pool_count=" << options.selector_pool_globals.size()
        << ";string_pool_count=" << options.runtime_string_pool_globals.size()
        << ";selector_section="
        << Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeSelectorPoolLogicalSection)
        << ";string_section="
        << Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeStringPoolLogicalSection)
        << "\n";
  }
  if (!options.typed_keypath_artifacts.empty()) {
    out << "; typed_keypath_artifact_emission = "
        << "contract=objc3c.type_system.typed.keypath.artifact.emission.v1"
        << ";keypath_count=" << options.typed_keypath_artifacts.size()
        << ";section="
        << Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeKeypathDescriptorLogicalSection)
        << ";aggregate_symbol=@__objc3_sec_keypath_descriptors"
        << ";generic_metadata_abi_replay_key="
        << (frontend_metadata.lowering_generic_metadata_abi_replay_key.empty()
                ? "none"
                : frontend_metadata.lowering_generic_metadata_abi_replay_key)
        << "\n";
  }
  out << "; runtime_metadata_binary_inspection_harness = "
      << Objc3RuntimeMetadataBinaryInspectionHarnessSummary()
      << ";positive_case_count=4"
      << ";negative_case_count=1"
      << ";section_inventory_command="
      << kObjc3RuntimeBinaryInspectionSectionCommand
      << ";symbol_inventory_command="
      << kObjc3RuntimeBinaryInspectionSymbolCommand << "\n";
  out << "; runtime_metadata_object_packaging_retention = "
      << Objc3RuntimeMetadataObjectPackagingRetentionSummary()
      << ";section_inventory_command="
      << kObjc3RuntimeBinaryInspectionSectionCommand
      << ";symbol_inventory_command="
      << kObjc3RuntimeBinaryInspectionSymbolCommand << "\n";
  out << "; runtime_metadata_linker_retention = "
      << Objc3RuntimeMetadataLinkerRetentionSummary()
      << ";linker_anchor_symbol="
      << options.runtime_metadata_symbols.linker_anchor_symbol
      << ";discovery_root_symbol="
      << options.runtime_metadata_symbols.discovery_root_symbol
      << ";linker_anchor_logical_section="
      << kObjc3RuntimeLinkerAnchorLogicalSection
      << ";discovery_root_logical_section="
      << kObjc3RuntimeLinkerDiscoveryRootLogicalSection
      << ";linker_response_artifact_suffix="
      << kObjc3RuntimeLinkerResponseArtifactSuffix
      << ";discovery_artifact_suffix="
      << kObjc3RuntimeLinkerDiscoveryArtifactSuffix
      << ";translation_unit_identity_key="
      << frontend_metadata
             .runtime_metadata_archive_static_link_translation_unit_identity_key
      << ";translation_unit_identity_key_hex="
      << EncodeBoundaryTokenValueHex(
             frontend_metadata
                 .runtime_metadata_archive_static_link_translation_unit_identity_key)
      << "\n";
  out << "; runtime_metadata_archive_static_link_discovery = "
      << Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary()
      << ";translation_unit_identity_key="
      << frontend_metadata
             .runtime_metadata_archive_static_link_translation_unit_identity_key
      << ";translation_unit_identity_key_hex="
      << EncodeBoundaryTokenValueHex(
             frontend_metadata
                 .runtime_metadata_archive_static_link_translation_unit_identity_key)
      << ";merged_linker_response_artifact_suffix="
      << frontend_metadata
             .runtime_metadata_archive_static_link_response_artifact_suffix
      << ";merged_discovery_artifact_suffix="
      << frontend_metadata
             .runtime_metadata_archive_static_link_discovery_artifact_suffix
      << "\n";
  out << "; runtime_metadata_emission_gate = "
      << Objc3RuntimeMetadataEmissionGateSummary()
      << ";source_to_section_matrix_surface=objc3c.runtime.metadata.sourcetosectionmatrix.v1"
      << ";object_format_policy_surface=objc3c.runtime.metadata.objectformatpolicy.v1"
      << ";binary_inspection_surface=objc3c.runtime.metadata.binaryinspection.v1"
      << ";archive_static_link_surface=objc3c.runtime.metadata.archivestaticlink.v1"
      << "\n";
  out << "; runtime_metadata_object_emission_closeout = "
      << Objc3RuntimeMetadataObjectEmissionCloseoutSummary()
      << ";dependency_gate_surface=objc3c.runtime.metadata.dependencygate.v1"
      << ";class_object_case_surface=objc3c.runtime.metadata.sourcetosectionmatrix.v1"
      << ";binary_object_case_surface=objc3c.runtime.metadata.binaryinspection.v1"
      << ";linker_fanin_surface=objc3c.runtime.metadata.archivestaticlink.v1"
      << "\n";
  out << "; manifest_object_ir_truth_gate = "
      << Objc3ManifestObjectIrTruthGateSummary()
      << ";manifest_artifact=module.manifest.json"
      << ";ir_artifact=module.ll"
      << ";object_artifact=module.obj"
      << ";registration_descriptor_artifact=module.runtime-registration-descriptor.json"
      << ";registration_manifest_artifact=module.runtime-registration-manifest.json"
      << ";conformance_report_artifact=module.objc3-conformance-report.json"
      << "\n";
}

}  // namespace

void EmitObjc3IRRuntimeMetadataScaffoldCommentSurfaces(
    const Objc3IRRuntimeMetadataScaffoldEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    bool emit_class_metaclass_bundle_payloads,
    bool emit_protocol_category_bundle_payloads,
    bool emit_member_table_payloads, std::ostringstream &out) {
  EmitRuntimeMetadataCommentSurfaces(
      options, layout_policy, emit_class_metaclass_bundle_payloads,
      emit_protocol_category_bundle_payloads, emit_member_table_payloads, out);
  EmitObjc3IRRuntimeMetadataScaffoldExecutableObjectCommentSurfaces(
      options, emit_class_metaclass_bundle_payloads,
      emit_protocol_category_bundle_payloads, emit_member_table_payloads, out);
  EmitRuntimeMetadataPackagingCommentSurfaces(options, out);
}
