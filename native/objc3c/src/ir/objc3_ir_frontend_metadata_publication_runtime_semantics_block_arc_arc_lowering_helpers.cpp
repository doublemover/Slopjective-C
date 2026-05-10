#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRArcLoweringSemanticsHelperMetadataNodes(
    std::ostringstream &out) {
  out << "!80 = !{!\""
      << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionSourceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionLoweringModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcLoweringAbiCleanupModelContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionFailureModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionNonGoalModel)
      << "\"}\n";
  out << "!81 = !{!\""
      << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksSourceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksLoweringModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcLoweringAbiCleanupModelContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksFailureModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel)
      << "\"}\n";
  out << "!82 = !{!\""
      << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePromoteBlockI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeInvokeBlockI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringFailureModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringNonGoalModel)
      << "\"}\n";
}
