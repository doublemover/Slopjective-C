#pragma once

#include <cstddef>
#include <string>

// Error throws/unwind contracts own hidden error-out ABI propagation,
// do/catch control-flow lowering, and cleanup/unwind replay facts.
inline constexpr const char *kObjc3ThrowsPropagationLoweringLaneContract =
    "objc3c.throws.propagation.lowering.v1";
inline constexpr const char *kObjc3UnwindCleanupLoweringLaneContract =
    "objc3c.unwind.cleanup.lowering.v1";

inline constexpr const char
    *kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId =
        "objc3c.error_handling.throws.abi.propagation.lowering.v1";
inline constexpr const char
    *kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel =
        "error_handling-semantic-packets-feed-runnable-error-out-abi-propagation-and-catch-dispatch-lowering";
inline constexpr const char
    *kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel =
        "native-lowering-emits-hidden-error-out-abi-propagation-operators-and-do-catch-control-flow-through-real-ir-and-object-artifacts";
inline constexpr const char
    *kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel =
        "generalized-foreign-exception-abi-and-runtime-bridge-helper-contract-remain-deferred-to-the-error_handling-error-runtime-bridge-helper-boundary";
inline constexpr const char
    *kObjc3ErrorHandlingThrowsAbiPropagationLoweringNonGoalModel =
        "no-generalized-foreign-exception-abi-no-stable-cross-module-replay-claim-yet";

struct Objc3ThrowsPropagationLoweringContract {
  std::size_t throws_propagation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnwindCleanupLoweringContract {
  std::size_t unwind_cleanup_sites = 0;
  std::size_t unwind_edge_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_emit_sites = 0;
  std::size_t landing_pad_sites = 0;
  std::size_t cleanup_resume_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

std::string Objc3ErrorHandlingThrowsAbiPropagationLoweringSummary();

bool IsValidObjc3ThrowsPropagationLoweringContract(
    const Objc3ThrowsPropagationLoweringContract &contract);
std::string Objc3ThrowsPropagationLoweringReplayKey(
    const Objc3ThrowsPropagationLoweringContract &contract);
bool IsValidObjc3UnwindCleanupLoweringContract(
    const Objc3UnwindCleanupLoweringContract &contract);
std::string Objc3UnwindCleanupLoweringReplayKey(
    const Objc3UnwindCleanupLoweringContract &contract);
