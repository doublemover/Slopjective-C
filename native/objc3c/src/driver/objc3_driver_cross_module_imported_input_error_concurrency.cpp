#include "driver/objc3_driver_cross_module_imported_input_error_concurrency.h"

void PopulateObjc3DriverCrossModuleRuntimeImportedInputErrorAndConcurrency(
    Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    const Objc3ImportedRuntimeModuleSurface &imported_surface) {
  imported_input.error_handling_result_and_bridging_artifact_replay_present =
      imported_surface.error_handling_result_and_bridging_artifact_replay_present;
  imported_input.error_handling_binary_artifact_replay_ready =
      imported_surface.error_handling_binary_artifact_replay_ready;
  imported_input.error_handling_runtime_import_artifact_ready =
      imported_surface.error_handling_runtime_import_artifact_ready;
  imported_input.error_handling_separate_compilation_replay_ready =
      imported_surface.error_handling_separate_compilation_replay_ready;
  imported_input.error_handling_deterministic =
      imported_surface.error_handling_deterministic;
  imported_input.error_handling_contract_id =
      imported_surface.error_handling_contract_id;
  imported_input.error_handling_source_contract_id =
      imported_surface.error_handling_source_contract_id;
  imported_input.error_handling_result_and_bridging_artifact_replay_key =
      imported_surface.error_handling_result_and_bridging_artifact_replay_key;
  imported_input.error_handling_error_handling_replay_key =
      imported_surface.error_handling_error_handling_replay_key;
  imported_input.error_handling_throws_replay_key =
      imported_surface.error_handling_throws_replay_key;
  imported_input.error_handling_result_like_replay_key =
      imported_surface.error_handling_result_like_replay_key;
  imported_input.error_handling_ns_error_replay_key =
      imported_surface.error_handling_ns_error_replay_key;
  imported_input.error_handling_unwind_replay_key =
      imported_surface.error_handling_unwind_replay_key;

  imported_input.concurrency_actor_mailbox_runtime_import_present =
      imported_surface.concurrency_actor_mailbox_runtime_import_present;
  imported_input.concurrency_actor_interface_sites =
      imported_surface.concurrency_actor_interface_sites;
  imported_input.concurrency_actor_method_sites =
      imported_surface.concurrency_actor_method_sites;
  imported_input.concurrency_actor_metadata_record_sites =
      imported_surface.concurrency_actor_metadata_record_sites;
  imported_input.concurrency_actor_nonisolated_entry_sites =
      imported_surface.concurrency_actor_nonisolated_entry_sites;
  imported_input.concurrency_actor_executor_affinity_sites =
      imported_surface.concurrency_actor_executor_affinity_sites;
  imported_input.concurrency_actor_hop_artifact_sites =
      imported_surface.concurrency_actor_hop_artifact_sites;
  imported_input.concurrency_actor_isolation_thunk_sites =
      imported_surface.concurrency_actor_isolation_thunk_sites;
  imported_input.concurrency_actor_replay_proof_dependency_sites =
      imported_surface.concurrency_actor_replay_proof_dependency_sites;
  imported_input.concurrency_actor_race_guard_dependency_sites =
      imported_surface.concurrency_actor_race_guard_dependency_sites;
  imported_input.concurrency_actor_task_handoff_sites =
      imported_surface.concurrency_actor_task_handoff_sites;
  imported_input.concurrency_actor_guard_blocked_sites =
      imported_surface.concurrency_actor_guard_blocked_sites;
  imported_input.concurrency_actor_contract_violation_sites =
      imported_surface.concurrency_actor_contract_violation_sites;
  imported_input.concurrency_actor_mailbox_runtime_ready =
      imported_surface.concurrency_actor_mailbox_runtime_ready;
  imported_input.concurrency_actor_mailbox_runtime_deterministic =
      imported_surface.concurrency_actor_mailbox_runtime_deterministic;
  imported_input.concurrency_actor_contract_id =
      imported_surface.concurrency_actor_mailbox_runtime_contract_id;
  imported_input.concurrency_actor_source_contract_id =
      imported_surface.concurrency_actor_mailbox_runtime_source_contract_id;
  imported_input.concurrency_actor_mailbox_runtime_replay_key =
      imported_surface.concurrency_actor_mailbox_runtime_replay_key;
  imported_input.concurrency_actor_lowering_replay_key =
      imported_surface.concurrency_actor_lowering_replay_key;
  imported_input.concurrency_actor_isolation_lowering_replay_key =
      imported_surface.concurrency_actor_isolation_lowering_replay_key;
}
