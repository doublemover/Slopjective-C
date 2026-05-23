#pragma once

#include <cstddef>
#include <string>

// Optional/keypath lowering owns optional binding/send/coalescing, typed
// keypath lowering, nil short-circuiting, descriptor publication, and the
// runtime helper handoff boundary for unsupported keypath shapes.
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringLaneContract =
    "objc3c.type_system.optional.keypath.lowering.v1";

inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringContractId =
    "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringOptionalModel =
    "optional-bindings-sends-optional-member-access-and-coalescing-lower-natively-with-single-evaluation-and-nil-short-circuit";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel =
    "validated-single-component-typed-keypath-literals-lower-to-canonical-runtime-descriptor-handles-with-generic-metadata-preservation";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel =
    "type_system-semantic-summary-plus-message-send-selector-dispatch-and-nil-receiver-lowering-contracts";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel =
    "native-lowering-fails-closed-on-lowering-contract-drift-and-on-semantically-unsupported-typed-keypath-shapes";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringDebugEvidenceModel =
    "live-typed-keypath-artifacts-require-descriptor-source-map-and-runtime-handle-evidence";
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
  std::size_t typed_keypath_descriptor_publication_sites = 0;
  std::size_t typed_keypath_source_map_evidence_sites = 0;
  std::size_t typed_keypath_runtime_handle_evidence_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

std::string Objc3TypeSystemOptionalKeypathLoweringSummary();
std::string Objc3TypeSystemOptionalKeypathRuntimeHelperContractSummary();
bool IsValidObjc3TypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract);
std::string Objc3TypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract);
