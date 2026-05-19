#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic_async_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic_error_safety_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic_lowering_replay_rows.h"

void EmitObjc3IRAsyncDiagnosticLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRAsyncDiagnosticAwaitLoweringCounterNode(metadata, out);
  EmitObjc3IRAsyncDiagnosticLoweringReplayCounterNode(metadata, out);
  EmitObjc3IRErrorSafetyDiagnosticRecoveryCounterNode(metadata, out);
}
