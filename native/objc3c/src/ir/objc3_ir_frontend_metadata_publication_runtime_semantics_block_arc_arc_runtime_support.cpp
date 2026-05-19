#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_runtime_support.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRArcHelperRuntimeSupportMetadataNode(std::ostringstream &out) {
  out << "!84 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportDependencyModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportExecutionModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportFailClosedModel)
      << "\"}\n";
}
