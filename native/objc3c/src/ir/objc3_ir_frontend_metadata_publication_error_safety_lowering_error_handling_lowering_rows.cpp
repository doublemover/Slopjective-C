#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling_lowering_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling_row_helpers.h"

void EmitObjc3IRUnwindCleanupLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRErrorHandlingLoweringCounterRow(
      "!35",
      {
          metadata.unwind_cleanup_lowering_sites,
          metadata.unwind_cleanup_lowering_unwind_edge_sites,
          metadata.unwind_cleanup_lowering_cleanup_scope_sites,
          metadata.unwind_cleanup_lowering_cleanup_emit_sites,
          metadata.unwind_cleanup_lowering_landing_pad_sites,
          metadata.unwind_cleanup_lowering_cleanup_resume_sites,
          metadata.unwind_cleanup_lowering_normalized_sites,
          metadata.unwind_cleanup_lowering_guard_blocked_sites,
          metadata.unwind_cleanup_lowering_contract_violation_sites,
      },
      metadata.deterministic_unwind_cleanup_lowering_handoff, out);
}
