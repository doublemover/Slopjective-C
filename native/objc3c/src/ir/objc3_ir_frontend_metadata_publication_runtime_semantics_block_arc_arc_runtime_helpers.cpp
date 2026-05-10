#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_runtime_helpers.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRArcLoweringHelperRuntimeMetadataNodes(std::ostringstream &out) {
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
  out << "!83 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceReferenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceAutoreleasepoolModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceFailClosedModel)
      << "\"}\n";
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
  out << "!85 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationDependencyModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationCoverageModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationValidationModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationFailClosedModel)
      << "\"}\n";
}
