#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

#include "artifacts/objc3_frontend_concurrency_semantic_json_task_executor_cancellation.inc"
#include "artifacts/objc3_frontend_concurrency_semantic_json_async_effect_suspension.inc"
#include "artifacts/objc3_frontend_concurrency_semantic_json_await_suspension_resume.inc"
#include "artifacts/objc3_frontend_concurrency_semantic_json_async_diagnostics.inc"
#include "artifacts/objc3_frontend_concurrency_semantic_json_structured_task_cancellation.inc"
#include "artifacts/objc3_frontend_concurrency_semantic_json_executor_hop_affinity.inc"

}  // namespace objc3::artifacts::frontend
