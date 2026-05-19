    if (surface.globals.find(fn.name) != surface.globals.end()) {
      diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S200", "duplicate function '" + fn.name + "'"));
      continue;
    }

    auto it = surface.functions.find(fn.name);
    if (it == surface.functions.end()) {
#include "sema/objc3_semantic_passes_integration_surface_function_builder_surface_construction.inc"
#include "sema/objc3_semantic_passes_integration_surface_function_builder_readiness_replay.inc"
#include "sema/objc3_semantic_passes_integration_surface_function_builder_metadata_publication.inc"
    }

#include "sema/objc3_semantic_passes_integration_surface_function_builder_existing_merge.inc"
