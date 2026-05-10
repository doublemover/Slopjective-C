#pragma once

#include "pipeline/objc3_typed_sema_to_lowering_contract_surface_closeout.h"
#include "pipeline/objc3_typed_sema_to_lowering_contract_surface_failure.h"

inline Objc3TypedSemaToLoweringContractSurface BuildObjc3TypedSemaToLoweringContractSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3TypedSemaToLoweringContractSurface surface;
  const Objc3TypedSemaToLoweringContractSurfaceFoundationState foundation =
      PopulateObjc3TypedSemaToLoweringContractSurfaceFoundation(
          surface,
          pipeline_result,
          options);
  const Objc3TypedSemaToLoweringContractSurfaceStageState stages =
      PopulateObjc3TypedSemaToLoweringContractSurfaceStages(surface);

  FinalizeObjc3TypedSemaToLoweringContractSurfaceReadiness(
      surface,
      foundation,
      stages);

  if (!surface.ready_for_lowering && surface.failure_reason.empty()) {
    AssignObjc3TypedSemaToLoweringContractSurfaceFailureReason(
        surface,
        foundation.semantic_type_metadata_handoff_failure_reason);
  }

  return surface;
}

inline bool IsObjc3TypedSemaToLoweringContractSurfaceReady(
    const Objc3TypedSemaToLoweringContractSurface &surface,
    std::string &failure_reason) {
  if (surface.ready_for_lowering) {
    failure_reason.clear();
    return true;
  }
  failure_reason = surface.failure_reason.empty() ? "typed sema-to-lowering readiness failed"
                                                  : surface.failure_reason;
  return false;
}
