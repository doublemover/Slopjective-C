#include "io/objc3_cross_module_imported_modules_document_record_runtime_sections.h"

#include <ostream>

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void EmitObjc3CrossModuleImportedModuleRecordRuntimePreludeJson(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input) {
  out << "      \"error_handling_result_and_bridging_artifact_replay_present\": "
      << (imported_input.error_handling_result_and_bridging_artifact_replay_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"error_handling_binary_artifact_replay_ready\": "
      << (imported_input.error_handling_binary_artifact_replay_ready ? "true"
                                                                     : "false")
      << ",\n"
      << "      \"error_handling_runtime_import_artifact_ready\": "
      << (imported_input.error_handling_runtime_import_artifact_ready ? "true"
                                                                     : "false")
      << ",\n"
      << "      \"error_handling_separate_compilation_replay_ready\": "
      << (imported_input.error_handling_separate_compilation_replay_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"error_handling_deterministic\": "
      << (imported_input.error_handling_deterministic ? "true" : "false")
      << ",\n"
      << "      \"error_handling_contract_id\": \""
      << EscapeJsonString(imported_input.error_handling_contract_id) << "\",\n"
      << "      \"error_handling_source_contract_id\": \""
      << EscapeJsonString(imported_input.error_handling_source_contract_id)
      << "\",\n"
      << "      \"error_handling_result_and_bridging_artifact_replay_key\": \""
      << EscapeJsonString(
             imported_input.error_handling_result_and_bridging_artifact_replay_key)
      << "\",\n"
      << "      \"error_handling_replay_key\": \""
      << EscapeJsonString(imported_input.error_handling_error_handling_replay_key)
      << "\",\n"
      << "      \"throws_replay_key\": \""
      << EscapeJsonString(imported_input.error_handling_throws_replay_key)
      << "\",\n"
      << "      \"result_like_replay_key\": \""
      << EscapeJsonString(imported_input.error_handling_result_like_replay_key)
      << "\",\n"
      << "      \"ns_error_replay_key\": \""
      << EscapeJsonString(imported_input.error_handling_ns_error_replay_key)
      << "\",\n"
      << "      \"unwind_replay_key\": \""
      << EscapeJsonString(imported_input.error_handling_unwind_replay_key)
      << "\",\n"
      << "      \"concurrency_actor_mailbox_runtime_import_present\": "
      << (imported_input.concurrency_actor_mailbox_runtime_import_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"concurrency_actor_interface_sites\": "
      << imported_input.concurrency_actor_interface_sites << ",\n"
      << "      \"concurrency_actor_method_sites\": "
      << imported_input.concurrency_actor_method_sites << ",\n"
      << "      \"concurrency_actor_metadata_record_sites\": "
      << imported_input.concurrency_actor_metadata_record_sites << ",\n"
      << "      \"concurrency_actor_nonisolated_entry_sites\": "
      << imported_input.concurrency_actor_nonisolated_entry_sites << ",\n"
      << "      \"concurrency_actor_executor_affinity_sites\": "
      << imported_input.concurrency_actor_executor_affinity_sites << ",\n"
      << "      \"concurrency_actor_hop_artifact_sites\": "
      << imported_input.concurrency_actor_hop_artifact_sites << ",\n"
      << "      \"concurrency_actor_isolation_thunk_sites\": "
      << imported_input.concurrency_actor_isolation_thunk_sites << ",\n"
      << "      \"concurrency_actor_replay_proof_dependency_sites\": "
      << imported_input.concurrency_actor_replay_proof_dependency_sites << ",\n"
      << "      \"concurrency_actor_race_guard_dependency_sites\": "
      << imported_input.concurrency_actor_race_guard_dependency_sites << ",\n"
      << "      \"concurrency_actor_task_handoff_sites\": "
      << imported_input.concurrency_actor_task_handoff_sites << ",\n"
      << "      \"concurrency_actor_mailbox_message_identity_field_count\": "
      << imported_input.concurrency_actor_mailbox_message_identity_field_count
      << ",\n"
      << "      \"concurrency_actor_mailbox_fifo_ordering_field_count\": "
      << imported_input.concurrency_actor_mailbox_fifo_ordering_field_count
      << ",\n"
      << "      \"concurrency_actor_mailbox_drain_operation_field_count\": "
      << imported_input.concurrency_actor_mailbox_drain_operation_field_count
      << ",\n"
      << "      \"concurrency_actor_mailbox_cancel_operation_field_count\": "
      << imported_input.concurrency_actor_mailbox_cancel_operation_field_count
      << ",\n"
      << "      \"concurrency_actor_mailbox_error_operation_field_count\": "
      << imported_input.concurrency_actor_mailbox_error_operation_field_count
      << ",\n"
      << "      \"concurrency_actor_mailbox_shutdown_operation_field_count\": "
      << imported_input.concurrency_actor_mailbox_shutdown_operation_field_count
      << ",\n"
      << "      \"concurrency_actor_distributed_transport_evidence_sites\": "
      << imported_input.concurrency_actor_distributed_transport_evidence_sites
      << ",\n"
      << "      \"concurrency_actor_guard_blocked_sites\": "
      << imported_input.concurrency_actor_guard_blocked_sites << ",\n"
      << "      \"concurrency_actor_contract_violation_sites\": "
      << imported_input.concurrency_actor_contract_violation_sites << ",\n"
      << "      \"concurrency_actor_mailbox_runtime_ready\": "
      << (imported_input.concurrency_actor_mailbox_runtime_ready ? "true"
                                                                 : "false")
      << ",\n"
      << "      \"concurrency_actor_mailbox_runtime_deterministic\": "
      << (imported_input.concurrency_actor_mailbox_runtime_deterministic
              ? "true"
              : "false")
      << ",\n"
      << "      \"concurrency_actor_contract_id\": \""
      << EscapeJsonString(imported_input.concurrency_actor_contract_id)
      << "\",\n"
      << "      \"concurrency_actor_source_contract_id\": \""
      << EscapeJsonString(imported_input.concurrency_actor_source_contract_id)
      << "\",\n"
      << "      \"concurrency_actor_mailbox_runtime_replay_key\": \""
      << EscapeJsonString(
             imported_input.concurrency_actor_mailbox_runtime_replay_key)
      << "\",\n"
      << "      \"concurrency_actor_lowering_replay_key\": \""
      << EscapeJsonString(imported_input.concurrency_actor_lowering_replay_key)
      << "\",\n"
      << "      \"concurrency_actor_isolation_lowering_replay_key\": \""
      << EscapeJsonString(
             imported_input.concurrency_actor_isolation_lowering_replay_key)
      << "\",\n"
      << "      \"concurrency_scheduler_task_runtime_import_present\": "
      << (imported_input.concurrency_scheduler_task_runtime_import_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"concurrency_scheduler_task_runtime_ready\": "
      << (imported_input.concurrency_scheduler_task_runtime_ready ? "true"
                                                                  : "false")
      << ",\n"
      << "      \"concurrency_scheduler_task_runtime_deterministic\": "
      << (imported_input.concurrency_scheduler_task_runtime_deterministic
              ? "true"
              : "false")
      << ",\n"
      << "      \"concurrency_scheduler_shutdown_drain_ready\": "
      << (imported_input.concurrency_scheduler_shutdown_drain_ready ? "true"
                                                                    : "false")
      << ",\n"
      << "      \"concurrency_scheduler_cancellation_error_cleanup_ready\": "
      << (imported_input
                  .concurrency_scheduler_cancellation_error_cleanup_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"concurrency_scheduler_task_runtime_contract_id\": \""
      << EscapeJsonString(
             imported_input.concurrency_scheduler_task_runtime_contract_id)
      << "\",\n"
      << "      \"concurrency_scheduler_task_runtime_source_contract_id\": \""
      << EscapeJsonString(
             imported_input.concurrency_scheduler_task_runtime_source_contract_id)
      << "\",\n"
      << "      \"concurrency_scheduler_task_runtime_replay_key\": \""
      << EscapeJsonString(
             imported_input.concurrency_scheduler_task_runtime_replay_key)
      << "\",\n"
      << "      \"concurrency_scheduler_task_lifecycle_replay_key\": \""
      << EscapeJsonString(
             imported_input.concurrency_scheduler_task_lifecycle_replay_key)
      << "\",\n"
      << "      \"concurrency_scheduler_task_cancellation_replay_key\": \""
      << EscapeJsonString(
             imported_input.concurrency_scheduler_task_cancellation_replay_key)
      << "\",\n"
      << "      \"concurrency_scheduler_task_shutdown_replay_key\": \""
      << EscapeJsonString(
             imported_input.concurrency_scheduler_task_shutdown_replay_key)
      << "\",\n"
      << "      \"concurrency_scheduler_task_record_sites\": "
      << imported_input.concurrency_scheduler_task_record_sites << ",\n"
      << "      \"concurrency_scheduler_continuation_record_sites\": "
      << imported_input.concurrency_scheduler_continuation_record_sites
      << ",\n"
      << "      \"concurrency_scheduler_executor_hop_record_sites\": "
      << imported_input.concurrency_scheduler_executor_hop_record_sites
      << ",\n"
      << "      \"concurrency_scheduler_queue_lifecycle_record_sites\": "
      << imported_input.concurrency_scheduler_queue_lifecycle_record_sites
      << ",\n"
      << "      \"concurrency_scheduler_cancellation_checkpoint_sites\": "
      << imported_input.concurrency_scheduler_cancellation_checkpoint_sites
      << ",\n"
      << "      \"concurrency_scheduler_error_cleanup_sites\": "
      << imported_input.concurrency_scheduler_error_cleanup_sites << ",\n"
      << "      \"concurrency_scheduler_shutdown_drain_sites\": "
      << imported_input.concurrency_scheduler_shutdown_drain_sites << ",\n"
      << "      \"concurrency_scheduler_unsupported_policy_sites\": "
      << imported_input.concurrency_scheduler_unsupported_policy_sites
      << ",\n";
}

void EmitObjc3CrossModuleImportedModuleRecordMetaprogrammingSectionJson(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input) {
  out << "      \"metaprogramming_macro_host_process_cache_runtime_integration_present\": "
      << (imported_input
                  .metaprogramming_macro_host_process_cache_runtime_integration_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"metaprogramming_macro_host_process_cache_runtime_ready\": "
      << (imported_input.metaprogramming_macro_host_process_cache_runtime_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"metaprogramming_macro_host_process_cache_separate_compilation_ready\": "
      << (imported_input
                  .metaprogramming_macro_host_process_cache_separate_compilation_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"metaprogramming_macro_host_process_cache_deterministic\": "
      << (imported_input.metaprogramming_macro_host_process_cache_deterministic
              ? "true"
              : "false")
      << ",\n"
      << "      \"metaprogramming_macro_host_process_cache_contract_id\": \""
      << EscapeJsonString(
             imported_input.metaprogramming_macro_host_process_cache_contract_id)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_source_contract_id\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_source_contract_id)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_replay_key\": \""
      << EscapeJsonString(
             imported_input.metaprogramming_macro_host_process_cache_replay_key)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_host_executable_relative_path\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_host_executable_relative_path)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_root_relative_path\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_root_relative_path)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_package_identity\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_package_identity)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_package_lock_identity\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_package_lock_identity)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_package_trust_identity\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_package_trust_identity)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_input_content_identity\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_input_content_identity)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_output_content_identity\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_output_content_identity)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_host_identity\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_host_identity)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_validation_status\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_validation_status)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_runtime_consumption_artifact_identity\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_runtime_consumption_artifact_identity)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_package_replay_generation\": "
      << imported_input
             .metaprogramming_macro_host_process_cache_package_replay_generation
      << ",\n";
}
