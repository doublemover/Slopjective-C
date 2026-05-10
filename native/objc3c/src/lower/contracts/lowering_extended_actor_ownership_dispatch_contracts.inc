bool IsValidObjc3ActorLoweringMetadataContract(
    const Objc3ActorLoweringMetadataContract &contract) {
  if (contract.actor_method_sites < contract.nonisolated_entry_sites ||
      contract.actor_method_sites < contract.executor_affinity_sites ||
      contract.actor_method_sites < contract.actor_hop_artifact_sites ||
      contract.actor_method_sites < contract.actor_isolation_thunk_sites ||
      contract.actor_method_sites < contract.guard_blocked_sites ||
      contract.actor_method_sites < contract.contract_violation_sites ||
      contract.actor_metadata_record_sites < contract.actor_interface_sites ||
      contract.task_handoff_sites < contract.actor_hop_artifact_sites ||
      contract.task_handoff_sites < contract.actor_isolation_thunk_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ActorLoweringMetadataReplayKey(
    const Objc3ActorLoweringMetadataContract &contract) {
  return std::string("actor_interface_sites=") +
             std::to_string(contract.actor_interface_sites) +
         ";actor_method_sites=" + std::to_string(contract.actor_method_sites) +
         ";actor_metadata_record_sites=" +
             std::to_string(contract.actor_metadata_record_sites) +
         ";nonisolated_entry_sites=" +
             std::to_string(contract.nonisolated_entry_sites) +
         ";executor_affinity_sites=" +
             std::to_string(contract.executor_affinity_sites) +
         ";actor_hop_artifact_sites=" +
             std::to_string(contract.actor_hop_artifact_sites) +
         ";actor_isolation_thunk_sites=" +
             std::to_string(contract.actor_isolation_thunk_sites) +
         ";replay_proof_dependency_sites=" +
             std::to_string(contract.replay_proof_dependency_sites) +
         ";race_guard_dependency_sites=" +
             std::to_string(contract.race_guard_dependency_sites) +
         ";task_handoff_sites=" + std::to_string(contract.task_handoff_sites) +
         ";guard_blocked_sites=" +
             std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
             std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
          ";lane_contract=" + kObjc3ConcurrencyActorLoweringMetadataLaneContract;
}

bool IsValidObjc3OwnershipSystemExtensionLoweringContract(
    const Objc3OwnershipSystemExtensionLoweringContract &contract) {
  if (contract.cleanup_hook_sites > contract.cleanup_owned_local_sites ||
      contract.resource_local_sites > contract.cleanup_owned_local_sites ||
      contract.resource_move_capture_sites > contract.cleanup_owned_local_sites ||
      contract.retainable_family_operation_callable_sites >
          contract.retainable_family_callable_sites ||
      contract.retainable_family_alias_callable_sites >
          contract.retainable_family_callable_sites ||
      contract.contract_violation_sites >
          contract.cleanup_owned_local_sites +
              contract.borrowed_escape_candidate_sites +
              contract.retainable_family_callable_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3OwnershipSystemExtensionLoweringReplayKey(
    const Objc3OwnershipSystemExtensionLoweringContract &contract) {
  return std::string("cleanup_hook_sites=") +
             std::to_string(contract.cleanup_hook_sites) +
         ";resource_local_sites=" +
         std::to_string(contract.resource_local_sites) +
         ";cleanup_owned_local_sites=" +
         std::to_string(contract.cleanup_owned_local_sites) +
         ";resource_move_capture_sites=" +
         std::to_string(contract.resource_move_capture_sites) +
         ";borrowed_parameter_sites=" +
         std::to_string(contract.borrowed_parameter_sites) +
         ";borrowed_return_callable_sites=" +
         std::to_string(contract.borrowed_return_callable_sites) +
         ";borrowed_escape_candidate_sites=" +
         std::to_string(contract.borrowed_escape_candidate_sites) +
         ";explicit_capture_item_sites=" +
         std::to_string(contract.explicit_capture_item_sites) +
         ";retainable_family_callable_sites=" +
         std::to_string(contract.retainable_family_callable_sites) +
         ";retainable_family_operation_callable_sites=" +
         std::to_string(contract.retainable_family_operation_callable_sites) +
         ";retainable_family_alias_callable_sites=" +
         std::to_string(contract.retainable_family_alias_callable_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3OwnershipSystemExtensionLoweringLaneContract;
}

bool IsValidObjc3DispatchDispatchControlLoweringContract(
    const Objc3DispatchDispatchControlLoweringContract &contract) {
  if (contract.direct_members_defaulted_sites >
          contract.direct_call_candidate_sites ||
      contract.dynamic_opt_out_sites >
          contract.metadata_preserved_callable_sites ||
      contract.final_container_sites >
          contract.metadata_preserved_container_sites ||
      contract.sealed_container_sites >
          contract.metadata_preserved_container_sites ||
      contract.override_legality_sites >
          contract.direct_call_candidate_sites +
              contract.metadata_preserved_container_sites ||
      contract.guard_blocked_sites >
          contract.metadata_preserved_callable_sites +
              contract.metadata_preserved_container_sites ||
      contract.contract_violation_sites >
          contract.metadata_preserved_callable_sites +
              contract.metadata_preserved_container_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3DispatchDispatchControlLoweringReplayKey(
    const Objc3DispatchDispatchControlLoweringContract &contract) {
  return std::string("direct_call_candidate_sites=") +
             std::to_string(contract.direct_call_candidate_sites) +
         ";direct_members_defaulted_sites=" +
         std::to_string(contract.direct_members_defaulted_sites) +
         ";dynamic_opt_out_sites=" +
         std::to_string(contract.dynamic_opt_out_sites) +
         ";final_container_sites=" +
         std::to_string(contract.final_container_sites) +
         ";sealed_container_sites=" +
         std::to_string(contract.sealed_container_sites) +
         ";override_legality_sites=" +
         std::to_string(contract.override_legality_sites) +
         ";metadata_preserved_callable_sites=" +
         std::to_string(contract.metadata_preserved_callable_sites) +
         ";metadata_preserved_container_sites=" +
         std::to_string(contract.metadata_preserved_container_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3DispatchDispatchControlLoweringLaneContract;
}

std::string Objc3DispatchDispatchMetadataInterfacePreservationSummary() {
  std::ostringstream out;
  // preservation anchor: lane-C extends the local C002 lowering win
  // by preserving direct/final/sealed intent through runtime metadata source
  // records, emitted runtime-import-surface artifacts, and imported replay
  // summaries before lane-D runtime boundary work begins.
  out << "contract="
      << kObjc3DispatchDispatchMetadataInterfacePreservationContractId
      << ";source_contract=" << kObjc3DispatchDispatchControlLoweringContractId
      << ";surface_path="
      << kObjc3DispatchDispatchMetadataInterfacePreservationSurfacePath
      << ";artifact_member="
      << kObjc3DispatchDispatchMetadataInterfacePreservationImportArtifactMemberName
      << ";source_model="
      << kObjc3DispatchDispatchMetadataInterfacePreservationSourceModel
      << ";preservation_model="
      << kObjc3DispatchDispatchMetadataInterfacePreservationModel
      << ";fail_closed_model="
      << kObjc3DispatchDispatchMetadataInterfacePreservationFailClosedModel
      << ";follow_on_surface=objc3c.dispatch.metadata.interfacepreservation.v1";
  return out.str();
}
