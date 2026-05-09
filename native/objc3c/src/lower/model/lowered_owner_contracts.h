#pragma once

#include <cstddef>
#include <string>

inline constexpr const char *kObjc3LoweringOwnerContractRecordId =
    "objc3c.lowering.owner.contract.record.v1";
inline constexpr const char *kObjc3LoweringOwnerTypedSemaHandoffContractId =
    "objc3c.lowering.owner.typed-sema-handoff.v1";
inline constexpr const char *kObjc3LoweringOwnerRuntimeStabilityContractId =
    "objc3c.lowering.owner.runtime-stability-invariant.v1";
inline constexpr const char *kObjc3LoweringOwnerPipelinePassGraphContractId =
    "objc3c.lowering.owner.pipeline-pass-graph.v1";
inline constexpr const char *kObjc3LoweringOwnerPassGraphCoreFeatureContractId =
    "objc3c.lowering.owner.pass-graph-core-feature.v1";
inline constexpr const char *kObjc3LoweringOwnerIREmissionCompletenessContractId =
    "objc3c.lowering.owner.ir-emission-completeness.v1";
inline constexpr const char *kObjc3LoweringOwnerDiagnosticsSurfacingContractId =
    "objc3c.lowering.owner.runtime-diagnostics-surfacing.v1";
inline constexpr const char *kObjc3LoweringOwnerToolchainGaContractId =
    "objc3c.lowering.owner.toolchain-runtime-ga.v1";
inline constexpr const char *kObjc3LoweringOwnerFinalReadinessGateContractId =
    "objc3c.lowering.owner.final-readiness-gate.v1";

inline constexpr const char *kObjc3LoweringOwnerTypedSemaHandoff =
    "native.frontend.sema.typed-handoff";
inline constexpr const char *kObjc3LoweringOwnerRuntimeStability =
    "native.lower.runtime-stability-invariant";
inline constexpr const char *kObjc3LoweringOwnerPipelinePassGraph =
    "native.lower.pipeline-pass-graph";
inline constexpr const char *kObjc3LoweringOwnerPassGraphCoreFeature =
    "native.lower.pass-graph-core-feature";
inline constexpr const char *kObjc3LoweringOwnerIREmissionCompleteness =
    "native.lower.ir-emission-completeness";
inline constexpr const char *kObjc3LoweringOwnerDiagnosticsSurfacing =
    "native.lower.runtime-diagnostics-surfacing";
inline constexpr const char *kObjc3LoweringOwnerToolchainGa =
    "native.lower.toolchain-runtime-ga";
inline constexpr const char *kObjc3LoweringOwnerFinalReadinessGate =
    "native.lower.final-readiness-gate";

struct Objc3LoweringOwnerContractRecord {
  std::string record_contract_id = kObjc3LoweringOwnerContractRecordId;
  std::string surface_contract_id;
  std::string owner_identity;
  std::string producer_replay_key;
  std::string consumer_replay_key;
  std::string artifact_publication_key;
  std::size_t required_edge_count = 0;
  std::size_t satisfied_edge_count = 0;
  bool producer_ready = false;
  bool consumer_ready = false;
  bool replay_key_deterministic = false;
  bool artifact_publication_ready = false;
  bool fail_closed = false;
  std::string failure_reason;
};

inline const char *Objc3LoweringOwnerBoolToken(bool value) {
  return value ? "true" : "false";
}

inline std::size_t Objc3LoweringOwnerCountReady(bool a, bool b, bool c,
                                                bool d, bool e) {
  return (a ? 1u : 0u) + (b ? 1u : 0u) + (c ? 1u : 0u) +
         (d ? 1u : 0u) + (e ? 1u : 0u);
}

inline bool IsReadyObjc3LoweringOwnerContractRecord(
    const Objc3LoweringOwnerContractRecord &record) {
  return record.record_contract_id == kObjc3LoweringOwnerContractRecordId &&
         !record.surface_contract_id.empty() && !record.owner_identity.empty() &&
         record.owner_identity.rfind("native.", 0) == 0 &&
         !record.producer_replay_key.empty() &&
         !record.consumer_replay_key.empty() &&
         !record.artifact_publication_key.empty() &&
         record.required_edge_count > 0 &&
         record.satisfied_edge_count == record.required_edge_count &&
         record.producer_ready && record.consumer_ready &&
         record.replay_key_deterministic &&
         record.artifact_publication_ready && record.fail_closed &&
         record.failure_reason.empty();
}

inline Objc3LoweringOwnerContractRecord BuildObjc3LoweringOwnerContractRecord(
    const std::string &surface_contract_id, const std::string &owner_identity,
    const std::string &producer_replay_key,
    const std::string &consumer_replay_key,
    const std::string &artifact_publication_key, bool producer_ready,
    bool consumer_ready, bool replay_key_deterministic,
    bool artifact_publication_ready, bool fail_closed,
    const std::string &source_failure_reason = std::string()) {
  Objc3LoweringOwnerContractRecord record;
  record.surface_contract_id = surface_contract_id;
  record.owner_identity = owner_identity;
  record.producer_replay_key = producer_replay_key;
  record.consumer_replay_key = consumer_replay_key;
  record.artifact_publication_key = artifact_publication_key;
  record.required_edge_count = 5;
  record.satisfied_edge_count = Objc3LoweringOwnerCountReady(
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed);
  record.producer_ready = producer_ready;
  record.consumer_ready = consumer_ready;
  record.replay_key_deterministic = replay_key_deterministic;
  record.artifact_publication_ready = artifact_publication_ready;
  record.fail_closed = fail_closed;

  if (!source_failure_reason.empty()) {
    record.failure_reason = source_failure_reason;
  } else if (record.surface_contract_id.empty()) {
    record.failure_reason = "lowering owner contract surface id missing";
  } else if (record.owner_identity.empty()) {
    record.failure_reason = "lowering owner identity missing";
  } else if (record.owner_identity.rfind("native.", 0) != 0) {
    record.failure_reason = "lowering owner identity is not native-owned";
  } else if (record.producer_replay_key.empty()) {
    record.failure_reason = "lowering owner producer replay key missing";
  } else if (record.consumer_replay_key.empty()) {
    record.failure_reason = "lowering owner consumer replay key missing";
  } else if (record.artifact_publication_key.empty()) {
    record.failure_reason = "lowering owner artifact publication key missing";
  } else if (!record.producer_ready) {
    record.failure_reason = "lowering owner producer edge not ready";
  } else if (!record.consumer_ready) {
    record.failure_reason = "lowering owner consumer edge not ready";
  } else if (!record.replay_key_deterministic) {
    record.failure_reason = "lowering owner replay key is not deterministic";
  } else if (!record.artifact_publication_ready) {
    record.failure_reason =
        "lowering owner artifact publication edge not ready";
  } else if (!record.fail_closed) {
    record.failure_reason = "lowering owner contract is not fail-closed";
  }

  return record;
}

inline std::string Objc3LoweringOwnerContractReplayKey(
    const Objc3LoweringOwnerContractRecord &record) {
  return "record_contract=" + record.record_contract_id +
         ";surface_contract=" + record.surface_contract_id +
         ";owner=" + record.owner_identity +
         ";producer=" + record.producer_replay_key +
         ";consumer=" + record.consumer_replay_key +
         ";artifact=" + record.artifact_publication_key +
         ";required_edges=" + std::to_string(record.required_edge_count) +
         ";satisfied_edges=" + std::to_string(record.satisfied_edge_count) +
         ";producer_ready=" +
         Objc3LoweringOwnerBoolToken(record.producer_ready) +
         ";consumer_ready=" +
         Objc3LoweringOwnerBoolToken(record.consumer_ready) +
         ";replay_key_deterministic=" +
         Objc3LoweringOwnerBoolToken(record.replay_key_deterministic) +
         ";artifact_publication_ready=" +
         Objc3LoweringOwnerBoolToken(record.artifact_publication_ready) +
         ";fail_closed=" + Objc3LoweringOwnerBoolToken(record.fail_closed) +
         (record.failure_reason.empty()
              ? std::string()
              : ";failure=" + record.failure_reason);
}
