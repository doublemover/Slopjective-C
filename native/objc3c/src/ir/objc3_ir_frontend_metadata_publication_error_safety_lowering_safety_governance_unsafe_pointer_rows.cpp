#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_governance_unsafe_pointer_rows.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_governance_row_helpers.h"

void EmitObjc3IRUnsafePointerExtensionLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRSafetyGovernanceLoweringCounterRow(
      "!37",
      {metadata.unsafe_pointer_extension_lowering_sites,
       metadata.unsafe_pointer_extension_lowering_unsafe_keyword_sites,
       metadata.unsafe_pointer_extension_lowering_pointer_arithmetic_sites,
       metadata.unsafe_pointer_extension_lowering_raw_pointer_type_sites,
       metadata.unsafe_pointer_extension_lowering_unsafe_operation_sites,
       metadata.unsafe_pointer_extension_lowering_normalized_sites,
       metadata.unsafe_pointer_extension_lowering_gate_blocked_sites,
       metadata.unsafe_pointer_extension_lowering_contract_violation_sites},
      metadata.deterministic_unsafe_pointer_extension_lowering_handoff, out);
}
