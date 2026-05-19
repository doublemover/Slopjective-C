#include "ir/objc3_ir_runtime_bootstrap_global_emission.h"

#include "ast/objc3_ast_contracts.h"
#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

#include <cstdint>
#include <limits>
#include <sstream>

#include "ir/objc3_ir_runtime_bootstrap_metadata_comments.inc"
#include "ir/objc3_ir_runtime_bootstrap_globals.inc"
#include "ir/objc3_ir_runtime_bootstrap_lowering_functions.inc"
