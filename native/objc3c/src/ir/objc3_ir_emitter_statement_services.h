#pragma once

#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_statement_orchestration.h"

Objc3IRStatementOrchestrationOptions
BuildObjc3IREmitterStatementOrchestrationOptions(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);
