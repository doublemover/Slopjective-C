#include "ir/objc3_ir_runtime_metadata_scaffold_emission.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_module_identity.h"
#include "ir/objc3_ir_runtime_artifact_emission.h"
#include "ir/objc3_ir_runtime_bootstrap_global_emission.h"
#include "ir/objc3_ir_runtime_member_metadata_emission.h"
#include "ir/objc3_ir_runtime_object_metadata_emission.h"
#include "lower/contracts/conformance_runtime_capability_contracts.h"
#include "lower/contracts/conformance_tooling_report_contracts.h"
#include "lower/contracts/conformance_versioned_report_contracts.h"
#include "lower/contracts/executable_object_artifact_layout_contracts.h"
#include "lower/contracts/manifest_truth_gate_contracts.h"
#include "lower/contracts/runtime_artifact_retention_contracts.h"
#include "lower/contracts/runtime_bootstrap_entry_lowering_contracts.h"
#include "lower/contracts/runtime_bootstrap_link_discovery_contracts.h"
#include "lower/contracts/runtime_metadata_gate_contracts.h"
#include "lower/contracts/runtime_metadata_layout_ordering_policy_contracts.h"
#include "lower/contracts/runtime_metadata_object_format_contracts.h"
#include "lower/contracts/runtime_metadata_section_publication_contracts.h"
#include "lower/contracts/runtime_metadata_source_record_contracts.h"
#include "lower/contracts/runtime_object_executable_support_contracts.h"
#include "lower/contracts/runtime_object_realization_contracts.h"
#include "lower/contracts/runtime_object_sample_support_contracts.h"
#include "lower/metadata/runtime_metadata_layout_policy.h"

namespace {

std::string BuildMethodListKey(const std::string &owner_family_kind,
                               const std::string &owner_identity,
                               const std::string &list_kind) {
  return owner_family_kind + "|" + owner_identity + "|" + list_kind;
}

void EmitGenericDescriptorSection(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  std::vector<std::string> descriptor_symbols;
  descriptor_symbols.reserve(family.descriptor_count);
  for (std::size_t i = 0; i < family.descriptor_count; ++i) {
    const std::string descriptor_symbol =
        BuildObjc3IRRuntimeMetadataDescriptorSymbol(
            layout_policy.descriptor_symbol_prefix, family.kind, i);
    descriptor_symbols.push_back(descriptor_symbol);
    out << descriptor_symbol << " = " << layout_policy.descriptor_linkage
        << " global [1 x i8] zeroinitializer, section \""
        << family.emitted_section_name << "\", align 1\n";
    retained_globals.push_back(descriptor_symbol);
  }

  const std::string aggregate_symbol = "@" + family.aggregate_symbol_name;
  out << aggregate_symbol << " = " << layout_policy.aggregate_linkage
      << (descriptor_symbols.empty() ? " constant " : " global ");
  if (descriptor_symbols.empty()) {
    out << "{ i64 } { i64 0 }";
  } else {
    out << "{ i64, [" << descriptor_symbols.size()
        << " x ptr] } { i64 " << descriptor_symbols.size() << ", ["
        << descriptor_symbols.size() << " x ptr] [";
    for (std::size_t i = 0; i < descriptor_symbols.size(); ++i) {
      if (i != 0) {
        out << ", ";
      }
      out << "ptr " << descriptor_symbols[i];
    }
    out << "] }";
  }
  out << ", section \"" << family.emitted_section_name << "\", align 8\n";
  retained_globals.push_back(aggregate_symbol);
}

bool BindImplementationMethodSymbols(
    const std::vector<Objc3IRMethodDefinition> &method_definitions,
    std::unordered_map<std::string, std::string>
        &implementation_method_symbols_by_owner_identity,
    std::string &error) {
  implementation_method_symbols_by_owner_identity.reserve(
      method_definitions.size());
  for (const Objc3IRMethodDefinition &method_def : method_definitions) {
    if (method_def.method_owner_identity.empty()) {
      continue;
    }
    const std::string implementation_symbol = "@" + method_def.symbol;
    const auto [binding_it, inserted] =
        implementation_method_symbols_by_owner_identity.emplace(
            method_def.method_owner_identity, implementation_symbol);
    if (!inserted && binding_it->second != implementation_symbol) {
      error = "duplicate executable method-body binding for owner identity '" +
              method_def.method_owner_identity + "'";
      return false;
    }
  }
  return true;
}

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

void EmitExecutableObjectCommentSurfaces(
    const Objc3IRRuntimeMetadataScaffoldEmissionOptions &options,
    bool emit_class_metaclass_bundle_payloads,
    bool emit_protocol_category_bundle_payloads,
    bool emit_member_table_payloads, std::ostringstream &out) {
  const Objc3IRFrontendMetadata &frontend_metadata = options.frontend_metadata;
  if (!emit_class_metaclass_bundle_payloads ||
      !emit_protocol_category_bundle_payloads || !emit_member_table_payloads) {
    return;
  }

  std::unordered_set<std::string> implementation_method_owner_identities;
  implementation_method_owner_identities.reserve(
      options.method_definitions.size());
  for (const Objc3IRMethodDefinition &method_def : options.method_definitions) {
    if (method_def.method_owner_identity.empty()) {
      continue;
    }
    implementation_method_owner_identities.insert(
        method_def.method_owner_identity);
  }
  std::size_t executable_method_entry_count = 0;
  std::size_t bound_method_entry_count = 0;
  for (const auto &bundle :
       frontend_metadata.runtime_metadata_method_list_bundles_lexicographic) {
    const bool implementation_owned =
        bundle.owner_kind == "class-implementation" ||
        bundle.owner_kind == "category-implementation";
    if (!implementation_owned) {
      continue;
    }
    for (const auto &entry : bundle.entries_lexicographic) {
      if (!entry.has_body) {
        continue;
      }
      ++executable_method_entry_count;
      if (implementation_method_owner_identities.find(entry.owner_identity) !=
          implementation_method_owner_identities.end()) {
        ++bound_method_entry_count;
      }
    }
  }
  out << "; executable_object_artifact_lowering = "
      << Objc3ExecutableObjectArtifactLoweringSummary()
      << ";class_realization_record_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << ";category_realization_record_count="
      << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
      << ";implementation_method_definition_count="
      << options.method_definitions.size()
      << ";executable_method_entry_count=" << executable_method_entry_count
      << ";bound_method_entry_count=" << bound_method_entry_count
      << ";class_handoff_replay="
      << frontend_metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key
      << ";protocol_category_handoff_replay="
      << frontend_metadata.runtime_metadata_protocol_category_typed_handoff_replay_key
      << ";member_table_handoff_replay="
      << frontend_metadata.runtime_metadata_member_table_typed_handoff_replay_key
      << "\n";
  out << "; executable_method_body_binding = "
      << Objc3ExecutableMethodBodyBindingSummary()
      << ";implementation_method_definition_count="
      << options.method_definitions.size()
      << ";executable_method_entry_count=" << executable_method_entry_count
      << ";bound_method_entry_count=" << bound_method_entry_count << "\n";
  out << "; executable_realization_records = "
      << Objc3ExecutableRealizationRecordsSummary()
      << ";class_record_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << ";protocol_record_count="
      << frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size()
      << ";category_record_count="
      << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
      << "\n";
  out << "; runtime_class_realization = "
      << Objc3RuntimeClassRealizationSummary()
      << ";class_bundle_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << ";protocol_record_count="
      << frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size()
      << ";category_record_count="
      << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
      << "\n";
  out << "; runtime_metaclass_graph_root_class_baseline = "
      << Objc3RuntimeMetaclassGraphRootClassSummary()
      << ";class_bundle_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << ";receiver_binding_candidate_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << "\n";
  std::size_t class_protocol_ref_count = 0;
  for (const auto &bundle :
       frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic) {
    class_protocol_ref_count +=
        bundle.adopted_protocol_owner_identities_lexicographic.size();
  }
  std::size_t category_protocol_ref_count = 0;
  for (const auto &bundle :
       frontend_metadata.runtime_metadata_category_bundles_lexicographic) {
    category_protocol_ref_count +=
        bundle.adopted_protocol_owner_identities_lexicographic.size();
  }
  out << "; runtime_category_attachment_protocol_conformance = "
      << Objc3RuntimeCategoryAttachmentProtocolConformanceSummary()
      << ";attached_category_candidate_count="
      << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
      << ";class_protocol_ref_count=" << class_protocol_ref_count
      << ";category_protocol_ref_count=" << category_protocol_ref_count
      << "\n";
  out << "; runtime_canonical_runnable_object_sample_support = "
      << Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary()
      << ";class_bundle_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << ";attached_category_candidate_count="
      << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
      << ";builtin_object_sample_selector_count=3\n";
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
      << ";symbol_inventory_command=" << kObjc3RuntimeBinaryInspectionSymbolCommand
      << "\n";
  out << "; runtime_metadata_object_packaging_retention = "
      << Objc3RuntimeMetadataObjectPackagingRetentionSummary()
      << ";section_inventory_command="
      << kObjc3RuntimeBinaryInspectionSectionCommand
      << ";symbol_inventory_command=" << kObjc3RuntimeBinaryInspectionSymbolCommand
      << "\n";
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

bool EmitObjc3IRRuntimeMetadataSectionScaffold(
    const Objc3IRRuntimeMetadataScaffoldEmissionOptions &options,
    std::ostringstream &out, std::string &error) {
  const Objc3IRFrontendMetadata &frontend_metadata = options.frontend_metadata;
  if (!Objc3IRRuntimeMetadataSectionScaffoldReady(frontend_metadata)) {
    return true;
  }

  Objc3RuntimeMetadataLayoutPolicy layout_policy;
  std::string layout_policy_error;
  if (!BuildObjc3IRRuntimeMetadataLayoutPolicy(
          frontend_metadata, layout_policy, layout_policy_error) ||
      !IsReadyObjc3RuntimeMetadataLayoutPolicy(layout_policy)) {
    return true;
  }

  const bool emit_class_metaclass_bundle_payloads =
      frontend_metadata.runtime_metadata_class_metaclass_emission_ready &&
      frontend_metadata.runtime_metadata_class_metaclass_emission_fail_closed &&
      !frontend_metadata
           .runtime_metadata_class_metaclass_emission_contract_id.empty() &&
      frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
              .size() == layout_policy.families[0].descriptor_count;
  const bool emit_protocol_category_bundle_payloads =
      frontend_metadata.runtime_metadata_protocol_category_emission_ready &&
      frontend_metadata.runtime_metadata_protocol_category_emission_fail_closed &&
      !frontend_metadata
           .runtime_metadata_protocol_category_emission_contract_id.empty() &&
      frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size() ==
          layout_policy.families[1].descriptor_count &&
      frontend_metadata.runtime_metadata_category_bundles_lexicographic.size() ==
          layout_policy.families[2].descriptor_count;
  const bool emit_member_table_payloads =
      frontend_metadata.runtime_metadata_member_table_emission_ready &&
      frontend_metadata.runtime_metadata_member_table_emission_fail_closed &&
      !frontend_metadata.runtime_metadata_member_table_emission_contract_id
           .empty() &&
      frontend_metadata.runtime_metadata_property_bundles_lexicographic.size() ==
          layout_policy.families[3].descriptor_count &&
      frontend_metadata.runtime_metadata_ivar_bundles_lexicographic.size() ==
          layout_policy.families[4].descriptor_count;

  EmitRuntimeMetadataCommentSurfaces(
      options, layout_policy, emit_class_metaclass_bundle_payloads,
      emit_protocol_category_bundle_payloads, emit_member_table_payloads, out);
  EmitExecutableObjectCommentSurfaces(
      options, emit_class_metaclass_bundle_payloads,
      emit_protocol_category_bundle_payloads, emit_member_table_payloads, out);
  EmitRuntimeMetadataPackagingCommentSurfaces(options, out);
  out << "; runtime metadata section publication globals\n";

  std::vector<std::string> retained_globals;
  retained_globals.reserve(layout_policy.total_retained_global_count);

  std::unordered_map<std::string, std::string>
      protocol_descriptor_symbols_by_owner_identity;
  if (emit_protocol_category_bundle_payloads) {
    protocol_descriptor_symbols_by_owner_identity.reserve(
        frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size());
    for (std::size_t i = 0;
         i <
         frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size();
         ++i) {
      const auto &bundle =
          frontend_metadata.runtime_metadata_protocol_bundles_lexicographic[i];
      protocol_descriptor_symbols_by_owner_identity.emplace(
          bundle.owner_identity, BuildObjc3IRRuntimeMetadataDescriptorSymbol(
                                     layout_policy.descriptor_symbol_prefix,
                                     kObjc3RuntimeMetadataLayoutPolicyProtocolFamily,
                                     i));
    }
  }

  std::unordered_map<std::string, std::string> method_list_symbols_by_key;
  std::unordered_map<std::string, std::size_t>
      method_list_entry_counts_by_key;
  std::unordered_map<std::string, std::string>
      implementation_method_symbols_by_owner_identity;
  if (emit_member_table_payloads) {
    method_list_symbols_by_key.reserve(
        frontend_metadata.runtime_metadata_method_list_bundles_lexicographic
            .size());
    method_list_entry_counts_by_key.reserve(
        frontend_metadata.runtime_metadata_method_list_bundles_lexicographic
            .size());
    for (std::size_t i = 0;
         i <
         frontend_metadata.runtime_metadata_method_list_bundles_lexicographic
             .size();
         ++i) {
      const auto &bundle =
          frontend_metadata.runtime_metadata_method_list_bundles_lexicographic[i];
      method_list_symbols_by_key.emplace(
          BuildMethodListKey(bundle.owner_family_kind,
                             bundle.declaration_owner_identity,
                             bundle.list_kind),
          BuildObjc3IRRuntimeMetadataAuxiliarySymbol(
              layout_policy.descriptor_symbol_prefix, bundle.owner_family_kind,
              bundle.list_kind + "_methods", i));
      method_list_entry_counts_by_key.emplace(
          BuildMethodListKey(bundle.owner_family_kind,
                             bundle.declaration_owner_identity,
                             bundle.list_kind),
          bundle.entries_lexicographic.size());
    }
    if (!BindImplementationMethodSymbols(
            options.method_definitions,
            implementation_method_symbols_by_owner_identity, error)) {
      return false;
    }
  }

  const std::string image_info_symbol = "@" + layout_policy.image_info_symbol;
  out << image_info_symbol << " = " << layout_policy.aggregate_linkage
      << " global { i32, i32 } zeroinitializer, section \""
      << layout_policy.emitted_image_info_section << "\", align 4\n";
  retained_globals.push_back(image_info_symbol);

  for (const auto &family : layout_policy.families) {
    if (emit_member_table_payloads &&
        (family.kind == kObjc3RuntimeMetadataLayoutPolicyClassFamily ||
         family.kind == kObjc3RuntimeMetadataLayoutPolicyProtocolFamily ||
         family.kind == kObjc3RuntimeMetadataLayoutPolicyCategoryFamily)) {
      if (!EmitObjc3IRRuntimeMethodListBundlesForFamily(
              Objc3IRRuntimeMemberMetadataEmissionOptions{
                  frontend_metadata, layout_policy, method_list_symbols_by_key,
                  implementation_method_symbols_by_owner_identity},
              family, out, retained_globals, error)) {
        if (error.empty()) {
          error = "runtime metadata method-body binding failed";
        }
        return false;
      }
    }
    if (emit_class_metaclass_bundle_payloads &&
        family.kind == kObjc3RuntimeMetadataLayoutPolicyClassFamily) {
      EmitObjc3IRRuntimeClassMetaclassBundleSection(
          Objc3IRRuntimeObjectMetadataEmissionOptions{
              frontend_metadata, layout_policy, method_list_symbols_by_key,
              method_list_entry_counts_by_key,
              protocol_descriptor_symbols_by_owner_identity},
          family, out, retained_globals);
    } else if (emit_protocol_category_bundle_payloads &&
               family.kind == kObjc3RuntimeMetadataLayoutPolicyProtocolFamily) {
      EmitObjc3IRRuntimeProtocolBundleSection(
          Objc3IRRuntimeObjectMetadataEmissionOptions{
              frontend_metadata, layout_policy, method_list_symbols_by_key,
              method_list_entry_counts_by_key,
              protocol_descriptor_symbols_by_owner_identity},
          family, out, retained_globals);
    } else if (emit_protocol_category_bundle_payloads &&
               family.kind == kObjc3RuntimeMetadataLayoutPolicyCategoryFamily) {
      EmitObjc3IRRuntimeCategoryBundleSection(
          Objc3IRRuntimeObjectMetadataEmissionOptions{
              frontend_metadata, layout_policy, method_list_symbols_by_key,
              method_list_entry_counts_by_key,
              protocol_descriptor_symbols_by_owner_identity},
          family, out, retained_globals);
    } else if (emit_member_table_payloads &&
               family.kind == kObjc3RuntimeMetadataLayoutPolicyPropertyFamily) {
      EmitObjc3IRRuntimePropertyDescriptorSection(
          Objc3IRRuntimeMemberMetadataEmissionOptions{
              frontend_metadata, layout_policy, method_list_symbols_by_key,
              implementation_method_symbols_by_owner_identity},
          family, out, retained_globals, error);
    } else if (emit_member_table_payloads &&
               family.kind == kObjc3RuntimeMetadataLayoutPolicyIvarFamily) {
      EmitObjc3IRRuntimeIvarDescriptorSection(
          Objc3IRRuntimeMemberMetadataEmissionOptions{
              frontend_metadata, layout_policy, method_list_symbols_by_key,
              implementation_method_symbols_by_owner_identity},
          family, out, retained_globals);
    } else {
      EmitGenericDescriptorSection(layout_policy, family, out,
                                   retained_globals);
    }
  }

  return EmitObjc3IRRuntimeArtifacts(
      Objc3IRRuntimeArtifactEmissionOptions{
          options.module_name,
          frontend_metadata,
          options.runtime_metadata_symbols,
          layout_policy,
          options.selector_pool_globals,
          options.runtime_string_pool_globals,
          options.typed_keypath_artifacts,
          image_info_symbol,
          options.runtime_metadata_symbols.discovery_root_symbol,
          options.runtime_metadata_symbols.linker_anchor_symbol,
          options.emit_runtime_bootstrap_lowering,
          options.emit_runtime_bootstrap_registration_descriptor_image_root},
      out, retained_globals, error);
}
