#include "ir/objc3_ir_statement_binding_emission.h"

#include <algorithm>
#include <cstddef>
#include <string>

#include "ast/objc3_ast.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"
#include "lower/core/lowering_primitive_ops.h"

#include "ir/objc3_ir_statement_binding_emission_local_storage.inc"
#include "ir/objc3_ir_statement_binding_emission_value_lowering.inc"
#include "ir/objc3_ir_statement_binding_emission_assignment.inc"
#include "ir/objc3_ir_statement_binding_emission_replay_readiness.inc"
