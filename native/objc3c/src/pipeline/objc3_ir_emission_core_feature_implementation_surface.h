#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

struct Objc3IREmissionCoreFeatureImplementationSurface {
  bool modular_split_ready = false;
  bool metadata_transport_ready = false;
  bool pass_graph_core_feature_ready = false;
  bool runtime_boundary_handoff_ready = false;
  bool direct_ir_entrypoint_ready = false;
  bool core_feature_impl_ready = false;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string failure_reason;
};

inline std::string BuildObjc3IREmissionCoreFeatureImplementationKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-implementation:v1:"
      << "modular-split-ready=" << (surface.modular_split_ready ? "true" : "false")
      << ";metadata-transport-ready="
      << (surface.metadata_transport_ready ? "true" : "false")
      << ";pass-graph-core-feature-ready="
      << (surface.pass_graph_core_feature_ready ? "true" : "false")
      << ";runtime-boundary-handoff-ready="
      << (surface.runtime_boundary_handoff_ready ? "true" : "false")
      << ";direct-ir-entrypoint-ready="
      << (surface.direct_ir_entrypoint_ready ? "true" : "false")
      << ";core-feature-impl-ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";scaffold-key=" << surface.scaffold_key;
  return key.str();
}

inline Objc3IREmissionCoreFeatureImplementationSurface
BuildObjc3IREmissionCoreFeatureImplementationSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3IREmissionCoreFeatureImplementationSurface surface;
  const Objc3IREmissionCompletenessScaffold &scaffold =
      pipeline_result.ir_emission_completeness_scaffold;
  const Objc3TypedSemaToLoweringContractSurface &typed_surface =
      pipeline_result.typed_sema_to_lowering_contract_surface;

  surface.modular_split_ready = scaffold.modular_split_ready;
  surface.metadata_transport_ready = scaffold.metadata_transport_ready;
  surface.pass_graph_core_feature_ready = scaffold.core_feature_ready;
  surface.runtime_boundary_handoff_ready =
      typed_surface.lowering_boundary_ready &&
      !typed_surface.lowering_boundary_replay_key.empty();
  surface.direct_ir_entrypoint_ready =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .ir_emission_entrypoint_ready;
  surface.scaffold_key = scaffold.scaffold_key;

  surface.core_feature_impl_ready =
      surface.modular_split_ready && surface.metadata_transport_ready &&
      surface.pass_graph_core_feature_ready &&
      surface.runtime_boundary_handoff_ready &&
      surface.direct_ir_entrypoint_ready && !surface.scaffold_key.empty();
  surface.core_feature_key =
      BuildObjc3IREmissionCoreFeatureImplementationKey(surface);

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.modular_split_ready) {
    surface.failure_reason =
        "IR emission completeness modular split scaffold is not ready";
  } else if (!surface.metadata_transport_ready) {
    surface.failure_reason =
        "IR emission completeness metadata transport is not ready";
  } else if (!surface.pass_graph_core_feature_ready) {
    surface.failure_reason = "pass-graph core feature is not ready";
  } else if (!surface.runtime_boundary_handoff_ready) {
    surface.failure_reason =
        "runtime boundary handoff replay surface is not ready";
  } else if (!surface.direct_ir_entrypoint_ready) {
    surface.failure_reason = "direct IR entrypoint is not ready";
  } else {
    surface.failure_reason =
        "IR emission core feature implementation surface is not ready";
  }
  return surface;
}

inline bool IsObjc3IREmissionCoreFeatureImplementationReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }
  reason =
      surface.failure_reason.empty()
          ? "IR emission core feature implementation surface is not ready"
          : surface.failure_reason;
  return false;
}
