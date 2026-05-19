#include "lower/objc3_lowering_contract.h"

#include "ast/objc3_ast.h"

#include <sstream>
#include <string>

std::string Objc3ArcSourceModeBoundarySummary() {
  std::ostringstream out;
  // ARC source-surface/mode-boundary anchor: ownership qualifiers,
  // weak/unowned metadata, autoreleasepool profiling, and ARC fix-it surfaces
  // remain live in parser/sema/replay space, but the native driver still
  // rejects `-fobjc-arc` and executable ownership-qualified functions/methods
  // stay fail-closed until ARC automation begins in the next lowering step.
  out << "contract=" << Expr::kObjc3ArcSourceModeBoundaryContractId
      << ";source_model=" << Expr::kObjc3ArcSourceModeBoundarySourceModel
      << ";mode_model=" << Expr::kObjc3ArcSourceModeBoundaryModeModel
      << ";ownership_qualifier_lane="
      << kObjc3OwnershipQualifierLoweringLaneContract
      << ";retain_release_lane=" << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";autoreleasepool_lane=" << kObjc3AutoreleasePoolScopeLoweringLaneContract
      << ";weak_unowned_lane=" << kObjc3WeakUnownedSemanticsLoweringLaneContract
      << ";arc_fixit_lane=" << kObjc3ArcDiagnosticsFixitLoweringLaneContract
      << ";non_goal_model=" << Expr::kObjc3ArcSourceModeBoundaryNonGoalModel
      << ";fail_closed_model=" << Expr::kObjc3ArcSourceModeBoundaryFailClosedModel
      << ";follow_on_surface=objc3c.arc.sourcemode.boundary.v1";
  return out.str();
}

std::string Objc3ArcModeHandlingSummary(bool arc_mode_enabled) {
  std::ostringstream out;
  // ARC mode-handling core implementation anchor: the native driver
  // now admits explicit ARC mode, threads it through frontend/sema/IR, and
  // keeps non-ARC ownership-qualified executable signatures fail-closed.
  out << "contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";source_model=" << Expr::kObjc3ArcModeHandlingSourceModel
      << ";mode_model=" << Expr::kObjc3ArcModeHandlingModeModel
      << ";arc_mode=" << (arc_mode_enabled ? "enabled" : "disabled")
      << ";ownership_qualifier_lane="
      << kObjc3OwnershipQualifierLoweringLaneContract
      << ";retain_release_lane=" << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";autoreleasepool_lane=" << kObjc3AutoreleasePoolScopeLoweringLaneContract
      << ";weak_unowned_lane=" << kObjc3WeakUnownedSemanticsLoweringLaneContract
      << ";arc_fixit_lane=" << kObjc3ArcDiagnosticsFixitLoweringLaneContract
      << ";block_runtime_gate=" << Expr::kObjc3RunnableBlockRuntimeGateContractId
      << ";fail_closed_model=" << Expr::kObjc3ArcModeHandlingFailClosedModel
      << ";non_goal_model=" << Expr::kObjc3ArcModeHandlingNonGoalModel
      << ";follow_on_surface=objc3c.arc.modehandling.surface.v1";
  return out.str();
}

std::string Objc3ArcSemanticRulesSummary() {
  std::ostringstream out;
  // ARC semantic-rule freeze anchor: explicit ARC mode is now a real
  // admission boundary, but property ownership conflicts, atomic
  // ownership-aware storage, and broader ARC inference still fail closed until
  // later lane-B implementation issues land.
  out << "contract=" << Expr::kObjc3ArcSemanticRulesContractId
      << ";source_model=" << Expr::kObjc3ArcSemanticRulesSourceModel
      << ";semantic_model=" << Expr::kObjc3ArcSemanticRulesSemanticModel
      << ";weak_unowned_lane=" << kObjc3WeakUnownedSemanticsLoweringLaneContract
      << ";arc_fixit_lane=" << kObjc3ArcDiagnosticsFixitLoweringLaneContract
      << ";fail_closed_model=" << Expr::kObjc3ArcSemanticRulesFailClosedModel
      << ";non_goal_model=" << Expr::kObjc3ArcSemanticRulesNonGoalModel
      << ";follow_on_surface=objc3c.arc.semanticrules.surface.v1";
  return out.str();
}

std::string Objc3ArcInferenceLifetimeSummary() {
  std::ostringstream out;
  // ARC inference/lifetime implementation anchor: explicit ARC mode
  // now upgrades the supported runnable slice from explicit-only ownership
  // spelling to semantic strong-owned inference for unqualified object
  // parameters, returns, and property surfaces, while non-ARC remains a
  // zero-inference baseline and broader ARC cleanup/runtime interactions stay
  // deferred.
  out << "contract=" << Expr::kObjc3ArcInferenceLifetimeContractId
      << ";source_model=" << Expr::kObjc3ArcInferenceLifetimeSourceModel
      << ";semantic_model=" << Expr::kObjc3ArcInferenceLifetimeSemanticModel
      << ";arc_mode_contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";semantic_rules_contract=" << Expr::kObjc3ArcSemanticRulesContractId
      << ";retain_release_lane=" << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";block_escape_lane=" << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";fail_closed_model="
      << Expr::kObjc3ArcInferenceLifetimeFailClosedModel
      << ";non_goal_model=" << Expr::kObjc3ArcInferenceLifetimeNonGoalModel
      << ";follow_on_surface=objc3c.arc.inferencelifetime.surface.v1";
  return out.str();
}
