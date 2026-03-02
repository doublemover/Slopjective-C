#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

struct Objc3IREmissionCoreFeatureImplementationSurface {
  bool modular_split_ready = false;
  bool metadata_transport_ready = false;
  bool pass_graph_core_feature_ready = false;
  bool pass_graph_expansion_ready = false;
  bool pass_graph_edge_case_compatibility_ready = false;
  bool pass_graph_edge_case_robustness_ready = false;
  bool runtime_boundary_handoff_ready = false;
  bool direct_ir_entrypoint_ready = false;
  bool expansion_metadata_transport_ready = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_artifact_edge_case_robustness_consistent = false;
  bool edge_case_expansion_consistent = false;
  bool parse_artifact_edge_case_robustness_ready = false;
  bool parse_artifact_replay_key_deterministic = false;
  bool edge_case_compatibility_key_transport_ready = false;
  bool edge_case_robustness_key_transport_ready = false;
  bool core_feature_impl_ready = false;
  bool core_feature_expansion_ready = false;
  bool core_feature_edge_case_compatibility_ready = false;
  bool core_feature_edge_case_robustness_ready = false;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string expansion_key;
  std::string pass_graph_edge_case_compatibility_key;
  std::string pass_graph_edge_case_robustness_key;
  std::string compatibility_handoff_key;
  std::string parse_artifact_edge_case_expansion_key;
  std::string parse_artifact_edge_robustness_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string failure_reason;
  std::string expansion_failure_reason;
  std::string edge_case_compatibility_failure_reason;
  std::string edge_case_robustness_failure_reason;
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

inline std::string BuildObjc3IREmissionCoreFeatureExpansionKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-expansion:v1:"
      << "core-feature-impl-ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";pass-graph-expansion-ready="
      << (surface.pass_graph_expansion_ready ? "true" : "false")
      << ";runtime-boundary-handoff-ready="
      << (surface.runtime_boundary_handoff_ready ? "true" : "false")
      << ";direct-ir-entrypoint-ready="
      << (surface.direct_ir_entrypoint_ready ? "true" : "false")
      << ";expansion-metadata-transport-ready="
      << (surface.expansion_metadata_transport_ready ? "true" : "false")
      << ";core-feature-expansion-ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";core-feature-key=" << surface.core_feature_key;
  return key.str();
}

inline std::string BuildObjc3IREmissionCoreFeatureEdgeCaseCompatibilityKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-edge-case-compatibility:v1:"
      << "core-feature-expansion-ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";pass-graph-edge-case-compatibility-ready="
      << (surface.pass_graph_edge_case_compatibility_ready ? "true" : "false")
      << ";compatibility-handoff-consistent="
      << (surface.compatibility_handoff_consistent ? "true" : "false")
      << ";language-version-pragma-coordinate-order-consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true"
                                                                      : "false")
      << ";parse-artifact-edge-case-robustness-consistent="
      << (surface.parse_artifact_edge_case_robustness_consistent ? "true"
                                                                 : "false")
      << ";parse-artifact-replay-key-deterministic="
      << (surface.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";edge-case-compatibility-key-transport-ready="
      << (surface.edge_case_compatibility_key_transport_ready ? "true" : "false")
      << ";edge-case-compatibility-ready="
      << (surface.core_feature_edge_case_compatibility_ready ? "true" : "false")
      << ";pass-graph-edge-case-compatibility-key="
      << surface.pass_graph_edge_case_compatibility_key
      << ";compatibility-handoff-key=" << surface.compatibility_handoff_key
      << ";parse-artifact-edge-robustness-key="
      << surface.parse_artifact_edge_robustness_key
      << ";expansion-key=" << surface.expansion_key;
  return key.str();
}

inline std::string BuildObjc3IREmissionCoreFeatureEdgeCaseRobustnessKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-edge-case-robustness:v1:"
      << "edge-case-compatibility-ready="
      << (surface.core_feature_edge_case_compatibility_ready ? "true" : "false")
      << ";pass-graph-edge-case-robustness-ready="
      << (surface.pass_graph_edge_case_robustness_ready ? "true" : "false")
      << ";edge-case-expansion-consistent="
      << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";parse-artifact-edge-case-robustness-ready="
      << (surface.parse_artifact_edge_case_robustness_ready ? "true" : "false")
      << ";edge-case-robustness-key-transport-ready="
      << (surface.edge_case_robustness_key_transport_ready ? "true" : "false")
      << ";edge-case-robustness-ready="
      << (surface.core_feature_edge_case_robustness_ready ? "true" : "false")
      << ";pass-graph-edge-case-robustness-key="
      << surface.pass_graph_edge_case_robustness_key
      << ";parse-artifact-edge-case-expansion-key="
      << surface.parse_artifact_edge_case_expansion_key
      << ";parse-artifact-edge-robustness-key="
      << surface.parse_artifact_edge_robustness_key
      << ";edge-case-compatibility-key=" << surface.edge_case_compatibility_key;
  return key.str();
}

inline Objc3IREmissionCoreFeatureImplementationSurface
BuildObjc3IREmissionCoreFeatureImplementationSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3IREmissionCoreFeatureImplementationSurface surface;
  const Objc3IREmissionCompletenessScaffold &scaffold =
      pipeline_result.ir_emission_completeness_scaffold;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3TypedSemaToLoweringContractSurface &typed_surface =
      pipeline_result.typed_sema_to_lowering_contract_surface;

  surface.modular_split_ready = scaffold.modular_split_ready;
  surface.metadata_transport_ready = scaffold.metadata_transport_ready;
  surface.pass_graph_core_feature_ready = scaffold.core_feature_ready;
  surface.pass_graph_expansion_ready = scaffold.expansion_ready;
  surface.pass_graph_edge_case_compatibility_ready =
      scaffold.edge_case_compatibility_ready;
  surface.pass_graph_edge_case_robustness_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_ready;
  surface.runtime_boundary_handoff_ready =
      typed_surface.lowering_boundary_ready &&
      !typed_surface.lowering_boundary_replay_key.empty();
  surface.direct_ir_entrypoint_ready =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .ir_emission_entrypoint_ready;
  surface.compatibility_handoff_consistent =
      parse_surface.compatibility_handoff_consistent;
  surface.language_version_pragma_coordinate_order_consistent =
      parse_surface.language_version_pragma_coordinate_order_consistent;
  surface.parse_artifact_edge_case_robustness_consistent =
      parse_surface.parse_artifact_edge_case_robustness_consistent;
  surface.edge_case_expansion_consistent =
      parse_surface.long_tail_grammar_edge_case_expansion_consistent;
  surface.parse_artifact_edge_case_robustness_ready =
      parse_surface.long_tail_grammar_edge_case_robustness_ready;
  surface.parse_artifact_replay_key_deterministic =
      parse_surface.parse_artifact_replay_key_deterministic;
  surface.scaffold_key = scaffold.scaffold_key;
  surface.pass_graph_edge_case_compatibility_key =
      scaffold.edge_case_compatibility_key;
  surface.pass_graph_edge_case_robustness_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_key;
  surface.compatibility_handoff_key = parse_surface.compatibility_handoff_key;
  surface.parse_artifact_edge_case_expansion_key =
      parse_surface.long_tail_grammar_expansion_key;
  surface.parse_artifact_edge_robustness_key =
      parse_surface.parse_artifact_edge_robustness_key;

  surface.core_feature_impl_ready =
      surface.modular_split_ready && surface.metadata_transport_ready &&
      surface.pass_graph_core_feature_ready &&
      surface.runtime_boundary_handoff_ready &&
      surface.direct_ir_entrypoint_ready && !surface.scaffold_key.empty();
  surface.core_feature_key =
      BuildObjc3IREmissionCoreFeatureImplementationKey(surface);
  surface.expansion_metadata_transport_ready = !scaffold.expansion_key.empty();
  surface.core_feature_expansion_ready =
      surface.core_feature_impl_ready && surface.pass_graph_expansion_ready &&
      surface.expansion_metadata_transport_ready &&
      surface.runtime_boundary_handoff_ready &&
      surface.direct_ir_entrypoint_ready;
  surface.expansion_key = BuildObjc3IREmissionCoreFeatureExpansionKey(surface);
  surface.edge_case_compatibility_key_transport_ready =
      !surface.pass_graph_edge_case_compatibility_key.empty() &&
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty();
  surface.core_feature_edge_case_compatibility_ready =
      surface.core_feature_expansion_ready &&
      surface.pass_graph_edge_case_compatibility_ready &&
      surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.edge_case_compatibility_key_transport_ready;
  surface.edge_case_compatibility_key =
      BuildObjc3IREmissionCoreFeatureEdgeCaseCompatibilityKey(surface);
  surface.edge_case_robustness_key_transport_ready =
      !surface.pass_graph_edge_case_robustness_key.empty() &&
      !surface.parse_artifact_edge_case_expansion_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.edge_case_compatibility_key.empty();
  surface.core_feature_edge_case_robustness_ready =
      surface.core_feature_edge_case_compatibility_ready &&
      surface.pass_graph_edge_case_robustness_ready &&
      surface.edge_case_expansion_consistent &&
      surface.parse_artifact_edge_case_robustness_ready &&
      surface.edge_case_robustness_key_transport_ready;
  surface.edge_case_robustness_key =
      BuildObjc3IREmissionCoreFeatureEdgeCaseRobustnessKey(surface);

  if (surface.core_feature_expansion_ready) {
    surface.expansion_failure_reason.clear();
  } else if (!surface.core_feature_impl_ready) {
    surface.expansion_failure_reason =
        "IR emission core feature implementation is not ready";
  } else if (!surface.pass_graph_expansion_ready) {
    surface.expansion_failure_reason = "pass-graph expansion is not ready";
  } else if (!surface.expansion_metadata_transport_ready) {
    surface.expansion_failure_reason =
        "IR emission core feature expansion metadata transport is not ready";
  } else if (!surface.runtime_boundary_handoff_ready) {
    surface.expansion_failure_reason =
        "runtime boundary handoff replay surface is not ready";
  } else if (!surface.direct_ir_entrypoint_ready) {
    surface.expansion_failure_reason = "direct IR entrypoint is not ready";
  } else {
    surface.expansion_failure_reason =
        "IR emission core feature expansion surface is not ready";
  }

  if (surface.core_feature_edge_case_compatibility_ready) {
    surface.edge_case_compatibility_failure_reason.clear();
  } else if (!surface.core_feature_expansion_ready) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature expansion is not ready";
  } else if (!surface.pass_graph_edge_case_compatibility_ready) {
    surface.edge_case_compatibility_failure_reason =
        "pass-graph edge-case compatibility is not ready";
  } else if (!surface.compatibility_handoff_consistent) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature compatibility handoff is inconsistent";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature language version pragma coordinate order is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_consistent) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature parse artifact edge-case robustness is inconsistent";
  } else if (!surface.parse_artifact_replay_key_deterministic) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature parse artifact replay key is not deterministic";
  } else if (!surface.edge_case_compatibility_key_transport_ready) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature edge-case compatibility key transport is not ready";
  } else {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature edge-case compatibility surface is not ready";
  }

  if (surface.core_feature_edge_case_robustness_ready) {
    surface.edge_case_robustness_failure_reason.clear();
  } else if (!surface.core_feature_edge_case_compatibility_ready) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case compatibility is not ready";
  } else if (!surface.pass_graph_edge_case_robustness_ready) {
    surface.edge_case_robustness_failure_reason =
        "pass-graph edge-case robustness is not ready";
  } else if (!surface.edge_case_expansion_consistent) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case expansion is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_ready) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature parse artifact edge-case robustness is not ready";
  } else if (!surface.edge_case_robustness_key_transport_ready) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case robustness key transport is not ready";
  } else {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case robustness surface is not ready";
  }

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

inline bool IsObjc3IREmissionCoreFeatureExpansionReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_expansion_ready) {
    reason.clear();
    return true;
  }
  reason =
      surface.expansion_failure_reason.empty()
          ? "IR emission core feature expansion surface is not ready"
          : surface.expansion_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_edge_case_compatibility_ready &&
      surface.edge_case_compatibility_key_transport_ready &&
      !surface.edge_case_compatibility_key.empty()) {
    reason.clear();
    return true;
  }
  reason =
      surface.edge_case_compatibility_failure_reason.empty()
          ? "IR emission core feature edge-case compatibility surface is not ready"
          : surface.edge_case_compatibility_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_edge_case_robustness_ready &&
      surface.edge_case_robustness_key_transport_ready &&
      !surface.edge_case_robustness_key.empty()) {
    reason.clear();
    return true;
  }
  reason = surface.edge_case_robustness_failure_reason.empty()
               ? "IR emission core feature edge-case robustness surface is not ready"
               : surface.edge_case_robustness_failure_reason;
  return false;
}
