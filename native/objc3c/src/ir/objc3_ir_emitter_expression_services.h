#pragma once

#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_expression_call_orchestration.h"

Objc3IRExpressionCallEmissionOptions
BuildObjc3IREmitterExpressionCallEmissionOptions(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);
