#pragma once

#include <string>

// Runtime object executable support owns the emitted method implementation
// binding and realization-ready class/protocol/category record contracts that
// object emission publishes before runtime graph realization consumes them.
inline constexpr const char *kObjc3ExecutableMethodBodyBindingContractId =
    "objc3c.executable.method.body.binding.v1";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingSourceModel =
    "implementation-owned-method-entry-owner-identity-selects-one-llvm-definition-symbol";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingRuntimeModel =
    "emitted-method-entry-implementation-pointer-dispatches-through-objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingFailClosedModel =
    "error-on-missing-or-duplicate-implementation-binding";

inline constexpr const char *kObjc3ExecutableRealizationRecordsContractId =
    "objc3c.executable.realization.records.v1";
inline constexpr const char *kObjc3ExecutableRealizationClassRecordModel =
    "class-and-metaclass-records-carry-bundle-object-and-super-owner-identities-plus-method-list-refs";
inline constexpr const char *kObjc3ExecutableRealizationProtocolRecordModel =
    "protocol-records-carry-owner-inherited-protocol-edges-and-split-instance-class-method-counts";
inline constexpr const char *kObjc3ExecutableRealizationCategoryRecordModel =
    "category-records-carry-explicit-class-and-category-owner-identities-plus-attachment-and-adopted-protocol-edges";
inline constexpr const char *kObjc3ExecutableRealizationFailClosedModel =
    "no-identity-edge-elision-no-out-of-band-graph-reconstruction";

std::string Objc3ExecutableMethodBodyBindingSummary();
std::string Objc3ExecutableRealizationRecordsSummary();
