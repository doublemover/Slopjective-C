#pragma once

#include <sstream>

#include "ir/objc3_ir_runtime_dispatch_state.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRRuntimeDispatchDeclarations(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state, std::ostringstream &out);
