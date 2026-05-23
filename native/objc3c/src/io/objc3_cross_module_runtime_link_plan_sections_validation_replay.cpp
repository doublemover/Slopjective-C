#include "io/objc3_cross_module_runtime_link_plan_sections_validation_replay.h"

bool TryValidateImportedLinkPlanReplaySurfaces(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    std::unordered_set<std::string> &seen_error_handling_replay_keys,
    std::unordered_set<std::string> &seen_concurrency_actor_replay_keys,
    std::unordered_set<std::string> &seen_scheduler_task_replay_keys,
    std::unordered_set<std::string> &seen_interop_ffi_replay_keys,
    std::unordered_set<std::string> &seen_foreign_abi_replay_keys,
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
        imported_input.concurrency_actor_interface_sites == 0u ||
        imported_input.concurrency_actor_metadata_record_sites <
            imported_input.concurrency_actor_interface_sites ||
        imported_input.concurrency_actor_contract_violation_sites != 0u ||
        imported_input.concurrency_actor_mailbox_runtime_replay_key.empty() ||
        imported_input.concurrency_actor_lowering_replay_key.empty() ||
        imported_input.concurrency_actor_isolation_lowering_replay_key.empty() ||
        imported_input
                .concurrency_actor_mailbox_message_identity_field_count == 0u ||
        imported_input.concurrency_actor_mailbox_fifo_ordering_field_count ==
            0u ||
        imported_input.concurrency_actor_mailbox_drain_operation_field_count ==
            0u ||
        imported_input.concurrency_actor_mailbox_cancel_operation_field_count ==
            0u ||
        imported_input.concurrency_actor_mailbox_error_operation_field_count ==
            0u ||
        imported_input
                .concurrency_actor_mailbox_shutdown_operation_field_count ==
            0u ||
        imported_input.concurrency_actor_distributed_transport_evidence_sites !=
            0u) {
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
  if (imported_input.concurrency_scheduler_task_runtime_import_present) {
    if (imported_input.concurrency_scheduler_task_runtime_contract_id !=
        "objc3c.concurrency.scheduler.task.runtime.import.surface.v1") {
      error =
          "cross-module runtime link-plan Part 7 scheduler/task contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.concurrency_scheduler_task_runtime_source_contract_id !=
        "objc3c.concurrency.task.runtime.lowering.contract.v1") {
      error =
          "cross-module runtime link-plan Part 7 scheduler/task source contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.concurrency_scheduler_task_runtime_ready ||
        !imported_input.concurrency_scheduler_task_runtime_deterministic ||
        !imported_input.concurrency_scheduler_shutdown_drain_ready ||
        !imported_input
             .concurrency_scheduler_cancellation_error_cleanup_ready ||
        imported_input.concurrency_scheduler_task_record_sites == 0u ||
        imported_input.concurrency_scheduler_continuation_record_sites == 0u ||
        imported_input.concurrency_scheduler_executor_hop_record_sites == 0u ||
        imported_input.concurrency_scheduler_queue_lifecycle_record_sites ==
            0u ||
        imported_input.concurrency_scheduler_cancellation_checkpoint_sites ==
            0u ||
        imported_input.concurrency_scheduler_error_cleanup_sites == 0u ||
        imported_input.concurrency_scheduler_shutdown_drain_sites == 0u ||
        imported_input.concurrency_scheduler_unsupported_policy_sites != 0u ||
        imported_input.concurrency_scheduler_task_runtime_replay_key.empty() ||
        imported_input.concurrency_scheduler_task_lifecycle_replay_key.empty() ||
        imported_input.concurrency_scheduler_task_cancellation_replay_key
            .empty() ||
        imported_input.concurrency_scheduler_task_shutdown_replay_key.empty()) {
      error =
          "cross-module runtime link-plan Part 7 scheduler/task replay surface incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (!seen_scheduler_task_replay_keys
             .insert(
                 imported_input.concurrency_scheduler_task_runtime_replay_key)
             .second) {
      error =
          "cross-module runtime link-plan duplicate imported Part 7 scheduler/task replay key: " +
          imported_input.concurrency_scheduler_task_runtime_replay_key;
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
  if (imported_input.interop_foreign_abi_runtime_closure_present) {
    if (imported_input.interop_foreign_abi_runtime_contract_id !=
        "objc3c.interop.foreign.abi.runtime.closure.v1") {
      error =
          "cross-module runtime link-plan Part 11 foreign ABI closure contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.interop_foreign_abi_source_contract_id !=
        inputs.expected_interop_ffi_contract_id) {
      error =
          "cross-module runtime link-plan Part 11 foreign ABI closure source contract mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (!imported_input.interop_ffi_metadata_interface_preservation_present ||
        !imported_input.interop_foreign_abi_runtime_closure_ready ||
        !imported_input.interop_foreign_abi_runtime_closure_deterministic ||
        !imported_input.interop_foreign_abi_typed_dispatch_ready ||
        !imported_input.interop_foreign_abi_package_runtime_identity_ready ||
        !imported_input.interop_foreign_abi_bridge_ownership_ready ||
        imported_input.interop_foreign_abi_package_identity.empty() ||
        imported_input.interop_foreign_abi_runtime_identity.empty() ||
        imported_input.interop_foreign_abi_replay_key.empty() ||
        imported_input.interop_foreign_abi_classification_replay_key.empty() ||
        imported_input.interop_foreign_abi_bridge_metadata_replay_key.empty() ||
        imported_input.interop_foreign_abi_foreign_surface_count == 0u ||
        imported_input.interop_foreign_abi_supported_c_abi_surface_count ==
            0u ||
        imported_input.interop_foreign_abi_supported_c_abi_surface_count >
            imported_input.interop_foreign_abi_foreign_surface_count ||
        imported_input
                .interop_foreign_abi_preserved_swift_metadata_surface_count ==
            0u ||
        imported_input
                .interop_foreign_abi_preserved_cpp_metadata_surface_count ==
            0u ||
        imported_input.interop_foreign_abi_rejected_surface_count == 0u ||
        imported_input.interop_foreign_abi_mismatch_negative_case_count == 0u ||
        imported_input
                .interop_foreign_abi_missing_bridge_ownership_negative_case_count ==
            0u ||
        imported_input
                .interop_foreign_abi_unsafe_mixed_image_negative_case_count ==
            0u ||
        imported_input.interop_foreign_abi_stale_import_negative_case_count ==
            0u ||
        imported_input
                .interop_foreign_abi_unsupported_runtime_fallback_negative_case_count ==
            0u) {
      error =
          "cross-module runtime link-plan Part 11 foreign ABI runtime closure incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (!seen_foreign_abi_replay_keys
             .insert(imported_input.interop_foreign_abi_replay_key)
             .second) {
      error =
          "cross-module runtime link-plan duplicate imported Part 11 foreign ABI replay key: " +
          imported_input.interop_foreign_abi_replay_key;
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
            .empty() ||
        imported_input.metaprogramming_macro_host_process_cache_package_identity
            .empty() ||
        imported_input
            .metaprogramming_macro_host_process_cache_package_lock_identity
            .empty() ||
        imported_input
            .metaprogramming_macro_host_process_cache_package_trust_identity
            .empty() ||
        imported_input
            .metaprogramming_macro_host_process_cache_input_content_identity
            .empty() ||
        imported_input
            .metaprogramming_macro_host_process_cache_output_content_identity
            .empty() ||
        imported_input.metaprogramming_macro_host_process_cache_host_identity
            .empty() ||
        imported_input
                .metaprogramming_macro_host_process_cache_validation_status !=
            "valid" ||
        imported_input
            .metaprogramming_macro_host_process_cache_runtime_consumption_artifact_identity
            .empty() ||
        imported_input
                .metaprogramming_macro_host_process_cache_package_replay_generation ==
            0u) {
      error =
          "cross-module runtime link-plan Part 10 macro host cache package replay surface incomplete for " +
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
    if (imported_input.metaprogramming_macro_host_process_cache_package_identity !=
        "std.metaprogramming.advanced-runtime") {
      error =
          "cross-module runtime link-plan Part 10 macro package identity mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input
            .metaprogramming_macro_host_process_cache_package_lock_identity !=
        "objc3c.metaprogramming.advanced-runtime.lock.v1") {
      error =
          "cross-module runtime link-plan Part 10 macro package lock identity mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input
            .metaprogramming_macro_host_process_cache_package_trust_identity !=
        "deterministic-sandbox+checked-package-replay") {
      error =
          "cross-module runtime link-plan Part 10 macro package trust identity mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.metaprogramming_macro_host_process_cache_host_identity
            .find(inputs.expected_metaprogramming_host_cache_executable_relative_path) !=
        0u) {
      error =
          "cross-module runtime link-plan Part 10 macro host identity mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input
            .metaprogramming_macro_host_process_cache_input_content_identity ==
        imported_input
            .metaprogramming_macro_host_process_cache_output_content_identity) {
      error =
          "cross-module runtime link-plan Part 10 macro replay input/output identity mismatch for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input
            .metaprogramming_macro_host_process_cache_runtime_consumption_artifact_identity
            .find(
                "objc_metaprogramming_macro_host_process_and_cache_runtime_integration") ==
        std::string::npos) {
      error =
          "cross-module runtime link-plan Part 10 macro runtime consumption identity mismatch for " +
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
