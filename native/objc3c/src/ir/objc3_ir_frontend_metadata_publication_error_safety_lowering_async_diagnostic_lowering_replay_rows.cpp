#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic_lowering_replay_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic_row_helpers.h"

void EmitObjc3IRAsyncDiagnosticLoweringReplayCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRAsyncDiagnosticLoweringCounterRow(
      "!43",
      {metadata.async_continuation_lowering_sites,
       metadata.async_continuation_lowering_async_keyword_sites,
       metadata.async_continuation_lowering_async_function_sites,
       metadata.async_continuation_lowering_continuation_allocation_sites,
       metadata.async_continuation_lowering_continuation_resume_sites,
       metadata.async_continuation_lowering_continuation_suspend_sites,
       metadata.async_continuation_lowering_async_state_machine_sites,
       metadata.async_continuation_lowering_normalized_sites,
       metadata.async_continuation_lowering_gate_blocked_sites,
       metadata.async_continuation_lowering_contract_violation_sites},
      metadata.deterministic_async_continuation_lowering_handoff, out);
}
