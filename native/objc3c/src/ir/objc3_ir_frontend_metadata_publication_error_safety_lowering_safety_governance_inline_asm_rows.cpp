#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_governance_inline_asm_rows.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_governance_row_helpers.h"

void EmitObjc3IRInlineAsmIntrinsicGovernanceLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRSafetyGovernanceLoweringCounterRow(
      "!38",
      {metadata.inline_asm_intrinsic_governance_lowering_sites,
       metadata.inline_asm_intrinsic_governance_lowering_inline_asm_sites,
       metadata.inline_asm_intrinsic_governance_lowering_intrinsic_sites,
       metadata
           .inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites,
       metadata
           .inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites,
       metadata.inline_asm_intrinsic_governance_lowering_normalized_sites,
       metadata.inline_asm_intrinsic_governance_lowering_gate_blocked_sites,
       metadata
           .inline_asm_intrinsic_governance_lowering_contract_violation_sites},
      metadata.deterministic_inline_asm_intrinsic_governance_lowering_handoff,
      out);
}
