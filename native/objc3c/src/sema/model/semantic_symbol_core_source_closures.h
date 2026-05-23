#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "contracts/objc3_language_evolution_reserved_diagnostic_codes.h"
#include "sema/model/frontend_linkage_summaries.h"
#include "sema/model/frontend_type_source_closure.h"

inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureContractId =
    "objc3c.control_flow.control.flow.source.closure.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_control_flow_control_flow_source_closure";
inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureSourceModel =
    "guard-condition-lists-defer-statements-statement-match-patterns-guarded-statement-match-patterns-and-expression-match-arms-are-live-frontend-owned-control-flow-surfaces-while-type-test-patterns-remain-fail-closed";
inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureFailureModel =
    "type-test-patterns-remain-fail-closed-until-later-sema-lowering-and-runtime-work";

struct Objc3FrontendControlFlowControlFlowSourceClosureSummary {
  std::string contract_id = kObjc3ControlFlowControlFlowSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3ControlFlowControlFlowSourceClosureSurfacePath;
  std::string source_model = kObjc3ControlFlowControlFlowSourceClosureSourceModel;
  std::string failure_model = kObjc3ControlFlowControlFlowSourceClosureFailureModel;
  std::vector<std::string> supported_construct_ids = {
      kObjc3ControlFlowSourceSurfaceGuardBindings,
      kObjc3ControlFlowSourceSurfaceGuardConditionLists,
      kObjc3ControlFlowSourceSurfaceSwitchCasePatterns,
      kObjc3ControlFlowSourceSurfaceDeferStatements,
      kObjc3ControlFlowSourceSurfaceMatchStatement,
      kObjc3ControlFlowSourceSurfaceMatchWildcardPatterns,
      kObjc3ControlFlowSourceSurfaceMatchLiteralPatterns,
      kObjc3ControlFlowSourceSurfaceMatchBindingPatterns,
      kObjc3ControlFlowSourceSurfaceGuardedMatchPatterns,
      kObjc3ControlFlowSourceSurfaceMatchResultCasePatterns,
      kObjc3ControlFlowSourceSurfaceMatchExpression,
  };
  std::vector<std::string> fail_closed_construct_ids = {
      kObjc3ControlFlowFailClosedConstructMatchTypeTestPatterns,
  };
  std::size_t guard_binding_sites = 0;
  std::size_t guard_binding_clause_sites = 0;
  std::size_t guard_boolean_condition_sites = 0;
  std::size_t switch_case_pattern_sites = 0;
  std::size_t switch_default_pattern_sites = 0;
  std::size_t defer_keyword_sites = 0;
  std::size_t match_statement_sites = 0;
  std::size_t match_case_pattern_sites = 0;
  std::size_t match_default_sites = 0;
  std::size_t match_wildcard_pattern_sites = 0;
  std::size_t match_literal_pattern_sites = 0;
  std::size_t match_binding_pattern_sites = 0;
  std::size_t guarded_match_pattern_sites = 0;
  std::size_t match_result_case_pattern_sites = 0;
  std::size_t match_expression_sites = 0;
  std::size_t match_expression_arm_sites = 0;
  std::size_t match_expression_guard_sites = 0;
  std::size_t guarded_match_issue_ref = 8236;
  std::string guarded_match_admitted_syntax =
      "case pattern where bool_condition:";
  bool guarded_match_condition_bool_required = true;
  bool guard_binding_source_supported = false;
  bool guard_condition_list_source_supported = false;
  bool switch_case_pattern_source_supported = false;
  bool match_statement_source_supported = false;
  bool match_wildcard_pattern_source_supported = false;
  bool match_literal_pattern_source_supported = false;
  bool match_binding_pattern_source_supported = false;
  bool guarded_match_pattern_source_supported = false;
  bool match_result_case_pattern_source_supported = false;
  bool match_expression_source_supported = false;
  bool defer_statement_source_supported = false;
  bool defer_keyword_reserved = false;
  bool defer_fail_closed = false;
  bool match_expression_fail_closed = false;
  bool match_expression_result_typing_supported = false;
  bool match_fat_arrow_arms_supported = false;
  bool type_test_pattern_fail_closed = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureContractId =
    "objc3c.error_handling.error.source.closure.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_error_source_closure";
inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureSourceModel =
    "bare-throws-declarations-single-payload-typed-throws-result-carrier-profiles-nserror-bridging-profiles-and-canonical-error-bridge-markers-are-live-frontend-owned-source-surfaces-while-typed-throws-abi-lowering-try-throw-and-do-catch-remain-fail-closed";
inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureFailureModel =
    "typed-throws-preserves-one-source-payload-through-interface-contracts-but-typed-error-abi-lowering-runtime-execution-try-expressions-throw-statements-and-do-catch-remain-fail-closed-boundaries-until-runnable-error_handling-work";

struct Objc3FrontendErrorHandlingErrorSourceClosureSummary {
  std::string contract_id = kObjc3ErrorHandlingErrorSourceClosureContractId;
  std::string frontend_surface_path = kObjc3ErrorHandlingErrorSourceClosureSurfacePath;
  std::string source_model = kObjc3ErrorHandlingErrorSourceClosureSourceModel;
  std::string failure_model = kObjc3ErrorHandlingErrorSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimThrowsDeclarations,
      kObjc3SourceOnlyFeatureClaimTypedThrowsDeclarations,
      kObjc3SourceOnlyFeatureClaimResultCarrierProfiles,
      kObjc3SourceOnlyFeatureClaimNSErrorBridgingProfiles,
  };
  std::vector<std::string> fail_closed_construct_ids = {
      kObjc3ErrorHandlingFailClosedConstructTypedThrowsAbiLowering,
      kObjc3ErrorHandlingFailClosedConstructTryExpressions,
      kObjc3ErrorHandlingFailClosedConstructThrowStatements,
      kObjc3ErrorHandlingFailClosedConstructDoCatchStatements,
  };
  std::size_t function_throws_declaration_sites = 0;
  std::size_t method_throws_declaration_sites = 0;
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t objc_nserror_attribute_sites = 0;
  std::size_t objc_status_code_attribute_sites = 0;
  std::size_t status_code_success_clause_sites = 0;
  std::size_t status_code_error_type_clause_sites = 0;
  std::size_t status_code_mapping_clause_sites = 0;
  std::size_t try_keyword_sites = 0;
  std::size_t throw_keyword_sites = 0;
  std::size_t catch_keyword_sites = 0;
  std::size_t typed_throws_declaration_sites = 0;
  std::size_t typed_throws_payload_arity_supported = 1;
  bool throws_declaration_source_supported = false;
  bool typed_throws_source_supported = false;
  bool result_carrier_source_supported = false;
  bool ns_error_bridging_source_supported = false;
  bool error_bridge_marker_source_supported = false;
  bool try_keyword_reserved = false;
  bool throw_keyword_reserved = false;
  bool catch_keyword_reserved = false;
  bool typed_throws_fail_closed = false;
  bool typed_throws_abi_lowering_fail_closed = false;
  std::size_t typed_throws_issue_ref = 8233;
  std::string typed_throws_canonical_syntax = "throws(E)";
  std::string typed_throws_reserved_diagnostic_code =
      kObjc3ParserDiagnosticReservedTypedThrowsCode;
  bool typed_throws_single_payload_reserved = false;
  bool typed_throws_empty_payload_rejected = true;
  bool typed_throws_multi_payload_rejected = true;
  bool typed_throws_silent_erasure_allowed = false;
  std::string typed_throws_effect_record_status =
      "typed-and-untyped-effects-preserved-with-exact-callable-compatibility";
  std::string typed_throws_abi_status = "typed-error-abi-deferred";
  std::string typed_throws_interface_roundtrip_status =
      "typed-payload-preserved";
  bool try_fail_closed = false;
  bool throw_fail_closed = false;
  bool do_catch_fail_closed = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3ConcurrencyAsyncSourceClosureContractId =
    "objc3c.concurrency.async.source.closure.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_async_source_closure";
inline constexpr const char *kObjc3ConcurrencyAsyncSourceClosureSourceModel =
    "async-entry-await-expression-and-executor-affinity-syntax-are-live-frontend-owned-source-surfaces-while-continuation-lowering-suspension-cleanup-and-runtime-scheduling-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyAsyncSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-runnable-continuation-abi-suspension-cleanup-or-executor-runtime-behavior";
inline constexpr const char *kObjc3ConcurrencyActorMemberIsolationSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_actor_member_and_isolation_source_closure";
inline constexpr const char *kObjc3ConcurrencyActorMemberIsolationSourceClosureSourceModel =
    "actor-class-declarations-actor-members-and-objc-nonisolated-annotations-are-live-frontend-owned-source-surfaces-while-actor-legality-diagnostics-and-runnable-actor-runtime-behavior-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyActorMemberIsolationSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-actor-member-legality-diagnostics-cross-actor-enforcement-or-runnable-actor-runtime-behavior";

struct Objc3FrontendConcurrencyAsyncSourceClosureSummary {
  std::string contract_id = kObjc3ConcurrencyAsyncSourceClosureContractId;
  std::string frontend_surface_path = kObjc3ConcurrencyAsyncSourceClosureSurfacePath;
  std::string source_model = kObjc3ConcurrencyAsyncSourceClosureSourceModel;
  std::string failure_model = kObjc3ConcurrencyAsyncSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimAsyncDeclarations,
      kObjc3SourceOnlyFeatureClaimAwaitExpressions,
      kObjc3SourceOnlyFeatureClaimExecutorAffinityAttributes,
  };
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t async_method_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_expression_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t executor_main_sites = 0;
  std::size_t executor_global_sites = 0;
  std::size_t executor_named_sites = 0;
  bool async_function_source_supported = false;
  bool async_method_source_supported = false;
  bool await_expression_source_supported = false;
  bool executor_attribute_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary {
  std::string contract_id = kObjc3ActorMemberIsolationSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3ConcurrencyActorMemberIsolationSourceClosureSurfacePath;
  std::string source_model =
      kObjc3ConcurrencyActorMemberIsolationSourceClosureSourceModel;
  std::string failure_model =
      kObjc3ConcurrencyActorMemberIsolationSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimActorDeclarationMarkers,
      kObjc3SourceOnlyFeatureClaimActorMemberSurfaces,
      kObjc3SourceOnlyFeatureClaimIsolationAnnotationMarkers,
      kObjc3SourceOnlyFeatureClaimActorMetadataSurfaces,
  };
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_property_sites = 0;
  std::size_t objc_nonisolated_annotation_sites = 0;
  std::size_t actor_member_executor_annotation_sites = 0;
  std::size_t actor_async_method_sites = 0;
  std::size_t actor_member_metadata_sites = 0;
  bool actor_declaration_source_supported = false;
  bool actor_member_source_supported = false;
  bool isolation_annotation_source_supported = false;
  bool actor_metadata_surface_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};
