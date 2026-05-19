#pragma once

#include <string>

// Executable object artifact layout owns method-body binding, realization
// record linkage, and implementation-backed method-entry payload contracts.
inline constexpr const char *kObjc3ExecutableObjectArtifactLoweringContractId =
    "objc3c.executable.object.artifact.lowering.v1";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringMethodBodyBindingModel =
        "implementation-owner-identity-to-llvm-definition-symbol";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringRealizationRecordModel =
        "class-metaclass-and-category-descriptor-bundles-point-to-owner-scoped-method-list-ref-records";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringMethodEntryPayloadModel =
        "selector-owner-return-arity-implementation-symbol-has-body-direct-flag-final-flag";
inline constexpr const char *kObjc3ExecutableObjectArtifactLoweringScopeModel =
    "parser-source-identities-sema-realization-closure-ir-object-binding";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringFailClosedModel =
        "no-synthetic-implementation-symbols-no-rebound-legality-no-new-section-families";

std::string Objc3ExecutableObjectArtifactLoweringSummary();
