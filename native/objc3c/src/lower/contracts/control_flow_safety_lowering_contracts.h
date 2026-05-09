#pragma once

#include <cstddef>
#include <string>

// Control-flow safety lowering owns guard, match, and defer lowering contracts
// and their fail-closed replay surface.
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringLaneContract =
    "objc3c.control_flow.control.flow.safety.lowering.v1";

inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringContractId =
    "objc3c.control_flow.control.flow.safety.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_control_flow_control_flow_safety_lowering_contract";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringGuardModel =
    "native-lowering-executes-guard-clauses-via-short-circuit-control-flow-and-else-edge-cleanup";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringMatchModel =
    "native-lowering-executes-literal-default-wildcard-and-binding-match-arms-while-result-case-patterns-remain-explicitly-fail-closed";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringDeferModel =
    "native-lowering-registers-defer-cleanups-per-scope-and-emits-lifo-cleanup-insertion-on-scope-exit";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringAuthorityModel =
    "control_flow-source-closure-plus-control_flow-semantic-model-own-the-current-lowering-boundary";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringFailClosedModel =
    "native-ir-emission-fails-closed-with-o3l300-on-result-case-match-patterns-until-a-runtime-result-payload-abi-lands";

struct Objc3ControlFlowControlFlowSafetyLoweringContract {
  std::size_t guard_statement_sites = 0;
  std::size_t guard_clause_sites = 0;
  std::size_t match_statement_sites = 0;
  std::size_t defer_statement_sites = 0;
  std::size_t live_guard_short_circuit_sites = 0;
  std::size_t live_match_dispatch_sites = 0;
  std::size_t live_defer_cleanup_sites = 0;
  std::size_t fail_closed_guard_short_circuit_sites = 0;
  std::size_t fail_closed_match_dispatch_sites = 0;
  std::size_t fail_closed_defer_cleanup_sites = 0;
  std::size_t deterministic_fail_closed_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

std::string Objc3ControlFlowControlFlowSafetyLoweringSummary();
bool IsValidObjc3ControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
std::string Objc3ControlFlowControlFlowSafetyLoweringReplayKey(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
