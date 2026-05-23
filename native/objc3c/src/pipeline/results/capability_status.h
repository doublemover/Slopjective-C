#pragma once

#include "pipeline/results/compatibility_strictness_status.h"

inline bool IsReadyObjc3FrontendTypeSystemTypeSourceClosureSummary(
    const Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3TypeSystemTypeSourceClosureContractId &&
         summary.frontend_surface_path == kObjc3TypeSystemTypeSourceClosureSurfacePath &&
         summary.source_model == kObjc3TypeSystemTypeSourceClosureSourceModel &&
         summary.failure_model == kObjc3TypeSystemTypeSourceClosureFailureModel &&
         summary.source_only_claim_ids.size() == 7 &&
         summary.source_only_claim_ids[0] ==
             kObjc3SourceOnlyFeatureClaimProtocolOptionalPartitions &&
         summary.source_only_claim_ids[1] ==
             kObjc3SourceOnlyFeatureClaimObjectPointerNullabilitySuffixes &&
         summary.source_only_claim_ids[2] ==
             kObjc3SourceOnlyFeatureClaimPragmaticGenericSuffixes &&
         summary.source_only_claim_ids[3] ==
             kObjc3SourceOnlyFeatureClaimOptionalBindings &&
         summary.source_only_claim_ids[4] ==
             kObjc3SourceOnlyFeatureClaimOptionalSends &&
         summary.source_only_claim_ids[5] ==
             kObjc3SourceOnlyFeatureClaimNilCoalescing &&
         summary.source_only_claim_ids[6] ==
             kObjc3SourceOnlyFeatureClaimTypedKeyPathLiterals &&
         summary.unsupported_claim_ids.size() == 1 &&
         summary.unsupported_claim_ids[0] ==
             kObjc3UnsupportedFeatureClaimValueOptionals &&
         summary.fail_closed_construct_ids.size() == 1 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3TypeSystemFailClosedConstructValueOptionals &&
         summary.protocol_optional_partition_source_supported &&
         summary.object_pointer_nullability_source_supported &&
         summary.pragmatic_generic_suffix_source_supported &&
         summary.optional_binding_source_supported &&
         summary.optional_send_source_supported &&
         summary.nil_coalescing_source_supported &&
         summary.typed_keypath_literal_source_supported &&
         !summary.optional_member_access_fail_closed &&
         summary.value_optional_type_fail_closed &&
         summary.value_optional_issue_ref == 8234u &&
         summary.value_optional_canonical_spelling == "Optional<T>" &&
         summary.lowercase_optional_alias_rejected &&
         !summary.value_optional_nil_to_scalar_coercion_allowed &&
         !summary.value_optional_nullable_pointer_conversion_allowed &&
         !summary.value_optional_throws_conversion_allowed &&
         summary.value_optional_abi_status == "reserved-no-layout" &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendControlFlowControlFlowSourceClosureSummary(
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3ControlFlowControlFlowSourceClosureContractId &&
         summary.frontend_surface_path ==
             kObjc3ControlFlowControlFlowSourceClosureSurfacePath &&
         summary.source_model == kObjc3ControlFlowControlFlowSourceClosureSourceModel &&
         summary.failure_model ==
             kObjc3ControlFlowControlFlowSourceClosureFailureModel &&
         summary.supported_construct_ids.size() == 10 &&
         summary.supported_construct_ids[0] ==
             kObjc3ControlFlowSourceSurfaceGuardBindings &&
         summary.supported_construct_ids[1] ==
             kObjc3ControlFlowSourceSurfaceGuardConditionLists &&
         summary.supported_construct_ids[2] ==
             kObjc3ControlFlowSourceSurfaceSwitchCasePatterns &&
         summary.supported_construct_ids[3] ==
             kObjc3ControlFlowSourceSurfaceDeferStatements &&
         summary.supported_construct_ids[4] ==
             kObjc3ControlFlowSourceSurfaceMatchStatement &&
         summary.supported_construct_ids[5] ==
             kObjc3ControlFlowSourceSurfaceMatchWildcardPatterns &&
         summary.supported_construct_ids[6] ==
             kObjc3ControlFlowSourceSurfaceMatchLiteralPatterns &&
         summary.supported_construct_ids[7] ==
             kObjc3ControlFlowSourceSurfaceMatchBindingPatterns &&
         summary.supported_construct_ids[8] ==
             kObjc3ControlFlowSourceSurfaceGuardedMatchPatterns &&
         summary.supported_construct_ids[9] ==
             kObjc3ControlFlowSourceSurfaceMatchResultCasePatterns &&
         summary.fail_closed_construct_ids.size() == 2 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3ControlFlowFailClosedConstructMatchExpression &&
         summary.fail_closed_construct_ids[1] ==
             kObjc3ControlFlowFailClosedConstructMatchTypeTestPatterns &&
         summary.guard_binding_source_supported &&
         summary.guard_condition_list_source_supported &&
         summary.switch_case_pattern_source_supported &&
         summary.defer_statement_source_supported &&
         summary.match_statement_source_supported &&
         summary.match_wildcard_pattern_source_supported &&
         summary.match_literal_pattern_source_supported &&
         summary.match_binding_pattern_source_supported &&
         summary.guarded_match_pattern_source_supported &&
         summary.match_result_case_pattern_source_supported &&
         summary.defer_keyword_reserved && !summary.defer_fail_closed &&
         summary.guarded_match_issue_ref == 8236u &&
         summary.guarded_match_admitted_syntax ==
             "case pattern where bool_condition:" &&
         summary.guarded_match_condition_bool_required &&
         summary.match_expression_fail_closed &&
         !summary.match_expression_result_typing_supported &&
         !summary.match_fat_arrow_arms_supported &&
         summary.type_test_pattern_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendErrorHandlingErrorSourceClosureSummary(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3ErrorHandlingErrorSourceClosureContractId &&
         summary.frontend_surface_path ==
             kObjc3ErrorHandlingErrorSourceClosureSurfacePath &&
         summary.source_model == kObjc3ErrorHandlingErrorSourceClosureSourceModel &&
         summary.failure_model == kObjc3ErrorHandlingErrorSourceClosureFailureModel &&
         summary.source_only_claim_ids.size() == 3 &&
         summary.source_only_claim_ids[0] ==
             kObjc3SourceOnlyFeatureClaimThrowsDeclarations &&
         summary.source_only_claim_ids[1] ==
             kObjc3SourceOnlyFeatureClaimResultCarrierProfiles &&
         summary.source_only_claim_ids[2] ==
             kObjc3SourceOnlyFeatureClaimNSErrorBridgingProfiles &&
         summary.fail_closed_construct_ids.size() == 4 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3ErrorHandlingFailClosedConstructTypedThrows &&
         summary.fail_closed_construct_ids[1] ==
             kObjc3ErrorHandlingFailClosedConstructTryExpressions &&
         summary.fail_closed_construct_ids[2] ==
             kObjc3ErrorHandlingFailClosedConstructThrowStatements &&
         summary.fail_closed_construct_ids[3] ==
             kObjc3ErrorHandlingFailClosedConstructDoCatchStatements &&
         summary.throws_declaration_source_supported &&
         summary.result_carrier_source_supported &&
         summary.ns_error_bridging_source_supported &&
         summary.error_bridge_marker_source_supported &&
         summary.try_keyword_reserved && summary.throw_keyword_reserved &&
         summary.catch_keyword_reserved && summary.typed_throws_fail_closed &&
         summary.typed_throws_issue_ref == 8233u &&
         summary.typed_throws_canonical_syntax == "throws(E)" &&
         summary.typed_throws_single_payload_reserved &&
         summary.typed_throws_empty_payload_rejected &&
         summary.typed_throws_multi_payload_rejected &&
         !summary.typed_throws_silent_erasure_allowed &&
         summary.typed_throws_effect_record_status ==
             "bare-throws-untyped-only" &&
         summary.typed_throws_abi_status == "reserved-no-lowering" &&
         summary.try_fail_closed &&
         summary.throw_fail_closed && summary.do_catch_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
