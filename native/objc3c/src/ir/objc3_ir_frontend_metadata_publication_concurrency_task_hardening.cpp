#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_hardening.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRConcurrencyTaskRuntimeHardeningMetadataNode(
    std::ostringstream &out) {
  out << "!96 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_reset_for_testing")
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_task_runtime_state_for_testing")
      << "\", !\""
      << EscapeCStringLiteral(
             "objc3_runtime_copy_memory_management_state_for_testing")
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_arc_debug_state_for_testing")
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\"}\n";
}
