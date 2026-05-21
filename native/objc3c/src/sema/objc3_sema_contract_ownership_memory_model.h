#pragma once

#include <string>

inline constexpr const char *kObjc3OwnershipMemoryModelFormalSliceContractId =
    "objc3c.ownership.memory-model.formal-slice.v1";
inline constexpr const char *kObjc3OwnershipMemoryModelFormalSliceIssueRef =
    "8166";
inline constexpr const char *kObjc3OwnershipMemoryModelFormalSliceClaim =
    "objc3c.behavior.language.ownership-memory-model";
inline constexpr const char *kObjc3OwnershipMemoryModelFormalSliceSourceRule =
    "strong-owned-weak-unowned-borrowed-consumed-and-autoreleased-ownership-facts-must-be-derived-from-live-sema-lowering-runtime-and-fixture-sources";
inline constexpr const char *kObjc3OwnershipMemoryModelFormalSliceConsumeRule =
    "consumed-retainable-values-transfer-ownership-at-consumed-call-boundaries-and-remain-unavailable-until-reassigned";
inline constexpr const char *kObjc3OwnershipMemoryModelFormalSliceBorrowRule =
    "borrowed-references-are-non-owning-and-must-not-escape-through-unproven-parameters-escaping-blocks-or-untied-borrowed-returns";
inline constexpr const char *kObjc3OwnershipMemoryModelFormalSliceArcRule =
    "arc-boundary-lowering-uses-private-retain-release-autorelease-weak-and-block-helper-contracts-for-the-supported-slice";
inline constexpr const char *kObjc3OwnershipMemoryModelFormalSlicePublicApiRule =
    "public-runtime-result-and-string-contracts-expose-runtime-owned-borrowed-data-with-fail-closed-ownership-diagnostics";
inline constexpr const char *kObjc3OwnershipMemoryModelFormalSliceFailureRule =
    "unsupported-ownership-qualifiers-unsafe-borrow-escapes-use-after-consume-and-summary-only-runtime-claims-remain-fail-closed";

inline constexpr const char *kObjc3OwnershipMemoryModelConsumedPositiveFixture =
    "tests/native/sema/ownership/retainable_consumed_reassignment.objc3";
inline constexpr const char *kObjc3OwnershipMemoryModelConsumedNegativeFixture =
    "tests/native/sema/errors/retainable_consumed_use_after_rejected.objc3";
inline constexpr const char *kObjc3OwnershipMemoryModelBorrowedPositiveFixture =
    "tests/tooling/fixtures/native/borrowed_pointer_escape_analysis_positive.objc3";
inline constexpr const char *kObjc3OwnershipMemoryModelArcBoundarySource =
    "native/objc3c/src/lower/contracts/arc_runtime_lifetime_lowering_contracts.h";
inline constexpr const char *kObjc3OwnershipMemoryModelBorrowedDiagnosticsSource =
    "native/objc3c/src/sema/objc3_semantic_passes_borrowed_ownership_diagnostics_pointer_escape_diagnostics.inc";
inline constexpr const char *kObjc3OwnershipMemoryModelRuntimeResultSource =
    "native/objc3c/src/runtime/public/objc3_runtime_result_ownership.cpp";

struct Objc3OwnershipMemoryModelFormalSliceSummary {
  std::string contract_id = kObjc3OwnershipMemoryModelFormalSliceContractId;
  std::string issue_ref = kObjc3OwnershipMemoryModelFormalSliceIssueRef;
  std::string support_claim = kObjc3OwnershipMemoryModelFormalSliceClaim;
  std::string source_rule = kObjc3OwnershipMemoryModelFormalSliceSourceRule;
  std::string consume_rule = kObjc3OwnershipMemoryModelFormalSliceConsumeRule;
  std::string borrow_rule = kObjc3OwnershipMemoryModelFormalSliceBorrowRule;
  std::string arc_rule = kObjc3OwnershipMemoryModelFormalSliceArcRule;
  std::string public_api_rule =
      kObjc3OwnershipMemoryModelFormalSlicePublicApiRule;
  std::string failure_rule = kObjc3OwnershipMemoryModelFormalSliceFailureRule;
  bool consumed_values_contract_ready = false;
  bool borrowed_reference_contract_ready = false;
  bool arc_boundary_lowering_contract_ready = false;
  bool runtime_result_boundary_contract_ready = false;
  bool diagnostics_evidence_contract_ready = false;
  bool drift_guard_source_derived = false;
  std::string failure_reason;
};

inline bool IsReadyObjc3OwnershipMemoryModelFormalSliceSummary(
    const Objc3OwnershipMemoryModelFormalSliceSummary &summary) {
  return summary.contract_id == kObjc3OwnershipMemoryModelFormalSliceContractId &&
         summary.issue_ref == kObjc3OwnershipMemoryModelFormalSliceIssueRef &&
         summary.support_claim == kObjc3OwnershipMemoryModelFormalSliceClaim &&
         !summary.source_rule.empty() && !summary.consume_rule.empty() &&
         !summary.borrow_rule.empty() && !summary.arc_rule.empty() &&
         !summary.public_api_rule.empty() && !summary.failure_rule.empty() &&
         summary.consumed_values_contract_ready &&
         summary.borrowed_reference_contract_ready &&
         summary.arc_boundary_lowering_contract_ready &&
         summary.runtime_result_boundary_contract_ready &&
         summary.diagnostics_evidence_contract_ready &&
         summary.drift_guard_source_derived && summary.failure_reason.empty();
}
