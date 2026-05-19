#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_runnable_arc_gate.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRRunnableArcGateMetadataNodes(std::ostringstream &out) {
  out << "!86 = !{!\""
      << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateEvidenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateActiveModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcInteractionSemanticsContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateFailClosedModel)
      << "\"}\n";
}
