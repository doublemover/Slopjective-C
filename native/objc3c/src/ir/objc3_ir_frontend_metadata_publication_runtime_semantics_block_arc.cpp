#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRBlockArcMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!73 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateActiveModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateNonGoalModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId)
      << "\"}\n";
  out << "!74 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixActiveModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixNonGoalModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\"}\n";
  out << "!75 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundarySourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryModeModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryFailClosedModel)
      << "\"}\n";
  out << "!76 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingModeModel)
      << "\", !\"" << EscapeCStringLiteral(metadata.arc_mode)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingNonGoalModel)
      << "\"}\n";
  out << "!77 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesNonGoalModel)
      << "\"}\n";
  out << "!78 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeNonGoalModel)
      << "\"}\n";
  out << "!79 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsNonGoalModel)
      << "\"}\n";
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
  out << "!86 = !{!\""
      << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateEvidenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateActiveModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcInteractionSemanticsContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateFailClosedModel)
      << "\"}\n";
}
