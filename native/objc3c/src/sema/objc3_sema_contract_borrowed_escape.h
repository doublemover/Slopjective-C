#pragma once

#include <cstddef>
#include <string>

inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisDependencyContractId =
        "objc3c.ownership.resource.move.use.after.move.semantics.v1";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisContractId =
        "objc3c.ownership.borrowed.pointer.escape.analysis.v1";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisSurfacePath =
        "frontend.pipeline.semantic_surface.objc_ownership_borrowed_pointer_escape_analysis";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisRule =
        "borrowed-pointer-bindings-now-fail-closed-on-unproven-call-boundaries-escaping-block-capture-and-invalid-borrowed-return-contracts-while-retainable-family-legality-lowering-and-runtime-remain-later-runtime-work";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisDeferredRule =
        "retainable-family-legality-lowering-and-runtime-remain-deferred-to-later-runtime-lanes";

struct Objc3OwnershipBorrowedPointerEscapeAnalysisSummary {
  std::string contract_id =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisContractId;
  std::string dependency_contract_id =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisDependencyContractId;
  std::string surface_path =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisSurfacePath;
  std::string semantic_model =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisRule;
  std::string deferred_model =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisDeferredRule;
  std::size_t borrowed_parameter_sites = 0;
  std::size_t borrowed_return_callable_sites = 0;
  std::size_t borrowed_escape_candidate_sites = 0;
  std::size_t illegal_unproven_call_escape_sites = 0;
  std::size_t illegal_escaping_block_capture_sites = 0;
  std::size_t illegal_borrowed_return_sites = 0;
  bool dependency_required = false;
  bool borrowed_call_boundary_enforced = false;
  bool escaping_block_capture_fail_closed = false;
  bool borrowed_return_contract_enforced = false;
  bool retainable_family_legality_deferred = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3OwnershipBorrowedPointerEscapeAnalysisSummary(
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.borrowed_call_boundary_enforced &&
         summary.escaping_block_capture_fail_closed &&
         summary.borrowed_return_contract_enforced &&
         summary.retainable_family_legality_deferred &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}
