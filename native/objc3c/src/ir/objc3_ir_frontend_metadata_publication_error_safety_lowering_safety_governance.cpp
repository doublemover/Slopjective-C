#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_governance.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_governance_inline_asm_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_governance_unsafe_pointer_rows.h"

void EmitObjc3IRSafetyGovernanceLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRUnsafePointerExtensionLoweringCounterNode(metadata, out);
  EmitObjc3IRInlineAsmIntrinsicGovernanceLoweringCounterNode(metadata, out);
}
