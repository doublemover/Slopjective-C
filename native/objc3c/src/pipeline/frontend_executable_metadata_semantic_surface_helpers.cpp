#include "pipeline/frontend_executable_metadata_semantic_surface_helpers.h"

#include "pipeline/frontend_executable_metadata_semantic_surface_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

Objc3ExecutableMetadataSemanticValidationSurface
BuildExecutableMetadataSemanticValidationSurface(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3SemanticTypeMetadataHandoff &sema_type_metadata_handoff,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary) {
  Objc3ExecutableMetadataSemanticValidationSurface surface;
  PopulateExecutableMetadataSemanticValidationBaseState(
      surface,
      graph,
      semantic_consistency_boundary,
      sema_type_metadata_handoff,
      class_protocol_category_linking_summary);
  PopulateExecutableMetadataInheritanceValidation(surface, graph);
  PopulateExecutableMetadataMetaclassValidation(surface, graph);
  PopulateExecutableMetadataOverrideValidation(surface, graph);
  PublishExecutableMetadataSemanticValidationReadiness(
      surface,
      sema_type_metadata_handoff.method_lookup_override_conflict_summary,
      class_protocol_category_linking_summary);
  return surface;
}

}  // namespace objc3c::pipeline::orchestration
