#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling_error_handling_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling_row_helpers.h"

void EmitObjc3IRThrowsPropagationLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRErrorHandlingLoweringCounterRow(
      "!34",
      {
          metadata.throws_propagation_lowering_sites,
          metadata.throws_propagation_lowering_namespace_segment_sites,
          metadata.throws_propagation_lowering_import_edge_candidate_sites,
          metadata.throws_propagation_lowering_object_pointer_type_sites,
          metadata.throws_propagation_lowering_pointer_declarator_sites,
          metadata.throws_propagation_lowering_normalized_sites,
          metadata.throws_propagation_lowering_cache_invalidation_candidate_sites,
          metadata.throws_propagation_lowering_contract_violation_sites,
      },
      metadata.deterministic_throws_propagation_lowering_handoff, out);
}
