#pragma once

#ifndef OBJC3_AST_EXPR_NODE_MEMBERS_IN_EXPR
#error "objc3_ast_expr_arc_contract_members.h must be included inside struct Expr"
#endif

  // ARC source-surface/mode-boundary constants: ownership
  // qualifiers, weak/unowned metadata, autoreleasepool profiling, and ARC
  // fix-it summaries are already parser/sema-visible, and the native driver
  // now admits the bounded helper-backed `-fobjc-arc` slice proven by the
  // executable ARC runtime probes.
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryContractId =
      "objc3c.arc.source.mode.boundary.freeze.v1";
  static inline constexpr const char *kObjc3ArcSourceModeBoundarySourceModel =
      "ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode";
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryModeModel =
      "native-driver-admits-fobjc-arc-and-fno-objc-arc-while-runnable-arc-stays-bounded-to-the-helper-backed-supported-slice";
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryNonGoalModel =
      "no-generalized-arc-cleanup-insertion-no-public-arc-runtime-abi-mode-split-no-full-arc-automation-beyond-the-supported-helper-backed-slice";
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryFailClosedModel =
      "fail-closed-on-arc-source-mode-boundary-drift-before-arc-automation";
  static inline constexpr const char *kObjc3ArcModeHandlingContractId =
      "objc3c.arc.mode.handling.v1";
  static inline constexpr const char *kObjc3ArcModeHandlingSourceModel =
      "ownership-qualified-method-property-return-and-block-capture-surfaces-are-runnable-under-explicit-arc-mode";
  static inline constexpr const char *kObjc3ArcModeHandlingModeModel =
      "driver-admits-fobjc-arc-and-fno-objc-arc-and-threads-arc-mode-through-frontend-sema-and-ir";
  static inline constexpr const char *kObjc3ArcModeHandlingFailClosedModel =
      "non-arc-mode-still-rejects-executable-ownership-qualified-method-and-function-signatures";
  static inline constexpr const char *kObjc3ArcModeHandlingNonGoalModel =
      "no-implicit-nonarc-promotion-no-cross-module-arc-mode-inference-no-full-arc-automation-beyond-the-supported-helper-backed-slice";
  static inline constexpr const char *kObjc3ArcSemanticRulesContractId =
      "objc3c.arc.semantic.rules.v1";
  static inline constexpr const char *kObjc3ArcSemanticRulesSourceModel =
      "explicit-arc-mode-admits-only-explicit-ownership-surfaces-while-forbidden-property-forms-and-broad-inference-remain-fail-closed";
  static inline constexpr const char *kObjc3ArcSemanticRulesSemanticModel =
      "conflicting-property-ownership-forms-and-atomic-ownership-aware-storage-still-fail-closed-while-general-arc-inference-remains-deferred";
  static inline constexpr const char *kObjc3ArcSemanticRulesFailClosedModel =
      "forbidden-arc-property-forms-and-non-inferred-lifetime-semantics-terminate-deterministically";
  static inline constexpr const char *kObjc3ArcSemanticRulesNonGoalModel =
      "no-general-local-lifetime-extension-no-cross-module-arc-optimization-beyond-supported-signature-and-method-family-retained-result-cleanup";
  static inline constexpr const char *kObjc3ArcInferenceLifetimeContractId =
      "objc3c.arc.inference.lifetime.v1";
  static inline constexpr const char *kObjc3ArcInferenceLifetimeSourceModel =
      "explicit-arc-mode-now-infers-strong-owned-executable-object-signatures-for-the-supported-runnable-slice";
  static inline constexpr const char *kObjc3ArcInferenceLifetimeSemanticModel =
      "arc-enabled-unqualified-object-signatures-now-produce-canonical-retain-release-lifetime-accounting-while-nonarc-remains-zero-inference";
  static inline constexpr const char *kObjc3ArcInferenceLifetimeFailClosedModel =
      "non-arc-mode-keeps-unqualified-object-signatures-non-inferred-and-zero-retain-release-lifetime-accounting";
  static inline constexpr const char *kObjc3ArcInferenceLifetimeNonGoalModel =
      "no-full-arc-cleanup-synthesis-no-weak-autorelease-return-property-synthesis-or-block-interaction-arc-semantics-yet";
  static inline constexpr const char *kObjc3ArcInteractionSemanticsContractId =
      "objc3c.arc.interaction.semantics.v1";
  static inline constexpr const char *kObjc3ArcInteractionSemanticsSourceModel =
      "explicit-arc-mode-now-covers-weak-autorelease-return-property-synthesis-and-block-ownership-interactions-for-the-supported-runnable-slice";
  static inline constexpr const char *kObjc3ArcInteractionSemanticsSemanticModel =
      "weak-properties-and-nonowning-captures-stay-nonretaining-autorelease-returns-method-family-retained-message-results-and-synthesized-property-accessors-publish-owned-lifetime-packets-under-arc";
  static inline constexpr const char *kObjc3ArcInteractionSemanticsFailClosedModel =
      "unsupported-arc-cleanup-and-broader-interactions-still-remain-explicitly-deferred";
  static inline constexpr const char *kObjc3ArcInteractionSemanticsNonGoalModel =
      "no-general-arc-cleanup-insertion-no-cross-module-arc-interop-no-method-family-automation-beyond-retained-message-result-cleanup";
