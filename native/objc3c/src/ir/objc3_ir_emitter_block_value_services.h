#pragma once

#include "ir/objc3_ir_block_lowering.h"
#include "ir/objc3_ir_compile_time_proof_analysis.h"
#include "ir/objc3_ir_value_materialization.h"

struct Objc3IREmitterServiceContextCallbacks;
struct Objc3IREmitterServiceContextState;

Objc3IRBlockLoweringContext BuildObjc3IREmitterBlockLoweringContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);

Objc3IRCompileTimeProofAnalysisContext
BuildObjc3IREmitterCompileTimeProofAnalysisContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);

Objc3IRValueMaterializationContext
BuildObjc3IREmitterValueMaterializationContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);
