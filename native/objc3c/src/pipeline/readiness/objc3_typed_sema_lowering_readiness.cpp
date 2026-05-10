#include <string>

#include "pipeline/readiness/objc3_typed_sema_lowering_readiness.h"
#include "pipeline/readiness/objc3_typed_sema_lowering_readiness_private.h"

bool HasObjc3TypedSemaToLoweringCoreFeatureSurface(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return surface.typed_core_feature_case_count > 0 ||
         surface.typed_core_feature_expansion_case_count > 0 ||
         surface.typed_core_feature_edge_case_compatibility_ready ||
         !surface.typed_handoff_key.empty() ||
         !surface.typed_core_feature_key.empty() ||
         !surface.typed_core_feature_expansion_key.empty() ||
         !surface.typed_core_feature_edge_case_compatibility_key.empty() ||
         surface.ready_for_lowering ||
         !surface.failure_reason.empty();
}

Objc3TypedSemaToLoweringContractSurface ResolveObjc3TypedSemaToLoweringContractSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  return HasObjc3TypedSemaToLoweringCoreFeatureSurface(
             pipeline_result.typed_sema_to_lowering_contract_surface)
             ? pipeline_result.typed_sema_to_lowering_contract_surface
             : BuildObjc3TypedSemaToLoweringContractSurface(pipeline_result, options);
}

Objc3TypedSemaLoweringReadinessRecord BuildObjc3TypedSemaLoweringReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3TypedSemaLoweringReadinessRecord record;
  record.contract_surface =
      ResolveObjc3TypedSemaToLoweringContractSurface(pipeline_result, options);
  const Objc3TypedSemaToLoweringContractSurface &typed_sema_to_lowering_contract_surface =
      record.contract_surface;

  PopulateObjc3TypedSemaLoweringReadinessSurface(
      surface,
      pipeline_result,
      typed_sema_to_lowering_contract_surface);
  ResolveObjc3TypedSemaLoweringBoundaryReadiness(
      surface,
      typed_sema_to_lowering_contract_surface,
      options);

  const bool typed_core_feature_expansion_ready =
      IsObjc3TypedSemaCoreFeatureExpansionReady(surface);
  PopulateObjc3TypedSemaLoweringReadinessAlignments(
      record,
      surface,
      typed_sema_to_lowering_contract_surface);
  record.typed_core_feature_ready = IsObjc3TypedSemaCoreFeatureReady(
      surface,
      record,
      typed_core_feature_expansion_ready);
  record.sema_handoff_ready =
      typed_sema_to_lowering_contract_surface.ready_for_lowering &&
      record.typed_core_feature_ready;
  record.semantic_handoff_deterministic =
      IsObjc3TypedSemaSemanticHandoffDeterministic(surface);

  return record;
}
