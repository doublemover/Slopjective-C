#pragma once

#include <cstddef>
#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline constexpr std::size_t kObjc3TypedSemaToLoweringCoreFeatureCaseCount = 6u;

inline std::string BuildObjc3TypedSemaToLoweringContractHandoffKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  std::ostringstream key;
  key << "typed-sema-lowering:v1:"
      << "semantic_surface=" << (surface.semantic_integration_surface_built ? "true" : "false")
      << ";type_metadata=" << (surface.semantic_type_metadata_handoff_deterministic ? "true" : "false")
      << ";sema_parity=" << (surface.sema_parity_surface_ready ? "true" : "false")
      << ";sema_parity_deterministic=" << (surface.sema_parity_surface_deterministic ? "true" : "false")
      << ";object_pointer=" << (surface.object_pointer_type_handoff_deterministic ? "true" : "false")
      << ";symbol_graph=" << (surface.symbol_graph_handoff_deterministic ? "true" : "false")
      << ";scope_resolution=" << (surface.scope_resolution_handoff_deterministic ? "true" : "false")
      << ";semantic_handoff_consistent=" << (surface.semantic_handoff_consistent ? "true" : "false")
      << ";semantic_handoff_deterministic=" << (surface.semantic_handoff_deterministic ? "true" : "false")
      << ";runtime_dispatch=" << (surface.runtime_dispatch_contract_consistent ? "true" : "false")
      << ";core_feature_passed_case_count=" << surface.typed_core_feature_passed_case_count
      << ";core_feature_failed_case_count=" << surface.typed_core_feature_failed_case_count
      << ";core_feature_consistent=" << (surface.typed_core_feature_consistent ? "true" : "false")
      << ";lowering_boundary=" << (surface.lowering_boundary_ready ? "true" : "false")
      << ";ready_for_lowering=" << (surface.ready_for_lowering ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3TypedSemaToLoweringCoreFeatureKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  std::ostringstream key;
  key << "typed-sema-lowering-core:v1:"
      << "case_count=" << surface.typed_core_feature_case_count
      << ";passed_case_count=" << surface.typed_core_feature_passed_case_count
      << ";failed_case_count=" << surface.typed_core_feature_failed_case_count
      << ";semantic_handoff_consistent=" << (surface.semantic_handoff_consistent ? "true" : "false")
      << ";semantic_handoff_deterministic=" << (surface.semantic_handoff_deterministic ? "true" : "false")
      << ";typed_handoff_key_deterministic=" << (surface.typed_handoff_key_deterministic ? "true" : "false")
      << ";runtime_dispatch_contract_consistent="
      << (surface.runtime_dispatch_contract_consistent ? "true" : "false")
      << ";lowering_boundary_ready=" << (surface.lowering_boundary_ready ? "true" : "false")
      << ";consistent=" << (surface.typed_core_feature_consistent ? "true" : "false");
  return key.str();
}

inline Objc3TypedSemaToLoweringContractSurface BuildObjc3TypedSemaToLoweringContractSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3TypedSemaToLoweringContractSurface surface;
  surface.semantic_integration_surface_built = pipeline_result.integration_surface.built;
  surface.semantic_type_metadata_handoff_deterministic =
      IsDeterministicSemanticTypeMetadataHandoff(pipeline_result.sema_type_metadata_handoff);
  surface.sema_parity_surface_ready =
      IsReadyObjc3SemaParityContractSurface(pipeline_result.sema_parity_surface);
  surface.sema_parity_surface_deterministic =
      pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics &&
      pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff;
  surface.object_pointer_type_handoff_deterministic =
      pipeline_result.object_pointer_nullability_generics_summary
          .deterministic_object_pointer_nullability_generics_handoff;
  surface.symbol_graph_handoff_deterministic =
      pipeline_result.symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff;
  surface.scope_resolution_handoff_deterministic =
      pipeline_result.symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;

  Objc3LoweringIRBoundary lowering_boundary;
  std::string lowering_error;
  if (TryBuildObjc3LoweringIRBoundary(options.lowering, lowering_boundary, lowering_error)) {
    surface.lowering_boundary_ready = true;
    surface.lowering_boundary_replay_key = Objc3LoweringIRBoundaryReplayKey(lowering_boundary);
  } else {
    surface.lowering_boundary_ready = false;
    surface.failure_reason = "invalid lowering contract: " + lowering_error;
  }

  surface.runtime_dispatch_contract_consistent =
      surface.lowering_boundary_ready &&
      lowering_boundary.runtime_dispatch_arg_slots >= kObjc3RuntimeDispatchDefaultArgs &&
      lowering_boundary.runtime_dispatch_arg_slots <= kObjc3RuntimeDispatchMaxArgs &&
      !lowering_boundary.runtime_dispatch_symbol.empty() &&
      lowering_boundary.selector_global_ordering == kObjc3SelectorGlobalOrdering;
  surface.semantic_handoff_consistent =
      surface.semantic_integration_surface_built &&
      surface.sema_parity_surface_ready;
  surface.semantic_handoff_deterministic =
      surface.semantic_type_metadata_handoff_deterministic &&
      surface.sema_parity_surface_deterministic &&
      surface.object_pointer_type_handoff_deterministic &&
      surface.symbol_graph_handoff_deterministic &&
      surface.scope_resolution_handoff_deterministic;

  const bool semantic_parity_feature_case_passed =
      surface.sema_parity_surface_ready &&
      surface.sema_parity_surface_deterministic;
  const bool symbol_graph_scope_resolution_feature_case_passed =
      surface.symbol_graph_handoff_deterministic &&
      surface.scope_resolution_handoff_deterministic;
  const bool lowering_runtime_boundary_feature_case_passed =
      surface.runtime_dispatch_contract_consistent &&
      surface.lowering_boundary_ready;
  surface.typed_core_feature_case_count = kObjc3TypedSemaToLoweringCoreFeatureCaseCount;
  surface.typed_core_feature_passed_case_count =
      static_cast<std::size_t>(surface.semantic_integration_surface_built) +
      static_cast<std::size_t>(surface.semantic_type_metadata_handoff_deterministic) +
      static_cast<std::size_t>(semantic_parity_feature_case_passed) +
      static_cast<std::size_t>(surface.object_pointer_type_handoff_deterministic) +
      static_cast<std::size_t>(symbol_graph_scope_resolution_feature_case_passed) +
      static_cast<std::size_t>(lowering_runtime_boundary_feature_case_passed);
  surface.typed_core_feature_failed_case_count =
      surface.typed_core_feature_case_count >= surface.typed_core_feature_passed_case_count
          ? (surface.typed_core_feature_case_count - surface.typed_core_feature_passed_case_count)
          : surface.typed_core_feature_case_count;
  const bool typed_core_feature_case_accounting_consistent =
      surface.typed_core_feature_case_count == kObjc3TypedSemaToLoweringCoreFeatureCaseCount &&
      surface.typed_core_feature_case_count > 0 &&
      surface.typed_core_feature_passed_case_count <= surface.typed_core_feature_case_count &&
      surface.typed_core_feature_failed_case_count ==
          (surface.typed_core_feature_case_count - surface.typed_core_feature_passed_case_count);
  const bool typed_core_feature_cases_passed =
      surface.typed_core_feature_passed_case_count == surface.typed_core_feature_case_count &&
      surface.typed_core_feature_failed_case_count == 0;
  surface.typed_core_feature_consistent =
      typed_core_feature_case_accounting_consistent &&
      typed_core_feature_cases_passed &&
      surface.semantic_handoff_consistent &&
      surface.semantic_handoff_deterministic &&
      lowering_runtime_boundary_feature_case_passed;

  surface.ready_for_lowering = surface.typed_core_feature_consistent;
  surface.typed_handoff_key = BuildObjc3TypedSemaToLoweringContractHandoffKey(surface);
  surface.typed_handoff_key_deterministic =
      surface.semantic_handoff_deterministic &&
      surface.runtime_dispatch_contract_consistent &&
      surface.lowering_boundary_ready &&
      !surface.typed_handoff_key.empty();
  surface.typed_core_feature_consistent =
      surface.typed_core_feature_consistent &&
      surface.typed_handoff_key_deterministic;
  surface.typed_core_feature_key = BuildObjc3TypedSemaToLoweringCoreFeatureKey(surface);
  surface.ready_for_lowering = surface.typed_core_feature_consistent;

  if (surface.ready_for_lowering || !surface.failure_reason.empty()) {
    return surface;
  }

  if (!surface.semantic_integration_surface_built) {
    surface.failure_reason = "semantic integration surface not built";
  } else if (!surface.semantic_type_metadata_handoff_deterministic) {
    surface.failure_reason = "semantic type metadata handoff is not deterministic";
  } else if (!surface.sema_parity_surface_ready) {
    surface.failure_reason = "semantic parity surface is not ready";
  } else if (!surface.sema_parity_surface_deterministic) {
    surface.failure_reason = "semantic parity surface is not deterministic";
  } else if (!surface.object_pointer_type_handoff_deterministic) {
    surface.failure_reason = "object pointer/nullability handoff is not deterministic";
  } else if (!surface.symbol_graph_handoff_deterministic) {
    surface.failure_reason = "symbol graph handoff is not deterministic";
  } else if (!surface.scope_resolution_handoff_deterministic) {
    surface.failure_reason = "scope resolution handoff is not deterministic";
  } else if (!surface.semantic_handoff_consistent) {
    surface.failure_reason = "semantic handoff is inconsistent";
  } else if (!surface.semantic_handoff_deterministic) {
    surface.failure_reason = "semantic handoff is not deterministic";
  } else if (!surface.runtime_dispatch_contract_consistent) {
    surface.failure_reason = "runtime dispatch contract is inconsistent";
  } else if (!surface.lowering_boundary_ready) {
    surface.failure_reason = "lowering boundary is not ready";
  } else if (!surface.typed_handoff_key_deterministic) {
    surface.failure_reason = "typed handoff key is not deterministic";
  } else if (!surface.typed_core_feature_consistent) {
    surface.failure_reason = "typed sema-to-lowering core feature contract is inconsistent";
  } else {
    surface.failure_reason = "typed sema-to-lowering contract readiness failed";
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
