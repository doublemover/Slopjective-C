#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_artifact_dispatch_contract_snapshots.h"
#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactSemanticLoweringPlan {
  Objc3DispatchDispatchControlLoweringContract
      dispatch_dispatch_control_lowering_contract;
  objc3::artifacts::frontend::Objc3DispatchControlLoweringSnapshot
      dispatch_dispatch_control_lowering_snapshot;
  std::string dispatch_dispatch_control_lowering_replay_key;

  Objc3MetaprogrammingExpansionLoweringContract
      metaprogramming_expansion_lowering_contract;
  std::string metaprogramming_expansion_lowering_replay_key;
  std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
      metaprogramming_derived_method_bundles;
  std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
      metaprogramming_macro_artifact_bundles;
  std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
      metaprogramming_property_behavior_artifact_bundles;
  Objc3MetaprogrammingSynthesizedArtifactEmissionContract
      metaprogramming_synthesized_artifact_emission_contract;
  std::string metaprogramming_synthesized_artifact_emission_replay_key;

  Objc3OwnershipSystemExtensionLoweringContract
      ownership_system_extension_lowering_contract;
  std::string ownership_system_extension_lowering_replay_key;
  std::string ownership_borrowed_retainable_abi_completion_replay_key;

  Objc3AsyncContinuationLoweringContract
      concurrency_async_continuation_lowering_contract;
  Objc3AwaitLoweringSuspensionStateLoweringContract
      concurrency_await_lowering_suspension_state_lowering_contract;
  std::string concurrency_async_continuation_lowering_replay_key;
  std::string concurrency_await_lowering_suspension_state_lowering_replay_key;
  Objc3ActorIsolationSendabilityLoweringContract
      concurrency_actor_isolation_sendability_lowering_contract;
  std::string concurrency_actor_isolation_sendability_lowering_replay_key;
  Objc3ActorLoweringMetadataContract concurrency_actor_lowering_metadata_contract;
  std::string concurrency_actor_lowering_metadata_replay_key;
  Objc3TaskRuntimeInteropCancellationLoweringContract
      concurrency_task_runtime_interop_cancellation_lowering_contract;
  std::string
      concurrency_task_runtime_interop_cancellation_lowering_replay_key;
  Objc3ConcurrencyReplayRaceGuardLoweringContract
      concurrency_concurrency_replay_race_guard_lowering_contract;
  std::string concurrency_concurrency_replay_race_guard_lowering_replay_key;

  objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure
      post_pipeline_failure;
};

Objc3FrontendArtifactSemanticLoweringPlan
BuildObjc3FrontendArtifactSemanticLoweringPlan(
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result);
