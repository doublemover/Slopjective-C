#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling_error_handling_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling_error_safety_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling_lowering_rows.h"

void EmitObjc3IRErrorHandlingLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRThrowsPropagationLoweringCounterNode(metadata, out);
  EmitObjc3IRUnwindCleanupLoweringCounterNode(metadata, out);
  EmitObjc3IRNSErrorBridgingLoweringCounterNode(metadata, out);
}
