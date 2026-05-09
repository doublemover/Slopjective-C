#include "io/objc3_cross_module_runtime_link_plan_sections.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_set>

#include "io/objc3_cross_module_runtime_link_plan_ordering.h"

namespace {

struct OrderedLinkInput {
  std::uint64_t registration_order_ordinal = 0;
  std::string translation_unit_identity_key;
  std::string module_name;
  std::string object_artifact_path;
  std::vector<std::string> driver_linker_flags;
};

bool TryValidateImportedLinkPlanInput(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    std::unordered_set<std::string> &seen_translation_unit_identity_keys,
    std::unordered_set<std::uint64_t> &seen_registration_ordinals,
    std::unordered_set<std::string> &seen_error_handling_replay_keys,
    std::unordered_set<std::string> &seen_concurrency_actor_replay_keys,
    std::unordered_set<std::string> &seen_interop_ffi_replay_keys,
    std::unordered_set<std::string>
        &seen_interop_header_module_bridge_replay_keys,
    std::unordered_set<std::string> &seen_metaprogramming_host_cache_replay_keys,
    std::string &error) {
  if (imported_input.module_name.empty() ||
      imported_input.import_surface_artifact_path.empty() ||
      imported_input.registration_manifest_artifact_path.empty() ||
      imported_input.object_artifact_path.empty() ||
      imported_input.discovery_artifact_path.empty() ||
      imported_input.linker_response_artifact_path.empty() ||
      imported_input.translation_unit_identity_model.empty() ||
      imported_input.translation_unit_identity_key.empty() ||
      imported_input.object_format.empty() ||
      imported_input.runtime_support_library_archive_relative_path.empty() ||
      imported_input.translation_unit_registration_order_ordinal == 0 ||
      imported_input.total_descriptor_count !=
          imported_input.class_descriptor_count +
              imported_input.protocol_descriptor_count +
              imported_input.category_descriptor_count +
              imported_input.property_descriptor_count +
              imported_input.ivar_descriptor_count ||
      imported_input.driver_linker_flags.empty() ||
      imported_input.bootstrap_live_registration_contract_id.empty() ||
      imported_input.bootstrap_live_restart_hardening_contract_id.empty() ||
      imported_input.bootstrap_live_replay_registered_images_symbol.empty() ||
      imported_input.bootstrap_live_reset_replay_state_snapshot_symbol.empty() ||
      imported_input.bootstrap_live_restart_reset_for_testing_symbol.empty() ||
      imported_input.bootstrap_live_restart_replay_registered_images_symbol
          .empty() ||
      imported_input
          .bootstrap_live_restart_reset_replay_state_snapshot_symbol.empty()) {
    error = "cross-module runtime link-plan imported input is incomplete for " +
            imported_input.module_name;
    return false;
  }
  if (imported_input.object_format != inputs.object_format) {
    error = "cross-module runtime link-plan object-format mismatch for " +
            imported_input.module_name;
    return false;
  }
  // runtime-support archive anchors remain intentionally centralized here:
  // imported modules must agree on the package path before later document
  // sections can claim cross-module preservation for these runtime surfaces.
  if (imported_input.runtime_support_library_archive_relative_path !=
      inputs.runtime_support_library_archive_relative_path) {
    error = "cross-module runtime link-plan runtime library path mismatch for " +
            imported_input.module_name;
    return false;
  }
  if (!imported_input.ready_for_live_registration_discovery_replay ||
      imported_input.bootstrap_live_registration_contract_id !=
          inputs.expected_bootstrap_live_registration_contract_id ||
      imported_input.bootstrap_live_replay_registered_images_symbol !=
          inputs.expected_bootstrap_replay_registered_images_symbol ||
      imported_input.bootstrap_live_reset_replay_state_snapshot_symbol !=
          inputs.expected_bootstrap_reset_replay_state_snapshot_symbol) {
    error =
        "cross-module runtime link-plan live registration replay preservation mismatch for " +
        imported_input.module_name;
    return false;
  }
  if (!imported_input.ready_for_live_restart_hardening ||
      imported_input.bootstrap_live_restart_hardening_contract_id !=
          inputs.expected_bootstrap_live_restart_hardening_contract_id ||
      imported_input.bootstrap_live_restart_reset_for_testing_symbol !=
          inputs.expected_bootstrap_reset_for_testing_symbol ||
      imported_input.bootstrap_live_restart_replay_registered_images_symbol !=
          inputs.expected_bootstrap_replay_registered_images_symbol ||
      imported_input.bootstrap_live_restart_reset_replay_state_snapshot_symbol !=
          inputs.expected_bootstrap_reset_replay_state_snapshot_symbol) {
    error =
        "cross-module runtime link-plan live restart hardening preservation mismatch for " +
        imported_input.module_name;
    return false;
  }
  for (const auto &flag : imported_input.driver_linker_flags) {
    if (flag.empty()) {
      error =
          "cross-module runtime link-plan imported linker flag is empty for " +
          imported_input.module_name;
      return false;
    }
  }
  if (!seen_translation_unit_identity_keys
           .insert(imported_input.translation_unit_identity_key)
           .second) {
    error =
        "cross-module runtime link-plan duplicate translation-unit identity key: " +
        imported_input.translation_unit_identity_key;
    return false;
  }
  if (!seen_registration_ordinals
           .insert(imported_input.translation_unit_registration_order_ordinal)
           .second) {
    error =
        "cross-module runtime link-plan duplicate registration order ordinal: " +
        std::to_string(
            imported_input.translation_unit_registration_order_ordinal);
    return false;
  }
  if (imported_input.error_handling_result_and_bridging_artifact_replay_present) {
    if (imported_input.error_handling_contract_id !=
        inputs.expected_error_handling_contract_id) {
      error = "cross-module runtime link-plan Part 6 contract mismatch for " +
              imported_input.module_name;
      return false;
    }
    if (imported_input.error_handling_source_contract_id !=
        inputs.expected_error_handling_source_contract_id) {
      error =
          "cross-module runtime link-plan Part 6 source contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.error_handling_binary_artifact_replay_ready ||
        !imported_input.error_handling_runtime_import_artifact_ready ||
        !imported_input.error_handling_separate_compilation_replay_ready ||
        imported_input.error_handling_result_and_bridging_artifact_replay_key
            .empty() ||
        imported_input.error_handling_error_handling_replay_key.empty() ||
        imported_input.error_handling_throws_replay_key.empty() ||
        imported_input.error_handling_result_like_replay_key.empty() ||
        imported_input.error_handling_ns_error_replay_key.empty() ||
        imported_input.error_handling_unwind_replay_key.empty()) {
      error =
          "cross-module runtime link-plan Part 6 replay surface incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (!seen_error_handling_replay_keys
             .insert(imported_input.error_handling_error_handling_replay_key)
             .second) {
      error =
          "cross-module runtime link-plan duplicate imported Part 6 replay key: " +
          imported_input.error_handling_error_handling_replay_key;
      return false;
    }
  }
  if (imported_input.concurrency_actor_mailbox_runtime_import_present) {
    if (imported_input.concurrency_actor_contract_id !=
        inputs.expected_concurrency_actor_contract_id) {
      error =
          "cross-module runtime link-plan Part 7 actor contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.concurrency_actor_source_contract_id !=
        inputs.expected_concurrency_actor_source_contract_id) {
      error =
          "cross-module runtime link-plan Part 7 actor source contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.concurrency_actor_mailbox_runtime_ready ||
        !imported_input.concurrency_actor_mailbox_runtime_deterministic ||
        imported_input.concurrency_actor_mailbox_runtime_replay_key.empty() ||
        imported_input.concurrency_actor_lowering_replay_key.empty() ||
        imported_input.concurrency_actor_isolation_lowering_replay_key.empty()) {
      error =
          "cross-module runtime link-plan Part 7 actor replay surface incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (!seen_concurrency_actor_replay_keys
             .insert(imported_input.concurrency_actor_mailbox_runtime_replay_key)
             .second) {
      error =
          "cross-module runtime link-plan duplicate imported Part 7 actor replay key: " +
          imported_input.concurrency_actor_mailbox_runtime_replay_key;
      return false;
    }
  }
  if (imported_input.interop_ffi_metadata_interface_preservation_present) {
    if (imported_input.interop_ffi_contract_id !=
        inputs.expected_interop_ffi_contract_id) {
      error = "cross-module runtime link-plan Part 11 ffi contract mismatch for " +
              imported_input.module_name;
      return false;
    }
    if (imported_input.interop_ffi_source_contract_id !=
        inputs.expected_interop_ffi_source_contract_id) {
      error =
          "cross-module runtime link-plan Part 11 ffi source contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.interop_ffi_preservation_contract_id !=
        inputs.expected_interop_ffi_preservation_contract_id) {
      error =
          "cross-module runtime link-plan Part 11 ffi preservation contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.interop_ffi_runtime_import_artifact_ready ||
        !imported_input.interop_ffi_separate_compilation_preservation_ready ||
        !imported_input.interop_ffi_deterministic ||
        imported_input.interop_ffi_replay_key.empty() ||
        imported_input.interop_ffi_lowering_replay_key.empty() ||
        imported_input.interop_ffi_preservation_replay_key.empty()) {
      error =
          "cross-module runtime link-plan Part 11 ffi preservation surface incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (!seen_interop_ffi_replay_keys
             .insert(imported_input.interop_ffi_replay_key)
             .second) {
      error =
          "cross-module runtime link-plan duplicate imported Part 11 ffi replay key: " +
          imported_input.interop_ffi_replay_key;
      return false;
    }
  }
  if (imported_input.interop_header_module_bridge_generation_present) {
    if (imported_input.interop_header_module_bridge_contract_id !=
        inputs.expected_interop_header_module_bridge_contract_id) {
      error =
          "cross-module runtime link-plan Part 11 bridge-generation contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.interop_header_module_bridge_source_contract_id !=
        inputs.expected_interop_header_module_bridge_source_contract_id) {
      error =
          "cross-module runtime link-plan Part 11 bridge-generation source contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.interop_header_module_bridge_preservation_contract_id !=
        inputs.expected_interop_header_module_bridge_preservation_contract_id) {
      error =
          "cross-module runtime link-plan Part 11 bridge-generation preservation contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.interop_header_module_bridge_runtime_generation_ready ||
        !imported_input
             .interop_header_module_bridge_cross_module_packaging_ready ||
        !imported_input.interop_header_module_bridge_deterministic ||
        imported_input.interop_header_module_bridge_replay_key.empty() ||
        imported_input.interop_header_module_bridge_preservation_replay_key
            .empty() ||
        imported_input.interop_bridge_header_artifact_relative_path.empty() ||
        imported_input.interop_bridge_module_artifact_relative_path.empty() ||
        imported_input.interop_bridge_artifact_relative_path.empty()) {
      error =
          "cross-module runtime link-plan Part 11 bridge-generation surface incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.interop_bridge_header_artifact_relative_path !=
            inputs.expected_interop_bridge_header_artifact_relative_path ||
        imported_input.interop_bridge_module_artifact_relative_path !=
            inputs.expected_interop_bridge_module_artifact_relative_path ||
        imported_input.interop_bridge_artifact_relative_path !=
            inputs.expected_interop_bridge_artifact_relative_path) {
      error =
          "cross-module runtime link-plan Part 11 bridge-generation artifact path mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!seen_interop_header_module_bridge_replay_keys
             .insert(imported_input.interop_header_module_bridge_replay_key)
             .second) {
      error =
          "cross-module runtime link-plan duplicate imported Part 11 bridge-generation replay key: " +
          imported_input.interop_header_module_bridge_replay_key;
      return false;
    }
  }
  if (imported_input
          .metaprogramming_macro_host_process_cache_runtime_integration_present) {
    if (imported_input.metaprogramming_macro_host_process_cache_contract_id !=
        inputs.expected_metaprogramming_host_cache_contract_id) {
      error =
          "cross-module runtime link-plan Part 10 macro host cache contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input
            .metaprogramming_macro_host_process_cache_source_contract_id !=
        inputs.expected_metaprogramming_host_cache_source_contract_id) {
      error =
          "cross-module runtime link-plan Part 10 macro host cache source contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.metaprogramming_macro_host_process_cache_runtime_ready ||
        !imported_input
             .metaprogramming_macro_host_process_cache_separate_compilation_ready ||
        !imported_input.metaprogramming_macro_host_process_cache_deterministic ||
        imported_input.metaprogramming_macro_host_process_cache_replay_key
            .empty() ||
        imported_input
            .metaprogramming_macro_host_process_cache_host_executable_relative_path
            .empty() ||
        imported_input.metaprogramming_macro_host_process_cache_root_relative_path
            .empty()) {
      error =
          "cross-module runtime link-plan Part 10 macro host cache surface incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input
            .metaprogramming_macro_host_process_cache_host_executable_relative_path !=
        inputs.expected_metaprogramming_host_cache_executable_relative_path) {
      error =
          "cross-module runtime link-plan Part 10 macro host cache executable path mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.metaprogramming_macro_host_process_cache_root_relative_path !=
        inputs.expected_metaprogramming_host_cache_root_relative_path) {
      error =
          "cross-module runtime link-plan Part 10 macro host cache root path mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!seen_metaprogramming_host_cache_replay_keys
             .insert(imported_input
                         .metaprogramming_macro_host_process_cache_replay_key)
             .second) {
      error =
          "cross-module runtime link-plan duplicate imported Part 10 macro host cache replay key: " +
          imported_input
              .metaprogramming_macro_host_process_cache_replay_key;
      return false;
    }
  }
  if (!imported_input.block_ownership_artifact_preservation_present ||
      imported_input.block_ownership_contract_id !=
          inputs.expected_block_ownership_contract_id ||
      imported_input.block_ownership_source_contract_id !=
          inputs.expected_block_ownership_source_contract_id ||
      imported_input.block_ownership_object_invoke_thunk_lowering_contract_id !=
          inputs.expected_block_ownership_object_invoke_thunk_lowering_contract_id ||
      imported_input.block_ownership_byref_helper_lowering_contract_id !=
          inputs.expected_block_ownership_byref_helper_lowering_contract_id ||
      imported_input.block_ownership_escape_runtime_hook_lowering_contract_id !=
          inputs.expected_block_ownership_escape_runtime_hook_lowering_contract_id ||
      imported_input
              .block_ownership_runtime_support_library_link_wiring_contract_id !=
          inputs
              .expected_block_ownership_runtime_support_library_link_wiring_contract_id) {
    error =
        "cross-module runtime link-plan block-ownership preservation contract mismatch for " +
        imported_input.module_name;
    return false;
  }
  if (!imported_input.block_ownership_runtime_import_artifact_ready ||
      !imported_input.block_ownership_separate_compilation_preservation_ready ||
      !imported_input.block_ownership_runtime_support_library_link_wiring_ready ||
      !imported_input.block_ownership_deterministic ||
      imported_input.block_ownership_replay_key.empty() ||
      imported_input.block_ownership_local_invoke_trampoline_symbolized_sites >
          imported_input.block_ownership_local_block_literal_sites ||
      imported_input.block_ownership_local_copy_helper_symbolized_sites >
          imported_input.block_ownership_local_copy_helper_required_sites ||
      imported_input.block_ownership_local_dispose_helper_symbolized_sites >
          imported_input.block_ownership_local_dispose_helper_required_sites ||
      imported_input.block_ownership_local_escape_to_heap_sites >
          imported_input.block_ownership_local_block_literal_sites) {
    error =
        "cross-module runtime link-plan block-ownership preservation surface incomplete for " +
        imported_input.module_name;
    return false;
  }
  if (!imported_input.storage_reflection_artifact_preservation_present ||
      imported_input.storage_reflection_contract_id !=
          inputs.expected_storage_reflection_contract_id ||
      imported_input.storage_reflection_source_contract_id !=
          inputs.expected_storage_reflection_source_contract_id ||
      imported_input
              .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id !=
          inputs
              .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id ||
      imported_input
              .storage_reflection_executable_property_accessor_layout_lowering_contract_id !=
          inputs
              .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id ||
      imported_input
              .storage_reflection_executable_ivar_layout_emission_contract_id !=
          inputs
              .expected_storage_reflection_executable_ivar_layout_emission_contract_id ||
      imported_input
              .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id !=
          inputs
              .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id) {
    error =
        "cross-module runtime link-plan storage/reflection preservation contract mismatch for " +
        imported_input.module_name;
    return false;
  }
  if (!imported_input.storage_reflection_runtime_import_artifact_ready ||
      !imported_input.storage_reflection_separate_compilation_preservation_ready ||
      !imported_input.storage_reflection_deterministic ||
      imported_input.storage_reflection_replay_key.empty() ||
      imported_input.storage_reflection_local_property_descriptor_count !=
          imported_input.property_descriptor_count ||
      imported_input.storage_reflection_local_ivar_descriptor_count !=
          imported_input.ivar_descriptor_count ||
      imported_input.storage_reflection_synthesized_accessor_entries !=
          imported_input.storage_reflection_synthesized_getter_entries +
              imported_input.storage_reflection_synthesized_setter_entries ||
      imported_input.storage_reflection_ivar_layout_entries !=
          imported_input.ivar_descriptor_count) {
    error =
        "cross-module runtime link-plan storage/reflection preservation surface incomplete for " +
        imported_input.module_name;
    return false;
  }
  return true;
}

void AddImportedLinkPlanInputToSections(
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    Objc3CrossModuleRuntimeLinkPlanSections &sections,
    std::vector<OrderedLinkInput> &ordered_link_inputs) {
  ordered_link_inputs.push_back(
      {imported_input.translation_unit_registration_order_ordinal,
       imported_input.translation_unit_identity_key,
       imported_input.module_name,
       imported_input.object_artifact_path,
       imported_input.driver_linker_flags});

  if (imported_input.error_handling_result_and_bridging_artifact_replay_present) {
    sections.imported_error_handling_module_names_lexicographic.push_back(
        imported_input.module_name);
  }
  if (imported_input.concurrency_actor_mailbox_runtime_import_present) {
    sections.imported_concurrency_actor_module_names_lexicographic.push_back(
        imported_input.module_name);
  }
  if (imported_input.interop_ffi_metadata_interface_preservation_present) {
    sections.imported_interop_ffi_module_names_lexicographic.push_back(
        imported_input.module_name);
  }
  if (imported_input.interop_header_module_bridge_generation_present) {
    sections
        .imported_interop_header_module_bridge_module_names_lexicographic
        .push_back(imported_input.module_name);
  }
  if (imported_input
          .metaprogramming_macro_host_process_cache_runtime_integration_present) {
    sections
        .imported_metaprogramming_host_cache_module_names_lexicographic
        .push_back(imported_input.module_name);
  }

  sections.imported_class_descriptor_count +=
      imported_input.class_descriptor_count;
  sections.imported_protocol_descriptor_count +=
      imported_input.protocol_descriptor_count;
  sections.imported_category_descriptor_count +=
      imported_input.category_descriptor_count;
  sections.imported_property_descriptor_count +=
      imported_input.property_descriptor_count;
  sections.imported_ivar_descriptor_count += imported_input.ivar_descriptor_count;
  sections.imported_total_descriptor_count += imported_input.total_descriptor_count;

  sections.imported_block_ownership_block_literal_sites +=
      imported_input.block_ownership_local_block_literal_sites;
  sections.imported_block_ownership_invoke_trampoline_symbolized_sites +=
      imported_input.block_ownership_local_invoke_trampoline_symbolized_sites;
  sections.imported_block_ownership_copy_helper_required_sites +=
      imported_input.block_ownership_local_copy_helper_required_sites;
  sections.imported_block_ownership_dispose_helper_required_sites +=
      imported_input.block_ownership_local_dispose_helper_required_sites;
  sections.imported_block_ownership_copy_helper_symbolized_sites +=
      imported_input.block_ownership_local_copy_helper_symbolized_sites;
  sections.imported_block_ownership_dispose_helper_symbolized_sites +=
      imported_input.block_ownership_local_dispose_helper_symbolized_sites;
  sections.imported_block_ownership_escape_to_heap_sites +=
      imported_input.block_ownership_local_escape_to_heap_sites;
  sections.imported_block_ownership_byref_layout_symbolized_sites +=
      imported_input.block_ownership_local_byref_layout_symbolized_sites;

  sections.imported_storage_reflection_implementation_owned_property_entries +=
      imported_input.storage_reflection_implementation_owned_property_entries;
  sections.imported_storage_reflection_synthesized_accessor_owner_entries +=
      imported_input.storage_reflection_synthesized_accessor_owner_entries;
  sections.imported_storage_reflection_synthesized_getter_entries +=
      imported_input.storage_reflection_synthesized_getter_entries;
  sections.imported_storage_reflection_synthesized_setter_entries +=
      imported_input.storage_reflection_synthesized_setter_entries;
  sections.imported_storage_reflection_synthesized_accessor_entries +=
      imported_input.storage_reflection_synthesized_accessor_entries;
  sections.imported_storage_reflection_current_property_read_entries +=
      imported_input.storage_reflection_current_property_read_entries;
  sections.imported_storage_reflection_current_property_write_entries +=
      imported_input.storage_reflection_current_property_write_entries;
  sections.imported_storage_reflection_current_property_exchange_entries +=
      imported_input.storage_reflection_current_property_exchange_entries;
  sections.imported_storage_reflection_weak_current_property_load_entries +=
      imported_input.storage_reflection_weak_current_property_load_entries;
  sections.imported_storage_reflection_weak_current_property_store_entries +=
      imported_input.storage_reflection_weak_current_property_store_entries;
  sections.imported_storage_reflection_ivar_layout_entries +=
      imported_input.storage_reflection_ivar_layout_entries;
  sections.imported_storage_reflection_ivar_layout_owner_entries +=
      imported_input.storage_reflection_ivar_layout_owner_entries;
}

void SortLexicographicModuleSections(
    Objc3CrossModuleRuntimeLinkPlanSections &sections) {
  std::sort(sections.module_names_lexicographic.begin(),
            sections.module_names_lexicographic.end());
  std::sort(sections.imported_error_handling_module_names_lexicographic.begin(),
            sections.imported_error_handling_module_names_lexicographic.end());
  std::sort(
      sections.imported_concurrency_actor_module_names_lexicographic.begin(),
      sections.imported_concurrency_actor_module_names_lexicographic.end());
  std::sort(sections.imported_interop_ffi_module_names_lexicographic.begin(),
            sections.imported_interop_ffi_module_names_lexicographic.end());
  std::sort(
      sections
          .imported_interop_header_module_bridge_module_names_lexicographic
          .begin(),
      sections.imported_interop_header_module_bridge_module_names_lexicographic
          .end());
  std::sort(
      sections.imported_metaprogramming_host_cache_module_names_lexicographic
          .begin(),
      sections.imported_metaprogramming_host_cache_module_names_lexicographic
          .end());
}

}  // namespace

bool BuildObjc3CrossModuleRuntimeLinkPlanSections(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
        &imported_inputs,
    Objc3CrossModuleRuntimeLinkPlanSections &sections,
    std::string &error) {
  sections = Objc3CrossModuleRuntimeLinkPlanSections{};

  std::unordered_set<std::string> seen_translation_unit_identity_keys;
  std::unordered_set<std::uint64_t> seen_registration_ordinals;
  std::unordered_set<std::string> seen_driver_linker_flags;
  std::unordered_set<std::string> seen_direct_import_surface_paths;
  std::unordered_set<std::string> seen_error_handling_replay_keys;
  std::unordered_set<std::string> seen_concurrency_actor_replay_keys;
  std::unordered_set<std::string> seen_interop_ffi_replay_keys;
  std::unordered_set<std::string> seen_interop_header_module_bridge_replay_keys;
  std::unordered_set<std::string> seen_metaprogramming_host_cache_replay_keys;

  sections.direct_import_surface_artifact_paths =
      BuildOrderedObjc3CrossModuleDirectImportSurfaceArtifactPaths(
          inputs.direct_import_surface_artifact_paths);

  seen_translation_unit_identity_keys.insert(
      inputs.local_translation_unit_identity_key);
  seen_registration_ordinals.insert(
      inputs.local_translation_unit_registration_order_ordinal);
  for (const auto &surface_path :
       sections.direct_import_surface_artifact_paths) {
    if (surface_path.empty()) {
      error =
          "cross-module runtime link-plan direct import surface path missing";
      return false;
    }
    if (!seen_direct_import_surface_paths.insert(surface_path).second) {
      error =
          "cross-module runtime link-plan duplicate direct import surface "
          "path: " +
          surface_path;
      return false;
    }
  }
  for (const auto &local_flag : inputs.local_driver_linker_flags) {
    if (local_flag.empty()) {
      error =
          "cross-module runtime link-plan local driver-linker flag input missing";
      return false;
    }
  }

  std::vector<OrderedLinkInput> ordered_link_inputs;
  ordered_link_inputs.reserve(imported_inputs.size() + 1u);

  sections.module_names_lexicographic.reserve(imported_inputs.size() + 1u);
  sections.module_names_lexicographic.push_back(inputs.local_module_name);

  for (const auto &imported_input : imported_inputs) {
    if (!TryValidateImportedLinkPlanInput(
            inputs,
            imported_input,
            seen_translation_unit_identity_keys,
            seen_registration_ordinals,
            seen_error_handling_replay_keys,
            seen_concurrency_actor_replay_keys,
            seen_interop_ffi_replay_keys,
            seen_interop_header_module_bridge_replay_keys,
            seen_metaprogramming_host_cache_replay_keys,
            error)) {
      return false;
    }
    sections.module_names_lexicographic.push_back(imported_input.module_name);
    AddImportedLinkPlanInputToSections(imported_input,
                                       sections,
                                       ordered_link_inputs);
  }

  ordered_link_inputs.push_back(
      {inputs.local_translation_unit_registration_order_ordinal,
       inputs.local_translation_unit_identity_key,
       inputs.local_module_name,
       inputs.local_object_artifact_relative_path,
       inputs.local_driver_linker_flags});
  std::sort(ordered_link_inputs.begin(),
            ordered_link_inputs.end(),
            [](const auto &lhs, const auto &rhs) {
              if (lhs.registration_order_ordinal !=
                  rhs.registration_order_ordinal) {
                return lhs.registration_order_ordinal <
                       rhs.registration_order_ordinal;
              }
              return lhs.translation_unit_identity_key <
                     rhs.translation_unit_identity_key;
            });
  for (const auto &ordered_input : ordered_link_inputs) {
    sections.ordered_link_object_artifacts.push_back(
        ordered_input.object_artifact_path);
    for (const auto &flag : ordered_input.driver_linker_flags) {
      if (seen_driver_linker_flags.insert(flag).second) {
        sections.merged_driver_linker_flags.push_back(flag);
      }
    }
  }
  if (sections.merged_driver_linker_flags.empty()) {
    error =
        "cross-module runtime link-plan merged driver-linker flag list is empty";
    return false;
  }

  SortLexicographicModuleSections(sections);
  return true;
}
