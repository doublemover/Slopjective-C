#pragma once

#include "lower/core/lowering_primitive_ops.h"
#include "lower/contracts/arc_boundary_lowering_contracts.h"
#include "lower/contracts/block_arc_lowering_plan.h"
#include "lower/contracts/block_runtime_lowering_contracts.h"
#include "lower/contracts/conformance_reporting_contracts.h"
#include "lower/contracts/cross_module_lowering_contracts.h"
#include "lower/contracts/diagnostic_recovery_lowering_contracts.h"
#include "lower/contracts/dispatch_control_lowering_contracts.h"
#include "lower/contracts/dispatch_surface_contracts.h"
#include "lower/contracts/error_handling_lowering_contracts.h"
#include "lower/contracts/executable_layout_lowering_contracts.h"
#include "lower/contracts/function_method_lowering_state.h"
#include "lower/contracts/interop_lowering_contracts.h"
#include "lower/contracts/lowering_concurrency_contracts.h"
#include "lower/contracts/lowering_diagnostics.h"
#include "lower/contracts/lowering_phase_io.h"
#include "lower/contracts/manifest_truth_gate_contracts.h"
#include "lower/contracts/metaprogramming_lowering_contracts.h"
#include "lower/contracts/object_model_lowering_contracts.h"
#include "lower/contracts/optional_control_flow_lowering_contracts.h"
#include "lower/contracts/ownership_runtime_lowering_contracts.h"
#include "lower/contracts/ownership_system_extension_contracts.h"
#include "lower/contracts/runtime_artifact_retention_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"
#include "lower/contracts/runtime_dispatch_boundary_contracts.h"
#include "lower/contracts/runtime_dispatch_lowering_contracts.h"
#include "lower/contracts/runtime_bootstrap_lowering_contracts.h"
#include "lower/contracts/runtime_metadata_emission_contracts.h"
#include "lower/contracts/runtime_metadata_handoff.h"
#include "lower/contracts/runtime_object_support_contracts.h"
#include "lower/contracts/type_system_generic_lowering_contracts.h"
#include "lower/contracts/unsafe_intrinsic_governance_contracts.h"

#include <cstddef>
#include <string>

inline constexpr const char *kObjc3OwnershipQualifierLoweringLaneContract =
    "objc3c.ownership.qualifier.lowering.v1";
inline constexpr const char *kObjc3RetainReleaseOperationLoweringLaneContract =
    "objc3c.retain.release.operation.lowering.v1";
inline constexpr const char *kObjc3AutoreleasePoolScopeLoweringLaneContract =
    "objc3c.autoreleasepool.scope.lowering.v1";
inline constexpr const char *kObjc3WeakUnownedSemanticsLoweringLaneContract =
    "objc3c.weak.unowned.semantics.lowering.v1";
inline constexpr const char *kObjc3ArcDiagnosticsFixitLoweringLaneContract =
    "objc3c.arc.diagnostics.fixit.lowering.v1";
struct Objc3OwnershipQualifierLoweringContract {
  std::size_t ownership_qualifier_sites = 0;
  std::size_t invalid_ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_annotation_sites = 0;
  bool deterministic = true;
};

struct Objc3RetainReleaseOperationLoweringContract {
  std::size_t ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AutoreleasePoolScopeLoweringContract {
  std::size_t scope_sites = 0;
  std::size_t scope_symbolized_sites = 0;
  unsigned max_scope_depth = 0;
  std::size_t scope_entry_transition_sites = 0;
  std::size_t scope_exit_transition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3WeakUnownedSemanticsLoweringContract {
  std::size_t ownership_candidate_sites = 0;
  std::size_t weak_reference_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ArcDiagnosticsFixitLoweringContract {
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract);
std::string Objc3OwnershipQualifierLoweringReplayKey(
    const Objc3OwnershipQualifierLoweringContract &contract);
bool IsValidObjc3RetainReleaseOperationLoweringContract(
    const Objc3RetainReleaseOperationLoweringContract &contract);
std::string Objc3RetainReleaseOperationLoweringReplayKey(
    const Objc3RetainReleaseOperationLoweringContract &contract);
bool IsValidObjc3AutoreleasePoolScopeLoweringContract(
    const Objc3AutoreleasePoolScopeLoweringContract &contract);
std::string Objc3AutoreleasePoolScopeLoweringReplayKey(
    const Objc3AutoreleasePoolScopeLoweringContract &contract);
bool IsValidObjc3WeakUnownedSemanticsLoweringContract(
    const Objc3WeakUnownedSemanticsLoweringContract &contract);
std::string Objc3WeakUnownedSemanticsLoweringReplayKey(
    const Objc3WeakUnownedSemanticsLoweringContract &contract);
bool IsValidObjc3ArcDiagnosticsFixitLoweringContract(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract);
std::string Objc3ArcDiagnosticsFixitLoweringReplayKey(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract);
