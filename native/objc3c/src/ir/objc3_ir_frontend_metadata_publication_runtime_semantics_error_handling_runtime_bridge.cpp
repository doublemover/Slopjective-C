#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_runtime_bridge.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRErrorRuntimeBridgeMetadataNodes(std::ostringstream &out) {
  out << "!89 = !{!\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeStatusErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeNSErrorErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeForeignExceptionErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCatchMatchesErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingErrorRuntimeBridgeHelperFailClosedModel)
      << "\"}\n";
  out << "!90 = !{!\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingLiveErrorRuntimeIntegrationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeStatusErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeForeignExceptionErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCatchMatchesErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationFailClosedModel)
      << "\"}\n";
}
