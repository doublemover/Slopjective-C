#include "io/objc3_process_internal.h"
#include "io/objc3_cross_module_imported_modules_document.h"
#include "io/objc3_cross_module_runtime_link_plan_inputs.h"
#include "io/objc3_cross_module_runtime_link_plan_ordering.h"

bool TryBuildObjc3CrossModuleRuntimeLinkPlanArtifact(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    std::string &plan_json,
    std::string &linker_response_payload,
    std::string &error) {
  plan_json.clear();
  linker_response_payload.clear();
  error.clear();

  if (!TryValidateObjc3CrossModuleRuntimeLinkPlanArtifactInputs(inputs,
                                                               error)) {
    return false;
  }

  const std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
      imported_inputs =
          BuildOrderedObjc3CrossModuleRuntimeLinkPlanImportedInputs(
              inputs.imported_inputs);

  std::unordered_set<std::string> seen_translation_unit_identity_keys;
  std::unordered_set<std::uint64_t> seen_registration_ordinals;
  std::unordered_set<std::string> seen_driver_linker_flags;
  std::unordered_set<std::string> seen_direct_import_surface_paths;
  std::unordered_set<std::string> seen_error_handling_replay_keys;
  std::unordered_set<std::string> seen_concurrency_actor_replay_keys;
  std::unordered_set<std::string> seen_interop_ffi_replay_keys;
  std::unordered_set<std::string> seen_interop_header_module_bridge_replay_keys;
  std::unordered_set<std::string> seen_metaprogramming_host_cache_replay_keys;
  std::vector<std::string> ordered_link_object_artifacts;
  std::vector<std::string> merged_driver_linker_flags;
  std::vector<std::string> imported_error_handling_module_names;
  std::vector<std::string> imported_concurrency_actor_module_names;
  std::vector<std::string> imported_interop_ffi_module_names;
  std::vector<std::string> imported_interop_header_module_bridge_module_names;
  std::vector<std::string> imported_metaprogramming_host_cache_module_names;
  std::size_t imported_class_descriptor_count = 0;
  std::size_t imported_protocol_descriptor_count = 0;
  std::size_t imported_category_descriptor_count = 0;
  std::size_t imported_property_descriptor_count = 0;
  std::size_t imported_ivar_descriptor_count = 0;
  std::size_t imported_total_descriptor_count = 0;
  std::size_t imported_block_ownership_block_literal_sites = 0;
  std::size_t imported_block_ownership_invoke_trampoline_symbolized_sites = 0;
  std::size_t imported_block_ownership_copy_helper_required_sites = 0;
  std::size_t imported_block_ownership_dispose_helper_required_sites = 0;
  std::size_t imported_block_ownership_copy_helper_symbolized_sites = 0;
  std::size_t imported_block_ownership_dispose_helper_symbolized_sites = 0;
  std::size_t imported_block_ownership_escape_to_heap_sites = 0;
  std::size_t imported_block_ownership_byref_layout_symbolized_sites = 0;
  std::size_t imported_storage_reflection_implementation_owned_property_entries =
      0;
  std::size_t imported_storage_reflection_synthesized_accessor_owner_entries =
      0;
  std::size_t imported_storage_reflection_synthesized_getter_entries = 0;
  std::size_t imported_storage_reflection_synthesized_setter_entries = 0;
  std::size_t imported_storage_reflection_synthesized_accessor_entries = 0;
  std::size_t imported_storage_reflection_current_property_read_entries = 0;
  std::size_t imported_storage_reflection_current_property_write_entries = 0;
  std::size_t imported_storage_reflection_current_property_exchange_entries = 0;
  std::size_t imported_storage_reflection_weak_current_property_load_entries =
      0;
  std::size_t imported_storage_reflection_weak_current_property_store_entries =
      0;
  std::size_t imported_storage_reflection_ivar_layout_entries = 0;
  std::size_t imported_storage_reflection_ivar_layout_owner_entries = 0;
  const std::vector<std::string> direct_import_surface_artifact_paths =
      BuildOrderedObjc3CrossModuleDirectImportSurfaceArtifactPaths(
          inputs.direct_import_surface_artifact_paths);

  seen_translation_unit_identity_keys.insert(
      inputs.local_translation_unit_identity_key);
  seen_registration_ordinals.insert(
      inputs.local_translation_unit_registration_order_ordinal);
  for (const auto &surface_path : direct_import_surface_artifact_paths) {
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

  struct OrderedLinkInput {
    std::uint64_t registration_order_ordinal = 0;
    std::string translation_unit_identity_key;
    std::string module_name;
    std::string object_artifact_path;
    std::vector<std::string> driver_linker_flags;
  };
  std::vector<OrderedLinkInput> ordered_link_inputs;
  ordered_link_inputs.reserve(imported_inputs.size() + 1u);

  for (const auto &imported_input : imported_inputs) {
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
        imported_input.bootstrap_live_reset_replay_state_snapshot_symbol
            .empty() ||
        imported_input.bootstrap_live_restart_reset_for_testing_symbol.empty() ||
        imported_input.bootstrap_live_restart_replay_registered_images_symbol
            .empty() ||
        imported_input
            .bootstrap_live_restart_reset_replay_state_snapshot_symbol.empty()) {
      error =
          "cross-module runtime link-plan imported input is incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.object_format != inputs.object_format) {
      error = "cross-module runtime link-plan object-format mismatch for " +
              imported_input.module_name;
      return false;
    }
    // error-runtime/bridge-helper anchor: Part 6 lane-D still rides
    // the same packaged runtime-support archive path as the rest of the native
    // runtime helper cluster, so imported modules must agree on that archive
    // before later cross-module Part 6 execution claims can become truthful.
    // continuation/runtime-helper anchor: the first private Part 7
    // continuation helper cluster likewise stays inside the same packaged
    // runtime-support archive path. Mixed-module async/runtime-helper claims
    // therefore may not diverge on the runtime archive even before live
    // suspension integration lands.
    // live continuation/runtime integration anchor: the supported
    // direct-call await slice now links and executes through that same archive,
    // so runnable Part 7 helper traffic still depends on this runtime archive
    // equality across mixed-module link plans.
    // actor-runtime/executor-binding anchor: the private actor
    // runtime helper cluster also rides that same packaged runtime archive, so
    // actor-state/executor-binding claims cannot diverge on archive identity
    // across mixed-module link plans.
    // actor-mailbox/isolation-runtime anchor: live mailbox helper
    // traffic still links through that same packaged runtime archive, so mixed
    // actor-runtime link plans must keep one archive identity even after the
    // mailbox helpers become runnable.
    // system-helper/runtime-contract anchor: Part 8 cleanup/resource
    // and retainable-family runtime proof also stays on that same packaged
    // runtime archive path. Mixed-module link plans may not diverge on archive
    // identity while lane-D still reuses the existing private helper cluster.
    // expansion-host/runtime-boundary anchor: Part 10 does not launch
    // a macro host from the driver yet; the packaged objc3_runtime.lib archive
    // remains the only host-facing runtime handoff while macro execution and
    // runtime package loading stay fail-closed.
    if (imported_input.runtime_support_library_archive_relative_path !=
        inputs.runtime_support_library_archive_relative_path) {
      error =
          "cross-module runtime link-plan runtime library path mismatch for " +
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
        imported_input
                .bootstrap_live_restart_reset_replay_state_snapshot_symbol !=
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
      if (imported_input.error_handling_contract_id != inputs.expected_error_handling_contract_id) {
        error =
            "cross-module runtime link-plan Part 6 contract mismatch for " +
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
          imported_input.error_handling_result_and_bridging_artifact_replay_key.empty() ||
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
      if (!seen_error_handling_replay_keys.insert(imported_input.error_handling_error_handling_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported Part 6 replay key: " +
            imported_input.error_handling_error_handling_replay_key;
        return false;
      }
      imported_error_handling_module_names.push_back(imported_input.module_name);
    }
    if (imported_input.concurrency_actor_mailbox_runtime_import_present) {
      // actor cross-module isolation-metadata hardening anchor:
      // imported actor-runtime surfaces must preserve one canonical private
      // mailbox/isolation replay contract and may not drift on contract ids or
      // replay keys across mixed-module link plans.
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
      imported_concurrency_actor_module_names.push_back(imported_input.module_name);
    }
    if (imported_input.interop_ffi_metadata_interface_preservation_present) {
      // bridge-packaging/toolchain anchor: imported Part 11 runtime-
      // import surfaces must preserve one canonical metadata/interface packet
      // across mixed-module link plans before D002 claims live header/module/
      // bridge generation from that packaging topology.
      if (imported_input.interop_ffi_contract_id !=
          inputs.expected_interop_ffi_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 ffi contract mismatch for " +
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
      imported_interop_ffi_module_names.push_back(imported_input.module_name);
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
      imported_interop_header_module_bridge_module_names.push_back(
          imported_input.module_name);
    }
    if (imported_input.metaprogramming_macro_host_process_cache_runtime_integration_present) {
      if (imported_input.metaprogramming_macro_host_process_cache_contract_id !=
          inputs.expected_metaprogramming_host_cache_contract_id) {
        error =
            "cross-module runtime link-plan metaprogramming host/cache contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.metaprogramming_macro_host_process_cache_source_contract_id !=
          inputs.expected_metaprogramming_host_cache_source_contract_id) {
        error =
            "cross-module runtime link-plan metaprogramming host/cache source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.metaprogramming_macro_host_process_cache_runtime_ready ||
          !imported_input.metaprogramming_macro_host_process_cache_separate_compilation_ready ||
          !imported_input.metaprogramming_macro_host_process_cache_deterministic ||
          imported_input.metaprogramming_macro_host_process_cache_replay_key.empty() ||
          imported_input.metaprogramming_macro_host_process_cache_host_executable_relative_path.empty() ||
          imported_input.metaprogramming_macro_host_process_cache_root_relative_path.empty()) {
        error =
            "cross-module runtime link-plan metaprogramming host/cache surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.metaprogramming_macro_host_process_cache_host_executable_relative_path !=
          inputs.expected_metaprogramming_host_cache_executable_relative_path) {
        error =
            "cross-module runtime link-plan metaprogramming host/cache executable path mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.metaprogramming_macro_host_process_cache_root_relative_path !=
          inputs.expected_metaprogramming_host_cache_root_relative_path) {
        error =
            "cross-module runtime link-plan metaprogramming host/cache root path mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_metaprogramming_host_cache_replay_keys
               .insert(imported_input.metaprogramming_macro_host_process_cache_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported metaprogramming host/cache replay key: " +
            imported_input.metaprogramming_macro_host_process_cache_replay_key;
        return false;
      }
      imported_metaprogramming_host_cache_module_names.push_back(imported_input.module_name);
    }
    if (!imported_input.block_ownership_artifact_preservation_present ||
        imported_input.block_ownership_contract_id !=
            inputs.expected_block_ownership_contract_id ||
        imported_input.block_ownership_source_contract_id !=
            inputs.expected_block_ownership_source_contract_id ||
        imported_input
                .block_ownership_object_invoke_thunk_lowering_contract_id !=
            inputs
                .expected_block_ownership_object_invoke_thunk_lowering_contract_id ||
        imported_input.block_ownership_byref_helper_lowering_contract_id !=
            inputs.expected_block_ownership_byref_helper_lowering_contract_id ||
        imported_input
                .block_ownership_escape_runtime_hook_lowering_contract_id !=
            inputs
                .expected_block_ownership_escape_runtime_hook_lowering_contract_id ||
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
        !imported_input
             .block_ownership_separate_compilation_preservation_ready ||
        !imported_input
             .block_ownership_runtime_support_library_link_wiring_ready ||
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
        !imported_input
             .storage_reflection_separate_compilation_preservation_ready ||
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
    ordered_link_inputs.push_back(
        {imported_input.translation_unit_registration_order_ordinal,
         imported_input.translation_unit_identity_key,
         imported_input.module_name,
         imported_input.object_artifact_path,
         imported_input.driver_linker_flags});
    imported_class_descriptor_count += imported_input.class_descriptor_count;
    imported_protocol_descriptor_count +=
        imported_input.protocol_descriptor_count;
    imported_category_descriptor_count +=
        imported_input.category_descriptor_count;
    imported_property_descriptor_count +=
        imported_input.property_descriptor_count;
    imported_ivar_descriptor_count += imported_input.ivar_descriptor_count;
    imported_total_descriptor_count += imported_input.total_descriptor_count;
    imported_block_ownership_block_literal_sites +=
        imported_input.block_ownership_local_block_literal_sites;
    imported_block_ownership_invoke_trampoline_symbolized_sites +=
        imported_input.block_ownership_local_invoke_trampoline_symbolized_sites;
    imported_block_ownership_copy_helper_required_sites +=
        imported_input.block_ownership_local_copy_helper_required_sites;
    imported_block_ownership_dispose_helper_required_sites +=
        imported_input.block_ownership_local_dispose_helper_required_sites;
    imported_block_ownership_copy_helper_symbolized_sites +=
        imported_input.block_ownership_local_copy_helper_symbolized_sites;
    imported_block_ownership_dispose_helper_symbolized_sites +=
        imported_input.block_ownership_local_dispose_helper_symbolized_sites;
    imported_block_ownership_escape_to_heap_sites +=
        imported_input.block_ownership_local_escape_to_heap_sites;
    imported_block_ownership_byref_layout_symbolized_sites +=
        imported_input.block_ownership_local_byref_layout_symbolized_sites;
    imported_storage_reflection_implementation_owned_property_entries +=
        imported_input.storage_reflection_implementation_owned_property_entries;
    imported_storage_reflection_synthesized_accessor_owner_entries +=
        imported_input.storage_reflection_synthesized_accessor_owner_entries;
    imported_storage_reflection_synthesized_getter_entries +=
        imported_input.storage_reflection_synthesized_getter_entries;
    imported_storage_reflection_synthesized_setter_entries +=
        imported_input.storage_reflection_synthesized_setter_entries;
    imported_storage_reflection_synthesized_accessor_entries +=
        imported_input.storage_reflection_synthesized_accessor_entries;
    imported_storage_reflection_current_property_read_entries +=
        imported_input.storage_reflection_current_property_read_entries;
    imported_storage_reflection_current_property_write_entries +=
        imported_input.storage_reflection_current_property_write_entries;
    imported_storage_reflection_current_property_exchange_entries +=
        imported_input.storage_reflection_current_property_exchange_entries;
    imported_storage_reflection_weak_current_property_load_entries +=
        imported_input.storage_reflection_weak_current_property_load_entries;
    imported_storage_reflection_weak_current_property_store_entries +=
        imported_input.storage_reflection_weak_current_property_store_entries;
    imported_storage_reflection_ivar_layout_entries +=
        imported_input.storage_reflection_ivar_layout_entries;
    imported_storage_reflection_ivar_layout_owner_entries +=
        imported_input.storage_reflection_ivar_layout_owner_entries;
  }

  ordered_link_inputs.push_back(
      {inputs.local_translation_unit_registration_order_ordinal,
       inputs.local_translation_unit_identity_key,
       inputs.local_module_name,
       inputs.local_object_artifact_relative_path,
       inputs.local_driver_linker_flags});
  std::sort(ordered_link_inputs.begin(), ordered_link_inputs.end(),
            [](const auto &lhs, const auto &rhs) {
              if (lhs.registration_order_ordinal != rhs.registration_order_ordinal) {
                return lhs.registration_order_ordinal <
                       rhs.registration_order_ordinal;
              }
              return lhs.translation_unit_identity_key <
                     rhs.translation_unit_identity_key;
            });
  for (const auto &ordered_input : ordered_link_inputs) {
    ordered_link_object_artifacts.push_back(ordered_input.object_artifact_path);
    for (const auto &flag : ordered_input.driver_linker_flags) {
      if (seen_driver_linker_flags.insert(flag).second) {
        merged_driver_linker_flags.push_back(flag);
      }
    }
  }
  if (merged_driver_linker_flags.empty()) {
    error =
        "cross-module runtime link-plan merged driver-linker flag list is empty";
    return false;
  }

  std::vector<std::string> module_names_lexicographic;
  module_names_lexicographic.reserve(imported_inputs.size() + 1u);
  module_names_lexicographic.push_back(inputs.local_module_name);
  for (const auto &imported_input : imported_inputs) {
    module_names_lexicographic.push_back(imported_input.module_name);
  }
  std::sort(module_names_lexicographic.begin(), module_names_lexicographic.end());

  const std::string imported_modules_json =
      BuildObjc3CrossModuleImportedModulesJson(imported_inputs);
  std::sort(imported_error_handling_module_names.begin(), imported_error_handling_module_names.end());
  std::sort(imported_concurrency_actor_module_names.begin(),
            imported_concurrency_actor_module_names.end());
  std::sort(imported_interop_ffi_module_names.begin(),
            imported_interop_ffi_module_names.end());
  std::sort(imported_interop_header_module_bridge_module_names.begin(),
            imported_interop_header_module_bridge_module_names.end());
  std::sort(imported_metaprogramming_host_cache_module_names.begin(),
            imported_metaprogramming_host_cache_module_names.end());

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(inputs.contract_id)
      << "\",\n"
      << "  \"source_orchestration_contract_id\": \""
      << EscapeJsonString(inputs.source_orchestration_contract_id) << "\",\n"
      << "  \"import_surface_contract_id\": \""
      << EscapeJsonString(inputs.import_surface_contract_id) << "\",\n"
      << "  \"registration_manifest_contract_id\": \""
      << EscapeJsonString(inputs.registration_manifest_contract_id)
      << "\",\n"
      << "  \"payload_model\": \"" << EscapeJsonString(inputs.payload_model)
      << "\",\n"
      << "  \"artifact\": \"" << EscapeJsonString(inputs.artifact_relative_path)
      << "\",\n"
      << "  \"linker_response_artifact\": \""
      << EscapeJsonString(inputs.linker_response_artifact_relative_path)
      << "\",\n"
      << "  \"authority_model\": \""
      << EscapeJsonString(inputs.authority_model) << "\",\n"
      << "  \"packaging_model\": \""
      << EscapeJsonString(inputs.packaging_model) << "\",\n"
      << "  \"registration_scope_model\": \""
      << EscapeJsonString(inputs.registration_scope_model) << "\",\n"
      << "  \"link_object_order_model\": \""
      << EscapeJsonString(inputs.link_object_order_model) << "\",\n"
      << "  \"expected_error_handling_contract_id\": \""
      << EscapeJsonString(inputs.expected_error_handling_contract_id) << "\",\n"
      << "  \"expected_error_handling_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_error_handling_source_contract_id) << "\",\n"
      << "  \"expected_concurrency_actor_contract_id\": \""
      << EscapeJsonString(inputs.expected_concurrency_actor_contract_id) << "\",\n"
      << "  \"expected_concurrency_actor_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_concurrency_actor_source_contract_id)
      << "\",\n"
      << "  \"expected_interop_ffi_contract_id\": \""
      << EscapeJsonString(inputs.expected_interop_ffi_contract_id) << "\",\n"
      << "  \"expected_interop_ffi_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_interop_ffi_source_contract_id)
      << "\",\n"
      << "  \"expected_interop_ffi_preservation_contract_id\": \""
      << EscapeJsonString(inputs.expected_interop_ffi_preservation_contract_id)
      << "\",\n"
      << "  \"expected_interop_header_module_bridge_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_interop_header_module_bridge_contract_id)
      << "\",\n"
      << "  \"expected_interop_header_module_bridge_source_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_interop_header_module_bridge_source_contract_id)
      << "\",\n"
      << "  \"expected_interop_header_module_bridge_preservation_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_interop_header_module_bridge_preservation_contract_id)
      << "\",\n"
      << "  \"expected_interop_bridge_header_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_interop_bridge_header_artifact_relative_path)
      << "\",\n"
      << "  \"expected_interop_bridge_module_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_interop_bridge_module_artifact_relative_path)
      << "\",\n"
      << "  \"expected_interop_bridge_artifact_relative_path\": \""
      << EscapeJsonString(inputs.expected_interop_bridge_artifact_relative_path)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_contract_id\": \""
      << EscapeJsonString(inputs.expected_metaprogramming_host_cache_contract_id)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_metaprogramming_host_cache_source_contract_id)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_executable_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_metaprogramming_host_cache_executable_relative_path)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_root_relative_path\": \""
      << EscapeJsonString(inputs.expected_metaprogramming_host_cache_root_relative_path)
      << "\",\n"
      << "  \"expected_block_ownership_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_source_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_object_invoke_thunk_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_object_invoke_thunk_lowering_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_byref_helper_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_block_ownership_byref_helper_lowering_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_escape_runtime_hook_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_escape_runtime_hook_lowering_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_runtime_support_library_link_wiring_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_runtime_support_library_link_wiring_contract_id)
      << "\",\n"
      << "  \"module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(module_names_lexicographic, "    ")
      << ",\n"
      << "  \"module_image_count\": " << module_names_lexicographic.size()
      << ",\n"
      << "  \"direct_import_input_count\": "
      << direct_import_surface_artifact_paths.size() << ",\n"
      << "  \"error_handling_imported_module_count\": "
      << imported_error_handling_module_names.size() << ",\n"
      << "  \"concurrency_actor_imported_module_count\": "
      << imported_concurrency_actor_module_names.size() << ",\n"
      << "  \"interop_ffi_imported_module_count\": "
      << imported_interop_ffi_module_names.size() << ",\n"
      << "  \"interop_header_module_bridge_imported_module_count\": "
      << imported_interop_header_module_bridge_module_names.size() << ",\n"
      << "  \"metaprogramming_host_cache_imported_module_count\": "
      << imported_metaprogramming_host_cache_module_names.size() << ",\n"
      << "  \"direct_import_surface_artifact_paths\": "
      << BuildIndentedStringArrayJson(direct_import_surface_artifact_paths,
                                      "    ")
      << ",\n"
      << "  \"error_handling_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_error_handling_module_names, "    ")
      << ",\n"
      << "  \"concurrency_actor_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_concurrency_actor_module_names, "    ")
      << ",\n"
      << "  \"interop_ffi_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_interop_ffi_module_names, "    ")
      << ",\n"
      << "  \"interop_header_module_bridge_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             imported_interop_header_module_bridge_module_names, "    ")
      << ",\n"
      << "  \"metaprogramming_host_cache_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_metaprogramming_host_cache_module_names,
                                      "    ")
      << ",\n"
      << "  \"error_handling_cross_module_preservation_ready\": "
      << (!imported_error_handling_module_names.empty() ? "true" : "false") << ",\n"
      << "  \"concurrency_actor_cross_module_isolation_ready\": "
      << (!imported_concurrency_actor_module_names.empty() ? "true" : "false")
      << ",\n"
      << "  \"interop_ffi_cross_module_packaging_ready\": "
      << (!imported_interop_ffi_module_names.empty() ? "true" : "false")
      << ",\n"
      << "  \"interop_header_module_bridge_cross_module_packaging_ready\": "
      << (!imported_interop_header_module_bridge_module_names.empty() ? "true"
                                                                     : "false")
      << ",\n"
      << "  \"metaprogramming_host_cache_cross_module_preservation_ready\": "
      << (!imported_metaprogramming_host_cache_module_names.empty() ? "true" : "false")
      << ",\n"
      << "  \"bootstrap_live_registration_contract_id\": \""
      << EscapeJsonString(inputs.expected_bootstrap_live_registration_contract_id)
      << "\",\n"
      << "  \"bootstrap_live_restart_hardening_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_bootstrap_live_restart_hardening_contract_id)
      << "\",\n"
      << "  \"bootstrap_replay_registered_images_symbol\": \""
      << EscapeJsonString(inputs.expected_bootstrap_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.expected_bootstrap_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_for_testing_symbol\": \""
      << EscapeJsonString(inputs.expected_bootstrap_reset_for_testing_symbol)
      << "\",\n"
      << "  \"runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id\": \""
      << "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1"
      << "\",\n"
      << "  \"runtime_object_model_realization_source_surface_contract_id\": \""
      << "objc3c.runtime.object.model.realization.source.surface.v1"
      << "\",\n"
      << "  \"runtime_realization_lowering_reflection_artifact_surface_contract_id\": \""
      << "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1"
      << "\",\n"
      << "  \"runtime_dispatch_table_reflection_record_lowering_surface_contract_id\": \""
      << "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1"
      << "\",\n"
      << "  \"runtime_cross_module_block_ownership_artifact_preservation_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_contract_id)
      << "\",\n"
      << "  \"runtime_block_arc_lowering_helper_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_source_contract_id)
      << "\",\n"
      << "  \"block_object_invoke_thunk_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_object_invoke_thunk_lowering_contract_id)
      << "\",\n"
      << "  \"block_byref_helper_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_block_ownership_byref_helper_lowering_contract_id)
      << "\",\n"
      << "  \"block_escape_runtime_hook_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_escape_runtime_hook_lowering_contract_id)
      << "\",\n"
      << "  \"block_runtime_support_library_link_wiring_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_runtime_support_library_link_wiring_contract_id)
      << "\",\n"
      << "  \"runtime_cross_module_storage_reflection_artifact_preservation_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_storage_reflection_contract_id)
      << "\",\n"
      << "  \"runtime_property_ivar_storage_accessor_source_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_storage_reflection_source_contract_id)
      << "\",\n"
      << "  \"dispatch_and_synthesized_accessor_lowering_surface_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id)
      << "\",\n"
      << "  \"executable_property_accessor_layout_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id)
      << "\",\n"
      << "  \"executable_ivar_layout_emission_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_executable_ivar_layout_emission_contract_id)
      << "\",\n"
      << "  \"executable_synthesized_accessor_property_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id)
      << "\",\n"
      << "  \"realized_metadata_replay_preservation_model\": \""
      << "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests"
      << "\",\n"
      << "  \"block_ownership_artifact_preservation_model\": \""
      << "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-block-ownership-lowering-helper-and-runtime-link-facts-beyond-local-ir-object-emission"
      << "\",\n"
      << "  \"storage_reflection_artifact_preservation_model\": \""
      << "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission"
      << "\",\n"
      << "  \"imported_live_registration_replay_ready\": true,\n"
      << "  \"imported_live_restart_hardening_ready\": true,\n"
      << "  \"block_ownership_cross_module_preservation_ready\": true,\n"
      << "  \"storage_reflection_cross_module_preservation_ready\": true,\n"
      << "  \"module_image_count\": " << imported_inputs.size() + 1 << ",\n"
      << "  \"direct_import_input_count\": " << imported_inputs.size() << ",\n"
      << "  \"local_class_descriptor_count\": "
      << inputs.local_class_descriptor_count << ",\n"
      << "  \"local_protocol_descriptor_count\": "
      << inputs.local_protocol_descriptor_count << ",\n"
      << "  \"local_category_descriptor_count\": "
      << inputs.local_category_descriptor_count << ",\n"
      << "  \"local_property_descriptor_count\": "
      << inputs.local_property_descriptor_count << ",\n"
      << "  \"local_ivar_descriptor_count\": "
      << inputs.local_ivar_descriptor_count << ",\n"
      << "  \"local_total_descriptor_count\": "
      << inputs.local_total_descriptor_count << ",\n"
      << "  \"imported_class_descriptor_count\": "
      << imported_class_descriptor_count << ",\n"
      << "  \"imported_protocol_descriptor_count\": "
      << imported_protocol_descriptor_count << ",\n"
      << "  \"imported_category_descriptor_count\": "
      << imported_category_descriptor_count << ",\n"
      << "  \"imported_property_descriptor_count\": "
      << imported_property_descriptor_count << ",\n"
      << "  \"imported_ivar_descriptor_count\": "
      << imported_ivar_descriptor_count << ",\n"
      << "  \"imported_total_descriptor_count\": "
      << imported_total_descriptor_count << ",\n"
      << "  \"transitive_class_descriptor_count\": "
      << inputs.local_class_descriptor_count + imported_class_descriptor_count
      << ",\n"
      << "  \"transitive_protocol_descriptor_count\": "
      << inputs.local_protocol_descriptor_count +
             imported_protocol_descriptor_count
      << ",\n"
      << "  \"transitive_category_descriptor_count\": "
      << inputs.local_category_descriptor_count +
             imported_category_descriptor_count
      << ",\n"
      << "  \"transitive_property_descriptor_count\": "
      << inputs.local_property_descriptor_count +
             imported_property_descriptor_count
      << ",\n"
      << "  \"transitive_ivar_descriptor_count\": "
      << inputs.local_ivar_descriptor_count + imported_ivar_descriptor_count
      << ",\n"
      << "  \"transitive_total_descriptor_count\": "
      << inputs.local_total_descriptor_count + imported_total_descriptor_count
      << ",\n"
      << "  \"local_block_ownership_block_literal_sites\": "
      << inputs.local_block_ownership_block_literal_sites << ",\n"
      << "  \"local_block_ownership_invoke_trampoline_symbolized_sites\": "
      << inputs.local_block_ownership_invoke_trampoline_symbolized_sites
      << ",\n"
      << "  \"local_block_ownership_copy_helper_required_sites\": "
      << inputs.local_block_ownership_copy_helper_required_sites << ",\n"
      << "  \"local_block_ownership_dispose_helper_required_sites\": "
      << inputs.local_block_ownership_dispose_helper_required_sites << ",\n"
      << "  \"local_block_ownership_copy_helper_symbolized_sites\": "
      << inputs.local_block_ownership_copy_helper_symbolized_sites << ",\n"
      << "  \"local_block_ownership_dispose_helper_symbolized_sites\": "
      << inputs.local_block_ownership_dispose_helper_symbolized_sites
      << ",\n"
      << "  \"local_block_ownership_escape_to_heap_sites\": "
      << inputs.local_block_ownership_escape_to_heap_sites << ",\n"
      << "  \"local_block_ownership_byref_layout_symbolized_sites\": "
      << inputs.local_block_ownership_byref_layout_symbolized_sites
      << ",\n"
      << "  \"imported_block_ownership_block_literal_sites\": "
      << imported_block_ownership_block_literal_sites << ",\n"
      << "  \"imported_block_ownership_invoke_trampoline_symbolized_sites\": "
      << imported_block_ownership_invoke_trampoline_symbolized_sites
      << ",\n"
      << "  \"imported_block_ownership_copy_helper_required_sites\": "
      << imported_block_ownership_copy_helper_required_sites << ",\n"
      << "  \"imported_block_ownership_dispose_helper_required_sites\": "
      << imported_block_ownership_dispose_helper_required_sites << ",\n"
      << "  \"imported_block_ownership_copy_helper_symbolized_sites\": "
      << imported_block_ownership_copy_helper_symbolized_sites << ",\n"
      << "  \"imported_block_ownership_dispose_helper_symbolized_sites\": "
      << imported_block_ownership_dispose_helper_symbolized_sites << ",\n"
      << "  \"imported_block_ownership_escape_to_heap_sites\": "
      << imported_block_ownership_escape_to_heap_sites << ",\n"
      << "  \"imported_block_ownership_byref_layout_symbolized_sites\": "
      << imported_block_ownership_byref_layout_symbolized_sites << ",\n"
      << "  \"transitive_block_ownership_block_literal_sites\": "
      << inputs.local_block_ownership_block_literal_sites +
             imported_block_ownership_block_literal_sites
      << ",\n"
      << "  \"transitive_block_ownership_invoke_trampoline_symbolized_sites\": "
      << inputs.local_block_ownership_invoke_trampoline_symbolized_sites +
             imported_block_ownership_invoke_trampoline_symbolized_sites
      << ",\n"
      << "  \"transitive_block_ownership_copy_helper_required_sites\": "
      << inputs.local_block_ownership_copy_helper_required_sites +
             imported_block_ownership_copy_helper_required_sites
      << ",\n"
      << "  \"transitive_block_ownership_dispose_helper_required_sites\": "
      << inputs.local_block_ownership_dispose_helper_required_sites +
             imported_block_ownership_dispose_helper_required_sites
      << ",\n"
      << "  \"transitive_block_ownership_copy_helper_symbolized_sites\": "
      << inputs.local_block_ownership_copy_helper_symbolized_sites +
             imported_block_ownership_copy_helper_symbolized_sites
      << ",\n"
      << "  \"transitive_block_ownership_dispose_helper_symbolized_sites\": "
      << inputs.local_block_ownership_dispose_helper_symbolized_sites +
             imported_block_ownership_dispose_helper_symbolized_sites
      << ",\n"
      << "  \"transitive_block_ownership_escape_to_heap_sites\": "
      << inputs.local_block_ownership_escape_to_heap_sites +
             imported_block_ownership_escape_to_heap_sites
      << ",\n"
      << "  \"transitive_block_ownership_byref_layout_symbolized_sites\": "
      << inputs.local_block_ownership_byref_layout_symbolized_sites +
             imported_block_ownership_byref_layout_symbolized_sites
      << ",\n"
      << "  \"local_storage_reflection_implementation_owned_property_entries\": "
      << inputs.local_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_accessor_owner_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_getter_entries\": "
      << inputs.local_storage_reflection_synthesized_getter_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_setter_entries\": "
      << inputs.local_storage_reflection_synthesized_setter_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_accessor_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "  \"local_storage_reflection_current_property_read_entries\": "
      << inputs.local_storage_reflection_current_property_read_entries
      << ",\n"
      << "  \"local_storage_reflection_current_property_write_entries\": "
      << inputs.local_storage_reflection_current_property_write_entries
      << ",\n"
      << "  \"local_storage_reflection_current_property_exchange_entries\": "
      << inputs.local_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "  \"local_storage_reflection_weak_current_property_load_entries\": "
      << inputs.local_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "  \"local_storage_reflection_weak_current_property_store_entries\": "
      << inputs.local_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "  \"local_storage_reflection_ivar_layout_entries\": "
      << inputs.local_storage_reflection_ivar_layout_entries << ",\n"
      << "  \"local_storage_reflection_ivar_layout_owner_entries\": "
      << inputs.local_storage_reflection_ivar_layout_owner_entries << ",\n"
      << "  \"imported_storage_reflection_implementation_owned_property_entries\": "
      << imported_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "  \"imported_storage_reflection_synthesized_accessor_owner_entries\": "
      << imported_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "  \"imported_storage_reflection_synthesized_getter_entries\": "
      << imported_storage_reflection_synthesized_getter_entries << ",\n"
      << "  \"imported_storage_reflection_synthesized_setter_entries\": "
      << imported_storage_reflection_synthesized_setter_entries << ",\n"
      << "  \"imported_storage_reflection_synthesized_accessor_entries\": "
      << imported_storage_reflection_synthesized_accessor_entries << ",\n"
      << "  \"imported_storage_reflection_current_property_read_entries\": "
      << imported_storage_reflection_current_property_read_entries << ",\n"
      << "  \"imported_storage_reflection_current_property_write_entries\": "
      << imported_storage_reflection_current_property_write_entries << ",\n"
      << "  \"imported_storage_reflection_current_property_exchange_entries\": "
      << imported_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "  \"imported_storage_reflection_weak_current_property_load_entries\": "
      << imported_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "  \"imported_storage_reflection_weak_current_property_store_entries\": "
      << imported_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "  \"imported_storage_reflection_ivar_layout_entries\": "
      << imported_storage_reflection_ivar_layout_entries << ",\n"
      << "  \"imported_storage_reflection_ivar_layout_owner_entries\": "
      << imported_storage_reflection_ivar_layout_owner_entries << ",\n"
      << "  \"transitive_storage_reflection_implementation_owned_property_entries\": "
      << inputs.local_storage_reflection_implementation_owned_property_entries +
             imported_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_accessor_owner_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_owner_entries +
             imported_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_getter_entries\": "
      << inputs.local_storage_reflection_synthesized_getter_entries +
             imported_storage_reflection_synthesized_getter_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_setter_entries\": "
      << inputs.local_storage_reflection_synthesized_setter_entries +
             imported_storage_reflection_synthesized_setter_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_accessor_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_entries +
             imported_storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "  \"transitive_storage_reflection_current_property_read_entries\": "
      << inputs.local_storage_reflection_current_property_read_entries +
             imported_storage_reflection_current_property_read_entries
      << ",\n"
      << "  \"transitive_storage_reflection_current_property_write_entries\": "
      << inputs.local_storage_reflection_current_property_write_entries +
             imported_storage_reflection_current_property_write_entries
      << ",\n"
      << "  \"transitive_storage_reflection_current_property_exchange_entries\": "
      << inputs.local_storage_reflection_current_property_exchange_entries +
             imported_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "  \"transitive_storage_reflection_weak_current_property_load_entries\": "
      << inputs.local_storage_reflection_weak_current_property_load_entries +
             imported_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "  \"transitive_storage_reflection_weak_current_property_store_entries\": "
      << inputs.local_storage_reflection_weak_current_property_store_entries +
             imported_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "  \"transitive_storage_reflection_ivar_layout_entries\": "
      << inputs.local_storage_reflection_ivar_layout_entries +
             imported_storage_reflection_ivar_layout_entries
      << ",\n"
      << "  \"transitive_storage_reflection_ivar_layout_owner_entries\": "
      << inputs.local_storage_reflection_ivar_layout_owner_entries +
             imported_storage_reflection_ivar_layout_owner_entries
      << ",\n"
      << "  \"cleanup_unwind_runtime_link_model\": "
      << "\"linker-response-plus-runtime-support-archive-sidecars-provide-runnable-cleanup-executable-link-inputs\",\n"
      << "  \"runtime_support_library_archive_relative_path\": \""
      << EscapeJsonString(inputs.runtime_support_library_archive_relative_path)
      << "\",\n"
      << "  \"object_format\": \"" << EscapeJsonString(inputs.object_format)
      << "\",\n"
      << "  \"local_module\": {\n"
      << "    \"module_name\": \""
      << EscapeJsonString(inputs.local_module_name) << "\",\n"
      << "    \"import_surface_artifact_relative_path\": \""
      << EscapeJsonString(inputs.local_import_surface_artifact_relative_path)
      << "\",\n"
      << "    \"registration_manifest_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.local_registration_manifest_artifact_relative_path)
      << "\",\n"
      << "    \"object_artifact_relative_path\": \""
      << EscapeJsonString(inputs.local_object_artifact_relative_path)
      << "\",\n"
      << "    \"translation_unit_identity_model\": \""
      << EscapeJsonString(inputs.local_translation_unit_identity_model)
      << "\",\n"
      << "    \"translation_unit_identity_key\": \""
      << EscapeJsonString(inputs.local_translation_unit_identity_key)
      << "\",\n"
      << "    \"translation_unit_registration_order_ordinal\": "
      << inputs.local_translation_unit_registration_order_ordinal << ",\n"
      << "    \"class_descriptor_count\": "
      << inputs.local_class_descriptor_count << ",\n"
      << "    \"protocol_descriptor_count\": "
      << inputs.local_protocol_descriptor_count << ",\n"
      << "    \"category_descriptor_count\": "
      << inputs.local_category_descriptor_count << ",\n"
      << "    \"property_descriptor_count\": "
      << inputs.local_property_descriptor_count << ",\n"
      << "    \"ivar_descriptor_count\": "
      << inputs.local_ivar_descriptor_count << ",\n"
      << "    \"total_descriptor_count\": "
      << inputs.local_total_descriptor_count << ",\n"
      << "    \"storage_reflection_implementation_owned_property_entries\": "
      << inputs.local_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_accessor_owner_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_getter_entries\": "
      << inputs.local_storage_reflection_synthesized_getter_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_setter_entries\": "
      << inputs.local_storage_reflection_synthesized_setter_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_accessor_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "    \"storage_reflection_current_property_read_entries\": "
      << inputs.local_storage_reflection_current_property_read_entries
      << ",\n"
      << "    \"storage_reflection_current_property_write_entries\": "
      << inputs.local_storage_reflection_current_property_write_entries
      << ",\n"
      << "    \"storage_reflection_current_property_exchange_entries\": "
      << inputs.local_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "    \"storage_reflection_weak_current_property_load_entries\": "
      << inputs.local_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "    \"storage_reflection_weak_current_property_store_entries\": "
      << inputs.local_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "    \"storage_reflection_ivar_layout_entries\": "
      << inputs.local_storage_reflection_ivar_layout_entries << ",\n"
      << "    \"storage_reflection_ivar_layout_owner_entries\": "
      << inputs.local_storage_reflection_ivar_layout_owner_entries << ",\n"
      << "    \"driver_linker_flags\": "
      << BuildIndentedStringArrayJson(inputs.local_driver_linker_flags,
                                      "      ")
      << "\n"
      << "  },\n"
      << "  \"imported_modules\": " << imported_modules_json << ",\n"
      << "  \"link_object_artifacts\": "
      << BuildIndentedStringArrayJson(ordered_link_object_artifacts, "    ")
      << ",\n"
      << "  \"driver_linker_flags\": "
      << BuildIndentedStringArrayJson(merged_driver_linker_flags, "    ")
      << ",\n"
      << "  \"ready\": true\n"
      << "}\n";
  plan_json = out.str();

  linker_response_payload =
      BuildObjc3CrossModuleRuntimeLinkerResponsePayload(
          merged_driver_linker_flags);
  return true;
}
