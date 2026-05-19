#include "lower/contracts/lowering_arc_contracts.h"

#include <string>

#include "lower/metadata/lowering_metadata_helpers.h"

bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract) {
  return contract.invalid_ownership_qualifier_sites <=
             contract.ownership_qualifier_sites &&
         contract.ownership_qualifier_sites <=
             contract.object_pointer_type_annotation_sites;
}

std::string Objc3OwnershipQualifierLoweringReplayKey(
    const Objc3OwnershipQualifierLoweringContract &contract) {
  return std::string("ownership_qualifier_sites=") +
         std::to_string(contract.ownership_qualifier_sites) +
         ";invalid_ownership_qualifier_sites=" +
         std::to_string(contract.invalid_ownership_qualifier_sites) +
         ";object_pointer_type_annotation_sites=" +
         std::to_string(contract.object_pointer_type_annotation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3OwnershipQualifierLoweringLaneContract;
}

bool IsValidObjc3RetainReleaseOperationLoweringContract(
    const Objc3RetainReleaseOperationLoweringContract &contract) {
  const std::size_t qualified_or_violation =
      contract.ownership_qualified_sites + contract.contract_violation_sites;
  return contract.retain_insertion_sites <= qualified_or_violation &&
         contract.release_insertion_sites <= qualified_or_violation &&
         contract.autorelease_insertion_sites <= qualified_or_violation;
}

std::string Objc3RetainReleaseOperationLoweringReplayKey(
    const Objc3RetainReleaseOperationLoweringContract &contract) {
  return std::string("ownership_qualified_sites=") +
         std::to_string(contract.ownership_qualified_sites) +
         ";retain_insertion_sites=" +
         std::to_string(contract.retain_insertion_sites) +
         ";release_insertion_sites=" +
         std::to_string(contract.release_insertion_sites) +
         ";autorelease_insertion_sites=" +
         std::to_string(contract.autorelease_insertion_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3RetainReleaseOperationLoweringLaneContract;
}

bool IsValidObjc3AutoreleasePoolScopeLoweringContract(
    const Objc3AutoreleasePoolScopeLoweringContract &contract) {
  return contract.scope_symbolized_sites <= contract.scope_sites &&
         contract.contract_violation_sites <= contract.scope_sites &&
         contract.scope_entry_transition_sites == contract.scope_sites &&
         contract.scope_exit_transition_sites == contract.scope_sites &&
         (contract.scope_sites > 0u || contract.max_scope_depth == 0u) &&
         contract.max_scope_depth <= static_cast<unsigned>(contract.scope_sites);
}

std::string Objc3AutoreleasePoolScopeLoweringReplayKey(
    const Objc3AutoreleasePoolScopeLoweringContract &contract) {
  return std::string("scope_sites=") +
         std::to_string(contract.scope_sites) +
         ";scope_symbolized_sites=" +
         std::to_string(contract.scope_symbolized_sites) +
         ";max_scope_depth=" + std::to_string(contract.max_scope_depth) +
         ";scope_entry_transition_sites=" +
         std::to_string(contract.scope_entry_transition_sites) +
         ";scope_exit_transition_sites=" +
         std::to_string(contract.scope_exit_transition_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3AutoreleasePoolScopeLoweringLaneContract;
}

bool IsValidObjc3WeakUnownedSemanticsLoweringContract(
    const Objc3WeakUnownedSemanticsLoweringContract &contract) {
  return contract.weak_reference_sites <= contract.ownership_candidate_sites &&
         contract.unowned_reference_sites <=
             contract.ownership_candidate_sites &&
         contract.unowned_safe_reference_sites <=
             contract.unowned_reference_sites &&
         contract.weak_unowned_conflict_sites <=
             contract.ownership_candidate_sites &&
         contract.contract_violation_sites <=
             contract.ownership_candidate_sites +
                 contract.weak_unowned_conflict_sites;
}

std::string Objc3WeakUnownedSemanticsLoweringReplayKey(
    const Objc3WeakUnownedSemanticsLoweringContract &contract) {
  return std::string("ownership_candidate_sites=") +
         std::to_string(contract.ownership_candidate_sites) +
         ";weak_reference_sites=" +
         std::to_string(contract.weak_reference_sites) +
         ";unowned_reference_sites=" +
         std::to_string(contract.unowned_reference_sites) +
         ";unowned_safe_reference_sites=" +
         std::to_string(contract.unowned_safe_reference_sites) +
         ";weak_unowned_conflict_sites=" +
         std::to_string(contract.weak_unowned_conflict_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3WeakUnownedSemanticsLoweringLaneContract;
}

bool IsValidObjc3ArcDiagnosticsFixitLoweringContract(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract) {
  return contract.ownership_arc_fixit_available_sites <=
             contract.ownership_arc_diagnostic_candidate_sites +
                 contract.contract_violation_sites &&
         contract.ownership_arc_profiled_sites <=
             contract.ownership_arc_diagnostic_candidate_sites +
                 contract.contract_violation_sites &&
         contract.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
             contract.ownership_arc_diagnostic_candidate_sites +
                 contract.contract_violation_sites &&
         contract.ownership_arc_empty_fixit_hint_sites <=
             contract.ownership_arc_fixit_available_sites +
                 contract.contract_violation_sites;
}

std::string Objc3ArcDiagnosticsFixitLoweringReplayKey(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract) {
  return std::string("ownership_arc_diagnostic_candidate_sites=") +
         std::to_string(
             contract.ownership_arc_diagnostic_candidate_sites) +
         ";ownership_arc_fixit_available_sites=" +
         std::to_string(contract.ownership_arc_fixit_available_sites) +
         ";ownership_arc_profiled_sites=" +
         std::to_string(contract.ownership_arc_profiled_sites) +
         ";ownership_arc_weak_unowned_conflict_diagnostic_sites=" +
         std::to_string(
             contract.ownership_arc_weak_unowned_conflict_diagnostic_sites) +
         ";ownership_arc_empty_fixit_hint_sites=" +
         std::to_string(contract.ownership_arc_empty_fixit_hint_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ArcDiagnosticsFixitLoweringLaneContract;
}
