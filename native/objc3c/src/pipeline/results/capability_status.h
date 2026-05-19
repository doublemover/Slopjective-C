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
         summary.unsupported_claim_ids.empty() &&
         summary.protocol_optional_partition_source_supported &&
         summary.object_pointer_nullability_source_supported &&
         summary.pragmatic_generic_suffix_source_supported &&
         summary.optional_binding_source_supported &&
         summary.optional_send_source_supported &&
         summary.nil_coalescing_source_supported &&
         summary.typed_keypath_literal_source_supported &&
         !summary.optional_member_access_fail_closed &&
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
         summary.supported_construct_ids.size() == 8 &&
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
             kObjc3ControlFlowSourceSurfaceMatchResultCasePatterns &&
         summary.fail_closed_construct_ids.size() == 3 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3ControlFlowFailClosedConstructMatchExpression &&
         summary.fail_closed_construct_ids[1] ==
             kObjc3ControlFlowFailClosedConstructGuardedPatterns &&
         summary.fail_closed_construct_ids[2] ==
             kObjc3ControlFlowFailClosedConstructMatchTypeTestPatterns &&
         summary.guard_binding_source_supported &&
         summary.guard_condition_list_source_supported &&
         summary.switch_case_pattern_source_supported &&
         summary.defer_statement_source_supported &&
         summary.match_statement_source_supported &&
         summary.match_wildcard_pattern_source_supported &&
         summary.match_literal_pattern_source_supported &&
         summary.match_binding_pattern_source_supported &&
         summary.match_result_case_pattern_source_supported &&
         summary.defer_keyword_reserved && !summary.defer_fail_closed &&
         summary.match_expression_fail_closed &&
         summary.guarded_pattern_fail_closed &&
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
         summary.fail_closed_construct_ids.size() == 3 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3ErrorHandlingFailClosedConstructTryExpressions &&
         summary.fail_closed_construct_ids[1] ==
             kObjc3ErrorHandlingFailClosedConstructThrowStatements &&
         summary.fail_closed_construct_ids[2] ==
             kObjc3ErrorHandlingFailClosedConstructDoCatchStatements &&
         summary.throws_declaration_source_supported &&
         summary.result_carrier_source_supported &&
         summary.ns_error_bridging_source_supported &&
         summary.error_bridge_marker_source_supported &&
         summary.try_keyword_reserved && summary.throw_keyword_reserved &&
         summary.catch_keyword_reserved && summary.try_fail_closed &&
         summary.throw_fail_closed && summary.do_catch_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
