#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic_async_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic_row_helpers.h"

void EmitObjc3IRAsyncDiagnosticAwaitLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRAsyncDiagnosticLoweringCounterRow(
      "!42",
      {metadata.await_lowering_suspension_state_lowering_sites,
       metadata.await_lowering_suspension_state_lowering_await_keyword_sites,
       metadata
           .await_lowering_suspension_state_lowering_await_suspension_point_sites,
       metadata.await_lowering_suspension_state_lowering_await_resume_sites,
       metadata
           .await_lowering_suspension_state_lowering_await_state_machine_sites,
       metadata.await_lowering_suspension_state_lowering_await_continuation_sites,
       metadata.await_lowering_suspension_state_lowering_normalized_sites,
       metadata.await_lowering_suspension_state_lowering_gate_blocked_sites,
       metadata.await_lowering_suspension_state_lowering_contract_violation_sites},
      metadata.deterministic_await_lowering_suspension_state_lowering_handoff,
      out);
}
