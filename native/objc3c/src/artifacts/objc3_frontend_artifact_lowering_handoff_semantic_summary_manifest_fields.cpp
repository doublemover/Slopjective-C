#include "artifacts/objc3_frontend_artifact_lowering_handoff_semantic_summary_manifest_fields.h"

#include <sstream>

#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"
#include "sema/model/frontend_linkage_summaries.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffSemanticSummaryManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_nullability_generics_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary) {
  manifest << ",\"deterministic_object_pointer_nullability_generics_handoff\":"
           << (object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff
                   ? "true"
                   : "false")
           << ",\"object_pointer_type_spellings\":"
           << object_pointer_nullability_generics_summary.object_pointer_type_spellings
           << ",\"pointer_declarator_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_entries
           << ",\"pointer_declarator_depth_total\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_depth_total
           << ",\"pointer_declarator_token_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_token_entries
           << ",\"nullability_suffix_entries\":"
           << object_pointer_nullability_generics_summary.nullability_suffix_entries
           << ",\"generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.generic_suffix_entries
           << ",\"terminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.terminated_generic_suffix_entries
           << ",\"unterminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries
           << ",\"symbol_graph_global_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.global_symbol_nodes
           << ",\"symbol_graph_function_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.function_symbol_nodes
           << ",\"symbol_graph_interface_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_symbol_nodes
           << ",\"symbol_graph_implementation_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_symbol_nodes
           << ",\"symbol_graph_interface_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_property_symbol_nodes
           << ",\"symbol_graph_implementation_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes
           << ",\"symbol_graph_interface_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_method_symbol_nodes
           << ",\"symbol_graph_implementation_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes
           << ",\"scope_resolution_top_level_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.top_level_scope_symbols
           << ",\"scope_resolution_nested_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.nested_scope_symbols
           << ",\"scope_resolution_scope_frames_total\":"
           << symbol_graph_scope_resolution_summary.scope_frames_total
           << ",\"scope_resolution_implementation_interface_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites
           << ",\"scope_resolution_implementation_interface_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits
           << ",\"scope_resolution_implementation_interface_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses
           << ",\"scope_resolution_method_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.method_resolution_sites
           << ",\"scope_resolution_method_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.method_resolution_hits
           << ",\"scope_resolution_method_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.method_resolution_misses
           << ",\"deterministic_symbol_graph_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff ? "true" : "false")
           << ",\"deterministic_scope_resolution_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff ? "true" : "false")
           << ",\"symbol_graph_scope_resolution_handoff_key\":\""
           << symbol_graph_scope_resolution_summary.deterministic_handoff_key
           << "\"},\n";
}

}  // namespace objc3::artifacts::frontend
