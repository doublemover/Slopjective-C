#include "ir/objc3_ir_frontend_metadata_publication_concurrency.h"

#include "ir/objc3_ir_frontend_metadata_publication_concurrency_continuation_runtime.h"
#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_hardening.h"
#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime.h"

void EmitObjc3IRConcurrencyRuntimeMetadataNodes(std::ostringstream &out) {
  EmitObjc3IRConcurrencyContinuationRuntimeMetadataNodes(out);
  EmitObjc3IRConcurrencyTaskRuntimeMetadataNodes(out);
  EmitObjc3IRConcurrencyTaskRuntimeHardeningMetadataNode(out);
}
