std::string ExplainNonDeterministicSemanticTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
#include "sema/objc3_semantic_type_metadata_handoff_ordering_checks.inc"
#include "sema/objc3_semantic_type_metadata_handoff_member_metadata_checks.inc"
#include "sema/objc3_semantic_type_metadata_handoff_type_annotation_checks.inc"
#include "sema/objc3_semantic_type_metadata_handoff_summary_drift_checks.inc"

  return "determinism invariant failed before the current debug summary checks";
}
