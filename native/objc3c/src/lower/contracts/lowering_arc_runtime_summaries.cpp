#include "lower/objc3_lowering_contract.h"

#include <sstream>
#include <string>

#include "lower/contracts/arc_runtime_debug_instrumentation_contracts.h"
#include "lower/contracts/arc_runtime_helper_api_surface_contracts.h"
#include "lower/contracts/arc_runtime_helper_runtime_support_contracts.h"
#include "lower/contracts/runnable_arc_closeout_contracts.h"
#include "lower/contracts/runnable_arc_runtime_gate_contracts.h"

std::string Objc3ArcAutomaticInsertionSummary() {
  std::ostringstream out;
  // ARC automatic-insertion anchor: lane-C now consumes the
  // existing ARC semantic insertion flags for the supported runnable slice so
  // ordinary function and method lowering materializes retain/release/
  // autorelease helper calls instead of publishing summary-only intent.
  out << "contract=" << kObjc3ArcAutomaticInsertionContractId
      << ";source_model=" << kObjc3ArcAutomaticInsertionSourceModel
      << ";lowering_model=" << kObjc3ArcAutomaticInsertionLoweringModel
      << ";arc_mode_contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";arc_inference_contract=" << Expr::kObjc3ArcInferenceLifetimeContractId
      << ";arc_interaction_contract=" << Expr::kObjc3ArcInteractionSemanticsContractId
      << ";arc_cleanup_contract=" << kObjc3ArcLoweringAbiCleanupModelContractId
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";fail_closed_model=" << kObjc3ArcAutomaticInsertionFailureModel
      << ";non_goal_model=" << kObjc3ArcAutomaticInsertionNonGoalModel
      << ";follow_on_surface=objc3c.arc.automaticinsertion.surface.v1";
  return out.str();
}

std::string Objc3ArcCleanupWeakLifetimeHooksSummary() {
  std::ostringstream out;
  // ARC cleanup/weak/lifetime lowering anchor: lane-C now widens the
  // supported ARC slice with scope-aware cleanup emission, the retained weak
  // current-property helper path, and deterministic block-capture lifetime
  // cleanup without claiming generalized weak-local or exception-driven ARC.
  out << "contract=" << kObjc3ArcCleanupWeakLifetimeHooksContractId
      << ";source_model=" << kObjc3ArcCleanupWeakLifetimeHooksSourceModel
      << ";lowering_model=" << kObjc3ArcCleanupWeakLifetimeHooksLoweringModel
      << ";arc_mode_contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";arc_interaction_contract="
      << Expr::kObjc3ArcInteractionSemanticsContractId
      << ";arc_cleanup_contract=" << kObjc3ArcLoweringAbiCleanupModelContractId
      << ";arc_insertion_contract=" << kObjc3ArcAutomaticInsertionContractId
      << ";weak_load_symbol=" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";fail_closed_model="
      << kObjc3ArcCleanupWeakLifetimeHooksFailureModel
      << ";non_goal_model=" << kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel
      << ";follow_on_surface=objc3c.arc.cleanupweaklifetime.surface.v1";
  return out.str();
}

std::string Objc3ArcBlockAutoreleaseReturnLoweringSummary() {
  std::ostringstream out;
  // ARC/block autorelease-return lowering anchor: lane-C now closes
  // the supported escaping-block plus autoreleasing-return edge inventory by
  // preserving terminal cleanup state across sibling branches while block
  // promotion/dispose helpers and autorelease-return conventions remain
  // runtime-lowered together.
  out << "contract=" << kObjc3ArcBlockAutoreleaseReturnLoweringContractId
      << ";source_model="
      << kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel
      << ";lowering_model="
      << kObjc3ArcBlockAutoreleaseReturnLoweringModel
      << ";arc_cleanup_contract="
      << kObjc3ArcCleanupWeakLifetimeHooksContractId
      << ";arc_insertion_contract=" << kObjc3ArcAutomaticInsertionContractId
      << ";block_escape_contract="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";promote_block_symbol=" << kObjc3RuntimePromoteBlockI32Symbol
      << ";invoke_block_symbol=" << kObjc3RuntimeInvokeBlockI32Symbol
      << ";fail_closed_model="
      << kObjc3ArcBlockAutoreleaseReturnLoweringFailureModel
      << ";non_goal_model="
      << kObjc3ArcBlockAutoreleaseReturnLoweringNonGoalModel
      << ";follow_on_surface=objc3c.arc.blockautoreleasereturn.surface.v1";
  return out.str();
}

std::string Objc3RuntimeArcHelperApiSurfaceSummary() {
  std::ostringstream out;
  // runtime ARC helper API surface anchor: lane-D now freezes the
  // private runtime helper ABI that current ARC lowering already consumes so
  // later runtime implementation work can widen behavior without silently
  // widening the public runtime header.
  out << "contract=" << kObjc3RuntimeArcHelperApiSurfaceContractId
      << ";reference_model=" << kObjc3RuntimeArcHelperApiSurfaceReferenceModel
      << ";weak_model=" << kObjc3RuntimeArcHelperApiSurfaceWeakModel
      << ";autoreleasepool_model="
      << kObjc3RuntimeArcHelperApiSurfaceAutoreleasepoolModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";read_current_property_symbol="
      << kObjc3RuntimeReadCurrentPropertyI32Symbol
      << ";write_current_property_symbol="
      << kObjc3RuntimeWriteCurrentPropertyI32Symbol
      << ";exchange_current_property_symbol="
      << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
      << ";load_weak_current_property_symbol="
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";store_weak_current_property_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";push_autoreleasepool_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";pop_autoreleasepool_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";fail_closed_model="
      << kObjc3RuntimeArcHelperApiSurfaceFailClosedModel
      << ";follow_on_surface=objc3c.arc.runtimehelperapi.surface.v1";
  return out.str();
}

std::string Objc3RuntimeArcHelperRuntimeSupportSummary() {
  std::ostringstream out;
  // runtime ARC helper implementation anchor: lane-D now proves the
  // private ARC helper ABI from D001 is linked and executable for the
  // currently supported ARC property/weak/autorelease-return slice.
  out << "contract=" << kObjc3RuntimeArcHelperRuntimeSupportContractId
      << ";dependency_model="
      << kObjc3RuntimeArcHelperRuntimeSupportDependencyModel
      << ";weak_model=" << kObjc3RuntimeArcHelperRuntimeSupportWeakModel
      << ";autorelease_return_model="
      << kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel
      << ";execution_model="
      << kObjc3RuntimeArcHelperRuntimeSupportExecutionModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";load_weak_current_property_symbol="
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";store_weak_current_property_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";push_autoreleasepool_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";pop_autoreleasepool_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";fail_closed_model="
      << kObjc3RuntimeArcHelperRuntimeSupportFailClosedModel
      << ";follow_on_surface=objc3c.arc.runtimehelpersupport.surface.v1";
  return out.str();
}

std::string Objc3RuntimeArcDebugInstrumentationSummary() {
  std::ostringstream out;
  // ownership-debug/runtime-validation anchor: lane-D now widens
  // the live ARC helper runtime surface with private testing snapshots and
  // deterministic counters so supported ARC helper traffic can be proven
  // without widening the public runtime ABI.
  out << "contract=" << kObjc3RuntimeArcDebugInstrumentationContractId
      << ";dependency_model="
      << kObjc3RuntimeArcDebugInstrumentationDependencyModel
      << ";coverage_model="
      << kObjc3RuntimeArcDebugInstrumentationCoverageModel
      << ";validation_model="
      << kObjc3RuntimeArcDebugInstrumentationValidationModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";read_current_property_symbol="
      << kObjc3RuntimeReadCurrentPropertyI32Symbol
      << ";write_current_property_symbol="
      << kObjc3RuntimeWriteCurrentPropertyI32Symbol
      << ";exchange_current_property_symbol="
      << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
      << ";load_weak_current_property_symbol="
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";store_weak_current_property_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";push_autoreleasepool_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";pop_autoreleasepool_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";fail_closed_model="
      << kObjc3RuntimeArcDebugInstrumentationFailClosedModel
      << ";follow_on_surface=objc3c.arc.debuginstrumentation.surface.v1";
  return out.str();
}

std::string Objc3RunnableArcRuntimeGateSummary() {
  std::ostringstream out;
  // runnable-arc-runtime gate anchor: lane-E freezes the supported
  // runnable ARC slice by consuming the current mode-handling, interaction,
  // lowering, and runtime proofs rather than widening source, lowering, or
  // public runtime behavior.
  out << "contract=" << kObjc3RunnableArcRuntimeGateContractId
      << ";evidence_model=" << kObjc3RunnableArcRuntimeGateEvidenceModel
      << ";active_gate_model=" << kObjc3RunnableArcRuntimeGateActiveModel
      << ";non_goal_model=" << kObjc3RunnableArcRuntimeGateNonGoalModel
      << ";mode_contract=" << kObjc3ArcModeHandlingContractId
      << ";interaction_contract=" << kObjc3ArcInteractionSemanticsContractId
      << ";lowering_contract="
      << kObjc3ArcBlockAutoreleaseReturnLoweringContractId
      << ";runtime_contract=" << kObjc3RuntimeArcDebugInstrumentationContractId
      << ";fail_closed_model=" << kObjc3RunnableArcRuntimeGateFailClosedModel
      << ";follow_on_surface=objc3c.arc.runnablegate.surface.v1";
  return out.str();
}

std::string Objc3RunnableArcCloseoutSummary() {
  std::ostringstream out;
  // runnable-arc-closeout anchor: lane-E closes the current ARC
  // tranche by consuming the existing mode-handling, interaction, lowering,
  // runtime, and gate proofs plus integrated execution smoke instead of
  // widening the supported source/runtime slice.
  out << "contract=" << kObjc3RunnableArcCloseoutContractId
      << ";matrix_model=" << kObjc3RunnableArcCloseoutMatrixModel
      << ";smoke_model=" << kObjc3RunnableArcCloseoutSmokeModel
      << ";gate_contract=" << kObjc3RunnableArcRuntimeGateContractId
      << ";runtime_contract=" << kObjc3RuntimeArcDebugInstrumentationContractId
      << ";fail_closed_model=" << kObjc3RunnableArcCloseoutFailClosedModel
      << ";follow_on_surface=objc3c.arc.closeout.surface.v1";
  return out.str();
}
