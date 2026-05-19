#include "sema/objc3_semantic_passes.h"

#include <algorithm>
#include <sstream>

#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"
#include "sema/model/semantic_symbol_core_source_closures.h"

#include "sema/objc3_semantic_concurrency_model_summary_async_effects.inc"
#include "sema/objc3_semantic_concurrency_model_summary_task_executor.inc"
#include "sema/objc3_semantic_concurrency_model_summary_actor_isolation.inc"
