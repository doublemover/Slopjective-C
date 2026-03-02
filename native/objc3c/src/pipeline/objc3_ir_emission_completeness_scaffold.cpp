#include "pipeline/objc3_ir_emission_completeness_scaffold.h"

#include <sstream>
#include <string>

std::string BuildObjc3IREmissionCompletenessScaffoldKey(
    const Objc3IREmissionCompletenessScaffold &scaffold) {
  std::ostringstream key;
  key << "ir-emission-completeness-scaffold:v1:"
      << "pass-graph-scaffold-ready="
      << (scaffold.pass_graph_scaffold_ready ? "true" : "false")
      << ";core-feature-ready="
      << (scaffold.core_feature_ready ? "true" : "false")
      << ";expansion-ready=" << (scaffold.expansion_ready ? "true" : "false")
      << ";edge-case-compatibility-ready="
      << (scaffold.edge_case_compatibility_ready ? "true" : "false")
      << ";metadata-transport-ready="
      << (scaffold.metadata_transport_ready ? "true" : "false")
      << ";modular-split-ready="
      << (scaffold.modular_split_ready ? "true" : "false")
      << ";pass-graph-key=" << scaffold.pass_graph_key
      << ";core-feature-key=" << scaffold.core_feature_key
      << ";expansion-key=" << scaffold.expansion_key
      << ";edge-case-compatibility-key=" << scaffold.edge_case_compatibility_key;
  return key.str();
}

Objc3IREmissionCompletenessScaffold BuildObjc3IREmissionCompletenessScaffold(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3IREmissionCompletenessScaffold scaffold;
  const Objc3LoweringPipelinePassGraphScaffold &pass_graph =
      pipeline_result.lowering_pipeline_pass_graph_scaffold;
  const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface;

  scaffold.pass_graph_scaffold_ready = pass_graph.pass_graph_ready;
  scaffold.core_feature_ready = surface.core_feature_ready;
  scaffold.expansion_ready = surface.expansion_ready;
  scaffold.edge_case_compatibility_ready = surface.edge_case_compatibility_ready;
  scaffold.pass_graph_key = pass_graph.pass_graph_key;
  scaffold.core_feature_key = surface.core_feature_key;
  scaffold.expansion_key = surface.expansion_key;
  scaffold.edge_case_compatibility_key = surface.edge_case_compatibility_key;
  scaffold.metadata_transport_ready =
      !scaffold.pass_graph_key.empty() && !scaffold.core_feature_key.empty() &&
      !scaffold.expansion_key.empty() &&
      !scaffold.edge_case_compatibility_key.empty();
  scaffold.modular_split_ready =
      scaffold.pass_graph_scaffold_ready && scaffold.core_feature_ready &&
      scaffold.expansion_ready && scaffold.edge_case_compatibility_ready &&
      scaffold.metadata_transport_ready;
  scaffold.scaffold_key = BuildObjc3IREmissionCompletenessScaffoldKey(scaffold);

  if (scaffold.modular_split_ready) {
    return scaffold;
  }

  if (!scaffold.pass_graph_scaffold_ready) {
    scaffold.failure_reason = "pass-graph scaffold is not ready";
  } else if (!scaffold.core_feature_ready) {
    scaffold.failure_reason = "pass-graph core feature is not ready";
  } else if (!scaffold.expansion_ready) {
    scaffold.failure_reason = "pass-graph expansion is not ready";
  } else if (!scaffold.edge_case_compatibility_ready) {
    scaffold.failure_reason = "pass-graph edge-case compatibility is not ready";
  } else if (!scaffold.metadata_transport_ready) {
    scaffold.failure_reason = "IR emission completeness metadata transport is not ready";
  } else {
    scaffold.failure_reason = "IR emission completeness modular split scaffold is not ready";
  }
  return scaffold;
}

bool IsObjc3IREmissionCompletenessScaffoldReady(
    const Objc3IREmissionCompletenessScaffold &scaffold,
    std::string &reason) {
  if (scaffold.modular_split_ready) {
    reason.clear();
    return true;
  }
  reason = scaffold.failure_reason.empty()
               ? "IR emission completeness modular split scaffold is not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3IREmissionCompletenessCoreFeatureReady(
    const Objc3IREmissionCompletenessScaffold &scaffold,
    std::string &reason) {
  if (scaffold.core_feature_ready) {
    reason.clear();
    return true;
  }
  reason = scaffold.failure_reason.empty()
               ? "pass-graph core feature is not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3IREmissionCompletenessExpansionReady(
    const Objc3IREmissionCompletenessScaffold &scaffold,
    std::string &reason) {
  if (scaffold.expansion_ready) {
    reason.clear();
    return true;
  }
  reason = scaffold.failure_reason.empty()
               ? "pass-graph expansion is not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3IREmissionCompletenessEdgeCaseCompatibilityReady(
    const Objc3IREmissionCompletenessScaffold &scaffold,
    std::string &reason) {
  if (scaffold.edge_case_compatibility_ready) {
    reason.clear();
    return true;
  }
  reason = scaffold.failure_reason.empty()
               ? "pass-graph edge-case compatibility is not ready"
               : scaffold.failure_reason;
  return false;
}
