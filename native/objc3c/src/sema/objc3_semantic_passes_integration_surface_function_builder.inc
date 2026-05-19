void PopulateSemanticIntegrationSurfaceFunctions(
    const Objc3Program &ast,
    Objc3SemanticIntegrationSurface &surface,
    const std::unordered_set<std::string> &generic_interface_names,
    bool arc_mode_enabled,
    std::vector<std::string> &diagnostics) {
  for (const auto &fn : ast.functions) {
#include "sema/objc3_semantic_passes_integration_surface_function_builder_function_collection.inc"
  }
}
