#include "io/objc3_cross_module_runtime_link_plan_sections_validation_replay.h"

bool TryValidateImportedLinkPlanReplaySurfaces(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    std::unordered_set<std::string> &seen_error_handling_replay_keys,
    std::unordered_set<std::string> &seen_concurrency_actor_replay_keys,
    std::unordered_set<std::string> &seen_interop_ffi_replay_keys,
    std::unordered_set<std::string>
        &seen_interop_header_module_bridge_replay_keys,
    std::unordered_set<std::string> &seen_metaprogramming_host_cache_replay_keys,
    std::string &error) {
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
  return true;
}
