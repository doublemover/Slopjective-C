#pragma once

#include <iosfwd>

struct Objc3ControlFlowControlFlowSemanticModelSummary;
struct Objc3ErrorHandlingErrorBridgeLegalitySummary;
struct Objc3ErrorHandlingErrorSemanticModelSummary;
struct Objc3ErrorHandlingTryDoCatchSemanticSummary;
struct Objc3FrontendArtifactFunctionManifest;
struct Objc3FrontendSymbolGraphScopeResolutionSummary;
struct Objc3TypeSystemTypeSemanticModelSummary;

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
    const Objc3FrontendArtifactFunctionManifest &function_manifest);

}  // namespace objc3::artifacts::frontend
