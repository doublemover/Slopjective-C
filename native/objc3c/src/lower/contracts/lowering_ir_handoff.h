#pragma once

#include "lower/contracts/lowering_artifact_publication.h"
#include "lower/contracts/runtime_dispatch_strict_abi_lowering_contracts.h"

#include <cstddef>
#include <string>

struct Objc3LoweringIRHandoff {
  std::string lower_handoff_owner = kObjc3LoweringIRHandoffOwner;
  std::string ir_artifact_owner = kObjc3IRModuleArtifactOwner;
  std::string runtime_dispatch_owner = kObjc3RuntimeDispatchLoweringOwner;
  std::string runtime_dispatch_result_owner =
      kObjc3IRRuntimeDispatchResultOwner;
  std::string owner_model = kObjc3LoweringNoRetiredRouteOwnerModel;
  std::string runtime_dispatch_symbol =
      kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol;
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  Objc3LoweringArtifactPlan artifacts;
  bool lower_handoff_owner_ready = false;
  bool ir_artifact_owner_ready = false;
  bool runtime_dispatch_owner_ready = false;
  bool runtime_dispatch_result_owner_ready = false;
  bool artifact_publication_owner_ready = false;
  bool canonical_runtime_dispatch_symbol = false;
  bool fixed_runtime_dispatch_slots = false;
  bool requested_artifact_paths_owned = false;
  bool phase_handoff_explicit = false;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool ready = false;
  std::string failure_reason;
  std::string replay_key;
};

Objc3LoweringIRHandoff Objc3BuildLoweringIRHandoff(
    const Objc3LoweringArtifactPlan &artifacts,
    const std::string &runtime_dispatch_symbol =
        kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol,
    std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs);
bool Objc3LoweringIRHandoffIsReady(const Objc3LoweringIRHandoff &handoff);
std::string Objc3LoweringIRHandoffReplayKey(
    const Objc3LoweringIRHandoff &handoff);
