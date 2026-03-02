#pragma once

#include <sstream>
#include <string>

#include "lower/objc3_lowering_contract.h"

struct Objc3OwnershipAwareLoweringBehaviorScaffold {
  bool ownership_qualifier_contract_ready = false;
  bool retain_release_contract_ready = false;
  bool autoreleasepool_scope_contract_ready = false;
  bool arc_diagnostics_fixit_contract_ready = false;
  bool weak_unowned_semantics_contract_ready = false;
  bool ownership_profile_accounting_consistent = false;
  bool replay_keys_ready = false;
  bool deterministic_replay_surface = false;
  bool modular_split_ready = false;
  bool expansion_replay_keys_ready = false;
  bool expansion_deterministic_replay_surface = false;
  bool expansion_ready = false;
  std::string ownership_qualifier_replay_key;
  std::string retain_release_replay_key;
  std::string autoreleasepool_scope_replay_key;
  std::string weak_unowned_semantics_replay_key;
  std::string arc_diagnostics_fixit_replay_key;
  std::string scaffold_key;
  std::string expansion_key;
  std::string failure_reason;
};

inline bool HasOwnershipLaneContractReplaySuffix(const std::string &replay_key, const char *lane_contract) {
  const std::string expected_suffix = std::string(";lane_contract=") + lane_contract;
  return replay_key.find(expected_suffix) != std::string::npos;
}

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorScaffoldKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-modular-split-scaffold:v1:"
      << "ownership_qualifier_contract_ready="
      << (scaffold.ownership_qualifier_contract_ready ? "true" : "false")
      << ";retain_release_contract_ready="
      << (scaffold.retain_release_contract_ready ? "true" : "false")
      << ";autoreleasepool_scope_contract_ready="
      << (scaffold.autoreleasepool_scope_contract_ready ? "true" : "false")
      << ";arc_diagnostics_fixit_contract_ready="
      << (scaffold.arc_diagnostics_fixit_contract_ready ? "true" : "false")
      << ";replay_keys_ready=" << (scaffold.replay_keys_ready ? "true" : "false")
      << ";deterministic_replay_surface="
      << (scaffold.deterministic_replay_surface ? "true" : "false")
      << ";modular_split_ready=" << (scaffold.modular_split_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionKey(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold) {
  std::ostringstream key;
  key << "ownership-aware-lowering-core-feature-expansion:v1:"
      << "modular_split_ready=" << (scaffold.modular_split_ready ? "true" : "false")
      << ";weak_unowned_semantics_contract_ready="
      << (scaffold.weak_unowned_semantics_contract_ready ? "true" : "false")
      << ";ownership_profile_accounting_consistent="
      << (scaffold.ownership_profile_accounting_consistent ? "true" : "false")
      << ";expansion_replay_keys_ready="
      << (scaffold.expansion_replay_keys_ready ? "true" : "false")
      << ";expansion_deterministic_replay_surface="
      << (scaffold.expansion_deterministic_replay_surface ? "true" : "false")
      << ";expansion_ready=" << (scaffold.expansion_ready ? "true" : "false")
      << ";scaffold_key=" << scaffold.scaffold_key
      << ";weak_unowned_semantics_replay_key="
      << scaffold.weak_unowned_semantics_replay_key;
  return key.str();
}

inline Objc3OwnershipAwareLoweringBehaviorScaffold BuildObjc3OwnershipAwareLoweringBehaviorScaffold(
    const Objc3OwnershipQualifierLoweringContract &ownership_qualifier_contract,
    const std::string &ownership_qualifier_replay_key,
    const Objc3RetainReleaseOperationLoweringContract &retain_release_contract,
    const std::string &retain_release_replay_key,
    const Objc3AutoreleasePoolScopeLoweringContract &autoreleasepool_scope_contract,
    const std::string &autoreleasepool_scope_replay_key,
    const Objc3WeakUnownedSemanticsLoweringContract &weak_unowned_semantics_contract,
    const std::string &weak_unowned_semantics_replay_key,
    const Objc3ArcDiagnosticsFixitLoweringContract &arc_diagnostics_fixit_contract,
    const std::string &arc_diagnostics_fixit_replay_key) {
  Objc3OwnershipAwareLoweringBehaviorScaffold scaffold;
  scaffold.ownership_qualifier_contract_ready =
      IsValidObjc3OwnershipQualifierLoweringContract(ownership_qualifier_contract) &&
      ownership_qualifier_contract.deterministic;
  scaffold.retain_release_contract_ready =
      IsValidObjc3RetainReleaseOperationLoweringContract(retain_release_contract) &&
      retain_release_contract.deterministic;
  scaffold.autoreleasepool_scope_contract_ready =
      IsValidObjc3AutoreleasePoolScopeLoweringContract(autoreleasepool_scope_contract) &&
      autoreleasepool_scope_contract.deterministic;
  scaffold.weak_unowned_semantics_contract_ready =
      IsValidObjc3WeakUnownedSemanticsLoweringContract(weak_unowned_semantics_contract) &&
      weak_unowned_semantics_contract.deterministic;
  scaffold.arc_diagnostics_fixit_contract_ready =
      IsValidObjc3ArcDiagnosticsFixitLoweringContract(arc_diagnostics_fixit_contract) &&
      arc_diagnostics_fixit_contract.deterministic;
  scaffold.ownership_qualifier_replay_key = ownership_qualifier_replay_key;
  scaffold.retain_release_replay_key = retain_release_replay_key;
  scaffold.autoreleasepool_scope_replay_key = autoreleasepool_scope_replay_key;
  scaffold.weak_unowned_semantics_replay_key = weak_unowned_semantics_replay_key;
  scaffold.arc_diagnostics_fixit_replay_key = arc_diagnostics_fixit_replay_key;
  scaffold.ownership_profile_accounting_consistent =
      ownership_qualifier_contract.object_pointer_type_annotation_sites >=
          ownership_qualifier_contract.ownership_qualifier_sites &&
      retain_release_contract.retain_insertion_sites <=
          retain_release_contract.ownership_qualified_sites +
              retain_release_contract.contract_violation_sites &&
      retain_release_contract.release_insertion_sites <=
          retain_release_contract.ownership_qualified_sites +
              retain_release_contract.contract_violation_sites &&
      retain_release_contract.autorelease_insertion_sites <=
          retain_release_contract.ownership_qualified_sites +
              retain_release_contract.contract_violation_sites &&
      weak_unowned_semantics_contract.contract_violation_sites <=
          weak_unowned_semantics_contract.ownership_candidate_sites +
              weak_unowned_semantics_contract.weak_unowned_conflict_sites &&
      arc_diagnostics_fixit_contract.ownership_arc_fixit_available_sites <=
          arc_diagnostics_fixit_contract.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_fixit_contract.contract_violation_sites;
  scaffold.replay_keys_ready =
      !scaffold.ownership_qualifier_replay_key.empty() &&
      !scaffold.retain_release_replay_key.empty() &&
      !scaffold.autoreleasepool_scope_replay_key.empty() &&
      !scaffold.arc_diagnostics_fixit_replay_key.empty();
  scaffold.deterministic_replay_surface =
      HasOwnershipLaneContractReplaySuffix(scaffold.ownership_qualifier_replay_key,
                                           kObjc3OwnershipQualifierLoweringLaneContract) &&
      HasOwnershipLaneContractReplaySuffix(scaffold.retain_release_replay_key,
                                           kObjc3RetainReleaseOperationLoweringLaneContract) &&
      HasOwnershipLaneContractReplaySuffix(scaffold.autoreleasepool_scope_replay_key,
                                           kObjc3AutoreleasePoolScopeLoweringLaneContract) &&
      HasOwnershipLaneContractReplaySuffix(scaffold.arc_diagnostics_fixit_replay_key,
                                           kObjc3ArcDiagnosticsFixitLoweringLaneContract);
  scaffold.modular_split_ready =
      scaffold.ownership_qualifier_contract_ready &&
      scaffold.retain_release_contract_ready &&
      scaffold.autoreleasepool_scope_contract_ready &&
      scaffold.arc_diagnostics_fixit_contract_ready &&
      scaffold.replay_keys_ready &&
      scaffold.deterministic_replay_surface;
  scaffold.scaffold_key = BuildObjc3OwnershipAwareLoweringBehaviorScaffoldKey(scaffold);
  scaffold.expansion_replay_keys_ready =
      scaffold.replay_keys_ready &&
      !scaffold.weak_unowned_semantics_replay_key.empty();
  scaffold.expansion_deterministic_replay_surface =
      scaffold.deterministic_replay_surface &&
      HasOwnershipLaneContractReplaySuffix(
          scaffold.weak_unowned_semantics_replay_key,
          kObjc3WeakUnownedSemanticsLoweringLaneContract);
  scaffold.expansion_ready =
      scaffold.modular_split_ready &&
      scaffold.weak_unowned_semantics_contract_ready &&
      scaffold.ownership_profile_accounting_consistent &&
      scaffold.expansion_replay_keys_ready &&
      scaffold.expansion_deterministic_replay_surface;
  scaffold.expansion_key =
      BuildObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionKey(scaffold);

  if (scaffold.modular_split_ready && scaffold.expansion_ready) {
    return scaffold;
  }

  if (!scaffold.ownership_qualifier_contract_ready) {
    scaffold.failure_reason = "ownership qualifier lowering contract is not ready";
  } else if (!scaffold.retain_release_contract_ready) {
    scaffold.failure_reason = "retain/release lowering contract is not ready";
  } else if (!scaffold.autoreleasepool_scope_contract_ready) {
    scaffold.failure_reason = "autoreleasepool scope lowering contract is not ready";
  } else if (!scaffold.arc_diagnostics_fixit_contract_ready) {
    scaffold.failure_reason = "ARC diagnostics/fix-it lowering contract is not ready";
  } else if (!scaffold.replay_keys_ready) {
    scaffold.failure_reason = "ownership-aware lowering replay keys are incomplete";
  } else if (!scaffold.deterministic_replay_surface) {
    scaffold.failure_reason = "ownership-aware lowering replay keys are not lane-contract deterministic";
  } else if (!scaffold.weak_unowned_semantics_contract_ready) {
    scaffold.failure_reason = "weak/unowned semantics lowering contract is not ready";
  } else if (!scaffold.ownership_profile_accounting_consistent) {
    scaffold.failure_reason = "ownership-aware lowering expansion accounting is inconsistent";
  } else if (!scaffold.expansion_replay_keys_ready) {
    scaffold.failure_reason = "ownership-aware lowering expansion replay keys are incomplete";
  } else if (!scaffold.expansion_deterministic_replay_surface) {
    scaffold.failure_reason = "ownership-aware lowering expansion replay keys are not lane-contract deterministic";
  } else if (!scaffold.expansion_ready) {
    scaffold.failure_reason = "ownership-aware lowering core feature expansion is not ready";
  } else {
    scaffold.failure_reason = "ownership-aware lowering modular split scaffold not ready";
  }

  return scaffold;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.modular_split_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering modular split scaffold not ready"
               : scaffold.failure_reason;
  return false;
}

inline bool IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.expansion_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering core feature expansion is not ready"
               : scaffold.failure_reason;
  return false;
}
