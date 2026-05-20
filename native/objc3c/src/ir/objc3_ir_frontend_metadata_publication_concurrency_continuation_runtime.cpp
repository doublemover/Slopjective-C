#include "ir/objc3_ir_frontend_metadata_publication_concurrency_continuation_runtime.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRConcurrencyContinuationRuntimeMetadataNodes(
    std::ostringstream &out) {
  out << "!91 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyContinuationRuntimeHelperExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAllocateAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeResumeAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCancelAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyContinuationRuntimeHelperFailClosedModel)
      << "\"}\n";
  out << "!92 = !{!\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAllocateAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeResumeAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCancelAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationFailClosedModel)
      << "\"}\n";
}
