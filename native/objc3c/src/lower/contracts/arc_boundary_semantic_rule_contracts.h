#pragma once

#include <string>

// ARC semantic rule contracts own forbidden forms, fail-closed property
// semantics, and the boundary around deferred generalized inference.
inline constexpr const char *kObjc3ArcSemanticRulesContractId =
    "objc3c.arc.semantic.rules.v1";
inline constexpr const char *kObjc3ArcSemanticRulesSourceModel =
    "explicit-arc-mode-admits-only-explicit-ownership-surfaces-while-forbidden-property-forms-and-broad-inference-remain-fail-closed";
inline constexpr const char *kObjc3ArcSemanticRulesSemanticModel =
    "conflicting-property-ownership-forms-and-atomic-ownership-aware-storage-still-fail-closed-while-general-arc-inference-remains-deferred";
inline constexpr const char *kObjc3ArcSemanticRulesFailClosedModel =
    "forbidden-arc-property-forms-and-non-inferred-lifetime-semantics-terminate-deterministically";
inline constexpr const char *kObjc3ArcSemanticRulesNonGoalModel =
    "no-implicit-retain-release-inference-no-lifetime-extension-no-method-family-based-arc-semantics-yet";

std::string Objc3ArcSemanticRulesSummary();
