#include "artifacts/objc3_frontend_artifact_semantic_summary_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_function_manifest.h"
#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"
#include "artifacts/objc3_frontend_error_semantic_artifacts.h"
#include "artifacts/objc3_frontend_type_system_semantic_artifacts.h"
#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"

namespace objc3::artifacts::frontend {

void WriteSemanticSummaryManifestSurfaces(
    std::ostream &manifest,
    const Objc3ErrorHandlingErrorSemanticModelSummary
        &error_handling_error_semantic_model_summary,
    const Objc3ErrorHandlingTryDoCatchSemanticSummary
        &error_handling_try_do_catch_semantic_summary,
    const Objc3ErrorHandlingErrorBridgeLegalitySummary
        &error_handling_error_bridge_legality_summary,
    const Objc3ControlFlowControlFlowSemanticModelSummary
        &control_flow_control_flow_semantic_model_summary,
    const Objc3TypeSystemTypeSemanticModelSummary
        &type_system_type_semantic_model_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary,
    const Objc3FrontendArtifactFunctionManifest &function_manifest) {
  manifest
      << ",\"objc_error_handling_error_semantic_model\":"
      << BuildErrorHandlingErrorSemanticModelSummaryJson(
             error_handling_error_semantic_model_summary)
      << ",\"objc_error_handling_try_do_catch_semantics\":"
      << BuildErrorHandlingTryDoCatchSemanticSummaryJson(
             error_handling_try_do_catch_semantic_summary)
      << ",\"objc_error_handling_error_bridge_legality\":"
      << BuildErrorHandlingErrorBridgeLegalitySummaryJson(
             error_handling_error_bridge_legality_summary)
      << ",\"objc_control_flow_control_flow_semantic_model\":"
      << BuildControlFlowControlFlowSemanticModelSummaryJson(
             control_flow_control_flow_semantic_model_summary)
      << ",\"objc_type_system_type_semantic_model\":"
      << BuildTypeSystemTypeSemanticModelSummaryJson(
             type_system_type_semantic_model_summary)
      << ",\"objc_symbol_graph_scope_resolution_surface\":{\"global_symbol_nodes\":"
      << symbol_graph_scope_resolution_summary.global_symbol_nodes
      << ",\"function_symbol_nodes\":"
      << symbol_graph_scope_resolution_summary.function_symbol_nodes
      << ",\"interface_symbol_nodes\":"
      << symbol_graph_scope_resolution_summary.interface_symbol_nodes
      << ",\"implementation_symbol_nodes\":"
      << symbol_graph_scope_resolution_summary.implementation_symbol_nodes
      << ",\"interface_property_symbol_nodes\":"
      << symbol_graph_scope_resolution_summary.interface_property_symbol_nodes
      << ",\"implementation_property_symbol_nodes\":"
      << symbol_graph_scope_resolution_summary
             .implementation_property_symbol_nodes
      << ",\"interface_method_symbol_nodes\":"
      << symbol_graph_scope_resolution_summary.interface_method_symbol_nodes
      << ",\"implementation_method_symbol_nodes\":"
      << symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes
      << ",\"top_level_scope_symbols\":"
      << symbol_graph_scope_resolution_summary.top_level_scope_symbols
      << ",\"nested_scope_symbols\":"
      << symbol_graph_scope_resolution_summary.nested_scope_symbols
      << ",\"scope_frames_total\":"
      << symbol_graph_scope_resolution_summary.scope_frames_total
      << ",\"implementation_interface_resolution_sites\":"
      << symbol_graph_scope_resolution_summary
             .implementation_interface_resolution_sites
      << ",\"implementation_interface_resolution_hits\":"
      << symbol_graph_scope_resolution_summary
             .implementation_interface_resolution_hits
      << ",\"implementation_interface_resolution_misses\":"
      << symbol_graph_scope_resolution_summary
             .implementation_interface_resolution_misses
      << ",\"method_resolution_sites\":"
      << symbol_graph_scope_resolution_summary.method_resolution_sites
      << ",\"method_resolution_hits\":"
      << symbol_graph_scope_resolution_summary.method_resolution_hits
      << ",\"method_resolution_misses\":"
      << symbol_graph_scope_resolution_summary.method_resolution_misses
      << ",\"deterministic_symbol_graph_handoff\":"
      << (symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff
              ? "true"
              : "false")
      << ",\"deterministic_scope_resolution_handoff\":"
      << (symbol_graph_scope_resolution_summary
                  .deterministic_scope_resolution_handoff
              ? "true"
              : "false")
      << ",\"deterministic_handoff_key\":\""
      << symbol_graph_scope_resolution_summary.deterministic_handoff_key
      << "\"}"
      << ",\"function_signature_surface\":{\"scalar_return_i32\":"
      << function_manifest.scalar_return_i32
      << ",\"scalar_return_bool\":"
      << function_manifest.scalar_return_bool
      << ",\"scalar_return_void\":"
      << function_manifest.scalar_return_void << ",\"scalar_param_i32\":"
      << function_manifest.scalar_param_i32 << ",\"scalar_param_bool\":"
      << function_manifest.scalar_param_bool << "}}\n";
}

}  // namespace objc3::artifacts::frontend
