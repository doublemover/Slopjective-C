#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc_arc_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc_row_helpers.h"

void EmitObjc3IRArcDiagnosticsFixitLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRDispatchOwnershipArcCounterRow(
      "!18",
      {metadata
           .arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites,
       metadata
           .arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites,
       metadata.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites,
       metadata
           .arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites,
       metadata
           .arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites,
       metadata.arc_diagnostics_fixit_lowering_contract_violation_sites},
      metadata.deterministic_arc_diagnostics_fixit_lowering_handoff, out);
}
