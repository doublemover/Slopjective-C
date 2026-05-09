#pragma once

#include <cstddef>
#include <string>

// Optional/keypath and control-flow safety lowering own the native short-circuit
// and fail-closed control contracts shared by typed keypath and guard/match/defer
// surfaces.
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringLaneContract =
    "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringLaneContract =
    "objc3c.control_flow.control.flow.safety.lowering.v1";

inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringContractId =
    "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringContractId =
    "objc3c.control_flow.control.flow.safety.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_control_flow_control_flow_safety_lowering_contract";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringOptionalModel =
    "optional-bindings-sends-optional-member-access-and-coalescing-lower-natively-with-single-evaluation-and-nil-short-circuit";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel =
    "validated-single-component-typed-keypath-literals-lower-to-canonical-runtime-descriptor-handles-with-generic-metadata-preservation";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel =
    "type_system-semantic-summary-plus-message-send-selector-dispatch-and-nil-receiver-lowering-contracts";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel =
    "native-lowering-fails-closed-on-lowering-contract-drift-and-on-semantically-unsupported-typed-keypath-shapes";
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
inline constexpr const char *kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId =
    "objc3c.type_system.optional.keypath.runtime.helper.contract.v1";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_runtime_helper_contract";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperOptionalModel =
        "optional-send-and-optional-member-access-sites-use-lowering-owned-nil-short-circuit-plus-public-runtime-selector-lookup-dispatch";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel =
        "validated-single-component-typed-keypath-sites-publish-stable-descriptor-handles-and-retained-descriptor-sections-while-runtime-evaluation-helpers-remain-a-follow-on-private-runtime-step";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperDiagnosticModel =
        "unsupported-typed-keypath-shapes-and-non-objc-optional-member-access-fail-closed-before-runtime";

struct Objc3TypeSystemOptionalKeypathLoweringContract {
  std::size_t optional_binding_sites = 0;
  std::size_t optional_binding_clause_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  std::size_t typed_keypath_self_root_sites = 0;
  std::size_t typed_keypath_class_root_sites = 0;
  std::size_t live_optional_lowering_sites = 0;
  std::size_t single_evaluation_nil_short_circuit_sites = 0;
  std::size_t live_typed_keypath_artifact_sites = 0;
  std::size_t deferred_typed_keypath_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

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

std::string Objc3TypeSystemOptionalKeypathLoweringSummary();
std::string Objc3ControlFlowControlFlowSafetyLoweringSummary();
std::string Objc3TypeSystemOptionalKeypathRuntimeHelperContractSummary();
bool IsValidObjc3TypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract);
std::string Objc3TypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract);
bool IsValidObjc3ControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
std::string Objc3ControlFlowControlFlowSafetyLoweringReplayKey(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
