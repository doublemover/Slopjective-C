#pragma once

#include "pipeline/results/runtime_import_evidence_record.h"

struct Objc3RuntimeStorageReflectionArtifactPreservationSummary {
  std::string contract_id =
      kObjc3RuntimeStorageReflectionArtifactPreservationContractId;
  std::string source_contract_id =
      kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId;
  std::string dispatch_and_synthesized_accessor_lowering_surface_contract_id =
      kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId;
  std::string executable_property_accessor_layout_lowering_contract_id =
      kObjc3ExecutablePropertyAccessorLayoutLoweringContractId;
  std::string executable_ivar_layout_emission_contract_id =
      kObjc3ExecutableIvarLayoutEmissionContractId;
  std::string executable_synthesized_accessor_property_lowering_contract_id =
      kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId;
  std::string surface_path =
      kObjc3RuntimeStorageReflectionArtifactPreservationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3RuntimeStorageReflectionArtifactPreservationImportArtifactMemberName;
  std::string source_model =
      kObjc3RuntimeStorageReflectionArtifactPreservationSourceModel;
  std::string preservation_model =
      kObjc3RuntimeStorageReflectionArtifactPreservationModel;
  std::string fail_closed_model =
      kObjc3RuntimeStorageReflectionArtifactPreservationFailClosedModel;
  std::size_t local_property_descriptor_count = 0;
  std::size_t local_ivar_descriptor_count = 0;
  std::size_t implementation_owned_property_entries = 0;
  std::size_t synthesized_accessor_owner_entries = 0;
  std::size_t synthesized_getter_entries = 0;
  std::size_t synthesized_setter_entries = 0;
  std::size_t synthesized_accessor_entries = 0;
  std::size_t current_property_read_entries = 0;
  std::size_t current_property_write_entries = 0;
  std::size_t current_property_exchange_entries = 0;
  std::size_t weak_current_property_load_entries = 0;
  std::size_t weak_current_property_store_entries = 0;
  std::size_t ivar_layout_entries = 0;
  std::size_t ivar_layout_owner_entries = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
  std::string replay_key;
};

inline Objc3RuntimeStorageReflectionArtifactPreservationSummary
BuildObjc3RuntimeStorageReflectionArtifactPreservationSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  Objc3RuntimeStorageReflectionArtifactPreservationSummary summary;
  summary.local_property_descriptor_count =
      records.properties_lexicographic.size();
  summary.local_ivar_descriptor_count = records.ivars_lexicographic.size();
  summary.deterministic = records.deterministic;

  for (const auto &property_record : records.properties_lexicographic) {
    if (!property_record.synthesizes_executable_accessors) {
      continue;
    }

    ++summary.synthesized_accessor_owner_entries;
    if (property_record.owner_kind == "class-implementation") {
      ++summary.implementation_owned_property_entries;
    }
    if (!property_record.getter_storage_runtime_helper_symbol.empty()) {
      ++summary.synthesized_getter_entries;
      ++summary.synthesized_accessor_entries;
      if (property_record.getter_storage_runtime_helper_symbol ==
          kObjc3RuntimeReadCurrentPropertyI32Symbol) {
        ++summary.current_property_read_entries;
      } else if (property_record.getter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol) {
        ++summary.weak_current_property_load_entries;
      }
    }
    if (!property_record.setter_storage_runtime_helper_symbol.empty()) {
      ++summary.synthesized_setter_entries;
      ++summary.synthesized_accessor_entries;
      if (property_record.setter_storage_runtime_helper_symbol ==
          kObjc3RuntimeWriteCurrentPropertyI32Symbol) {
        ++summary.current_property_write_entries;
      } else if (property_record.setter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeExchangeCurrentPropertyI32Symbol) {
        ++summary.current_property_exchange_entries;
      } else if (property_record.setter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol) {
        ++summary.weak_current_property_store_entries;
      }
    }
  }

  std::unordered_set<std::string> ivar_layout_owner_keys;
  for (const auto &ivar_record : records.ivars_lexicographic) {
    if (ivar_record.executable_ivar_layout_symbol.empty()) {
      continue;
    }
    ++summary.ivar_layout_entries;
    ivar_layout_owner_keys.insert(ivar_record.owner_kind + "|" +
                                  ivar_record.owner_name);
  }
  summary.ivar_layout_owner_entries = ivar_layout_owner_keys.size();

  const bool helpers_complete =
      summary.synthesized_accessor_entries ==
      summary.synthesized_getter_entries + summary.synthesized_setter_entries;
  const bool layouts_complete =
      summary.ivar_layout_entries == summary.local_ivar_descriptor_count;
  summary.runtime_import_artifact_ready =
      summary.deterministic && !summary.contract_id.empty() &&
      !summary.source_contract_id.empty() &&
      !summary.dispatch_and_synthesized_accessor_lowering_surface_contract_id
           .empty() &&
      !summary.executable_property_accessor_layout_lowering_contract_id.empty() &&
      !summary.executable_ivar_layout_emission_contract_id.empty() &&
      !summary
           .executable_synthesized_accessor_property_lowering_contract_id
           .empty() &&
      !summary.surface_path.empty() &&
      !summary.import_artifact_member_name.empty() &&
      !summary.source_model.empty() && !summary.preservation_model.empty() &&
      !summary.fail_closed_model.empty() && helpers_complete &&
      layouts_complete;
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready;

  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << "|properties=" << summary.local_property_descriptor_count
             << "|ivars=" << summary.local_ivar_descriptor_count
             << "|impl_owned=" << summary.implementation_owned_property_entries
             << "|owners=" << summary.synthesized_accessor_owner_entries
             << "|accessors=" << summary.synthesized_accessor_entries
             << "|reads=" << summary.current_property_read_entries
             << "|writes=" << summary.current_property_write_entries
             << "|exchanges=" << summary.current_property_exchange_entries
             << "|weak_loads=" << summary.weak_current_property_load_entries
             << "|weak_stores=" << summary.weak_current_property_store_entries
             << "|layouts=" << summary.ivar_layout_entries
             << "|layout_owners=" << summary.ivar_layout_owner_entries;
  summary.replay_key = replay_key.str();
  return summary;
}

struct Objc3RuntimeBlockOwnershipArtifactPreservationSummary {
  std::string contract_id =
      kObjc3RuntimeBlockOwnershipArtifactPreservationContractId;
  std::string source_contract_id =
      kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId;
  std::string block_object_invoke_thunk_lowering_contract_id =
      Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId;
  std::string block_byref_helper_lowering_contract_id =
      Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId;
  std::string block_escape_runtime_hook_lowering_contract_id =
      Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId;
  std::string runtime_support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string surface_path =
      kObjc3RuntimeBlockOwnershipArtifactPreservationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3RuntimeBlockOwnershipArtifactPreservationImportArtifactMemberName;
  std::string source_model =
      kObjc3RuntimeBlockOwnershipArtifactPreservationSourceModel;
  std::string preservation_model =
      kObjc3RuntimeBlockOwnershipArtifactPreservationModel;
  std::string fail_closed_model =
      kObjc3RuntimeBlockOwnershipArtifactPreservationFailClosedModel;
  std::size_t local_block_literal_sites = 0;
  std::size_t local_invoke_trampoline_symbolized_sites = 0;
  std::size_t local_copy_helper_required_sites = 0;
  std::size_t local_dispose_helper_required_sites = 0;
  std::size_t local_copy_helper_symbolized_sites = 0;
  std::size_t local_dispose_helper_symbolized_sites = 0;
  std::size_t local_escape_to_heap_sites = 0;
  std::size_t local_byref_layout_symbolized_sites = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool runtime_support_library_link_wiring_ready = false;
  bool deterministic = false;
  std::string replay_key;
};

inline Objc3RuntimeBlockOwnershipArtifactPreservationSummary
BuildObjc3RuntimeBlockOwnershipArtifactPreservationSummary(
    const Objc3BlockAbiInvokeTrampolineLoweringContract
        &block_abi_invoke_trampoline_lowering_contract,
    const Objc3BlockStorageEscapeLoweringContract
        &block_storage_escape_lowering_contract,
    const Objc3BlockCopyDisposeLoweringContract
        &block_copy_dispose_lowering_contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeBlockOwnershipArtifactPreservationSummary summary;
  summary.local_block_literal_sites =
      block_abi_invoke_trampoline_lowering_contract.block_literal_sites;
  summary.local_invoke_trampoline_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract
          .invoke_trampoline_symbolized_sites;
  summary.local_copy_helper_required_sites =
      block_copy_dispose_lowering_contract.copy_helper_required_sites;
  summary.local_dispose_helper_required_sites =
      block_copy_dispose_lowering_contract.dispose_helper_required_sites;
  summary.local_copy_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.copy_helper_symbolized_sites;
  summary.local_dispose_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites;
  summary.local_escape_to_heap_sites =
      block_storage_escape_lowering_contract.escape_to_heap_sites;
  summary.local_byref_layout_symbolized_sites =
      block_storage_escape_lowering_contract.byref_layout_symbolized_sites;
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.deterministic =
      block_abi_invoke_trampoline_lowering_contract.deterministic &&
      block_storage_escape_lowering_contract.deterministic &&
      block_copy_dispose_lowering_contract.deterministic;

  const bool invoke_sites_complete =
      summary.local_invoke_trampoline_symbolized_sites <=
      summary.local_block_literal_sites;
  const bool helper_sites_complete =
      summary.local_copy_helper_symbolized_sites <=
          summary.local_copy_helper_required_sites &&
      summary.local_dispose_helper_symbolized_sites <=
          summary.local_dispose_helper_required_sites;
  const bool byref_sites_complete =
      summary.local_byref_layout_symbolized_sites <=
      block_storage_escape_lowering_contract.byref_slot_count_total;
  const bool escape_sites_complete =
      summary.local_escape_to_heap_sites <=
      block_storage_escape_lowering_contract.block_literal_sites;
  summary.runtime_import_artifact_ready =
      summary.deterministic && !summary.contract_id.empty() &&
      !summary.source_contract_id.empty() &&
      !summary.block_object_invoke_thunk_lowering_contract_id.empty() &&
      !summary.block_byref_helper_lowering_contract_id.empty() &&
      !summary.block_escape_runtime_hook_lowering_contract_id.empty() &&
      !summary.runtime_support_library_link_wiring_contract_id.empty() &&
      !summary.surface_path.empty() &&
      !summary.import_artifact_member_name.empty() &&
      !summary.source_model.empty() && !summary.preservation_model.empty() &&
      !summary.fail_closed_model.empty() &&
      summary.runtime_support_library_link_wiring_ready &&
      invoke_sites_complete && helper_sites_complete && byref_sites_complete &&
      escape_sites_complete;
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready;

  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << "|blocks=" << summary.local_block_literal_sites
             << "|invoke=" << summary.local_invoke_trampoline_symbolized_sites
             << "|copy_required=" << summary.local_copy_helper_required_sites
             << "|dispose_required="
             << summary.local_dispose_helper_required_sites
             << "|copy_symbolized="
             << summary.local_copy_helper_symbolized_sites
             << "|dispose_symbolized="
             << summary.local_dispose_helper_symbolized_sites
             << "|escape=" << summary.local_escape_to_heap_sites
             << "|byref_layout="
             << summary.local_byref_layout_symbolized_sites;
  summary.replay_key = replay_key.str();
  return summary;
}
