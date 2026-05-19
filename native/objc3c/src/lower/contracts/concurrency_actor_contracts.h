#pragma once

#include <cstddef>
#include <string>

// Actor concurrency contracts own actor isolation/sendability, actor metadata,
// runtime actor helper symbols, and deterministic replay/race-guard surfaces.
inline constexpr const char *kObjc3RuntimeActorEnterIsolationThunkI32Symbol =
    "objc3_runtime_actor_enter_isolation_thunk_i32";
inline constexpr const char *kObjc3RuntimeActorEnterNonisolatedI32Symbol =
    "objc3_runtime_actor_enter_nonisolated_i32";
inline constexpr const char *kObjc3RuntimeActorHopToExecutorI32Symbol =
    "objc3_runtime_actor_hop_to_executor_i32";
inline constexpr const char *kObjc3RuntimeActorRecordReplayProofI32Symbol =
    "objc3_runtime_actor_record_replay_proof_i32";
inline constexpr const char *kObjc3RuntimeActorRecordRaceGuardI32Symbol =
    "objc3_runtime_actor_record_race_guard_i32";
inline constexpr const char *kObjc3RuntimeActorBindExecutorI32Symbol =
    "objc3_runtime_actor_bind_executor_i32";
inline constexpr const char *kObjc3RuntimeActorMailboxEnqueueI32Symbol =
    "objc3_runtime_actor_mailbox_enqueue_i32";
inline constexpr const char *kObjc3RuntimeActorMailboxDrainNextI32Symbol =
    "objc3_runtime_actor_mailbox_drain_next_i32";

inline constexpr const char *kObjc3ActorIsolationSendabilityLoweringLaneContract =
    "objc3c.actor.isolation.sendability.lowering.v1";

inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataContractId =
    "objc3c.concurrency.actor.lowering.and.metadata.contract.v1";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_actor_lowering_and_metadata_contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataModel =
    "actor-member-semantic-and-hazard-packets-now-lower-through-one-deterministic-actor-metadata-isolation-thunk-and-hop-artifact-contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataDeferredModel =
    "live-actor-thunk-bodies-mailbox-runtime-entrypoints-and-runnable-cross-actor-scheduling-remain-later-actor-lowering-and-replay-runtime-work";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataLaneContract =
    "objc3c.actor.lowering.metadata.contract.v1";

inline constexpr const char *kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract =
    "objc3c.concurrency.replay.race.guard.lowering.v1";

struct Objc3ActorIsolationSendabilityLoweringContract {
  std::size_t actor_isolation_sites = 0;
  std::size_t sendability_check_sites = 0;
  std::size_t cross_actor_hop_sites = 0;
  std::size_t non_sendable_capture_sites = 0;
  std::size_t sendable_transfer_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ActorLoweringMetadataContract {
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_metadata_record_sites = 0;
  std::size_t nonisolated_entry_sites = 0;
  std::size_t executor_affinity_sites = 0;
  std::size_t actor_hop_artifact_sites = 0;
  std::size_t actor_isolation_thunk_sites = 0;
  std::size_t replay_proof_dependency_sites = 0;
  std::size_t race_guard_dependency_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ConcurrencyReplayRaceGuardLoweringContract {
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

bool IsValidObjc3ActorIsolationSendabilityLoweringContract(
    const Objc3ActorIsolationSendabilityLoweringContract &contract);
std::string Objc3ActorIsolationSendabilityLoweringReplayKey(
    const Objc3ActorIsolationSendabilityLoweringContract &contract);
bool IsValidObjc3ActorLoweringMetadataContract(
    const Objc3ActorLoweringMetadataContract &contract);
std::string Objc3ActorLoweringMetadataReplayKey(
    const Objc3ActorLoweringMetadataContract &contract);
bool IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract);
std::string Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract);
