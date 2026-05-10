#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "io/objc3_json.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

#include "artifacts/objc3_frontend_concurrency_lowering_json_task_runtime.inc"
#include "artifacts/objc3_frontend_concurrency_lowering_json_continuation_async.inc"
#include "artifacts/objc3_frontend_concurrency_lowering_json_async_direct_call.inc"
#include "artifacts/objc3_frontend_concurrency_lowering_json_suspension_cleanup.inc"

}  // namespace objc3::artifacts::frontend
