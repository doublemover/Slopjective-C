#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime_cancellation_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime_replay_guard_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime_task_rows.h"

void EmitObjc3IRConcurrencyTaskRuntimeMetadataNodes(std::ostringstream &out) {
  EmitObjc3IRConcurrencyTaskRuntimeAbiMetadataNode(out);
  EmitObjc3IRConcurrencyTaskRuntimeCancellationMetadataNode(out);
  EmitObjc3IRConcurrencyTaskRuntimeReplayGuardMetadataNode(out);
}
