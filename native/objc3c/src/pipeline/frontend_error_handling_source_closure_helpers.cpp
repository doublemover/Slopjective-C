#include "pipeline/frontend_error_handling_source_closure_helpers.h"

#include "pipeline/frontend_source_closure_replay_keys.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendErrorHandlingErrorSourceClosureSummary
BuildErrorHandlingErrorSourceClosureSummary(
    const Objc3Program &program,
    const std::vector<Objc3LexToken> &tokens) {
  Objc3FrontendErrorHandlingErrorSourceClosureSummary summary;
  for (const auto &token : tokens) {
    if (token.kind == Objc3LexTokenKind::KwTry) {
      ++summary.try_keyword_sites;
    } else if (token.kind == Objc3LexTokenKind::KwThrow) {
      ++summary.throw_keyword_sites;
    } else if (token.kind == Objc3LexTokenKind::KwCatch) {
      ++summary.catch_keyword_sites;
    }
  }

  bool throws_profiles_normalized = true;
  bool result_profiles_normalized = true;
  bool ns_error_profiles_normalized = true;
  for (const auto &fn : program.functions) {
    if (fn.throws_declared) {
      ++summary.function_throws_declaration_sites;
    }
    if (fn.typed_throws_declared) {
      ++summary.typed_throws_declaration_sites;
    }
    throws_profiles_normalized =
        throws_profiles_normalized &&
        fn.throws_declaration_profile_is_normalized;
    result_profiles_normalized =
        result_profiles_normalized && fn.result_like_profile_is_normalized;
    ns_error_profiles_normalized =
        ns_error_profiles_normalized &&
        fn.ns_error_bridging_profile_is_normalized;
    summary.result_like_sites += fn.result_like_sites;
    summary.result_success_sites += fn.result_success_sites;
    summary.result_failure_sites += fn.result_failure_sites;
    summary.result_branch_sites += fn.result_branch_sites;
    summary.result_payload_sites += fn.result_payload_sites;
    summary.ns_error_bridging_sites += fn.ns_error_bridging_sites;
    summary.ns_error_out_parameter_sites += fn.ns_error_out_parameter_sites;
    summary.ns_error_bridge_path_sites += fn.ns_error_bridge_path_sites;
    summary.objc_nserror_attribute_sites += fn.objc_nserror_attribute_sites;
    summary.objc_status_code_attribute_sites +=
        fn.objc_status_code_attribute_sites;
    summary.status_code_success_clause_sites +=
        fn.status_code_success_clause_sites;
    summary.status_code_error_type_clause_sites +=
        fn.status_code_error_type_clause_sites;
    summary.status_code_mapping_clause_sites +=
        fn.status_code_mapping_clause_sites;
    throws_profiles_normalized =
        throws_profiles_normalized &&
        fn.error_bridge_marker_profile_is_normalized;
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      if (method.throws_declared) {
        ++summary.method_throws_declaration_sites;
      }
      if (method.typed_throws_declared) {
        ++summary.typed_throws_declaration_sites;
      }
      throws_profiles_normalized =
          throws_profiles_normalized &&
          method.throws_declaration_profile_is_normalized;
      result_profiles_normalized =
          result_profiles_normalized &&
          method.result_like_profile_is_normalized;
      ns_error_profiles_normalized =
          ns_error_profiles_normalized &&
          method.ns_error_bridging_profile_is_normalized;
      summary.result_like_sites += method.result_like_sites;
      summary.result_success_sites += method.result_success_sites;
      summary.result_failure_sites += method.result_failure_sites;
      summary.result_branch_sites += method.result_branch_sites;
      summary.result_payload_sites += method.result_payload_sites;
      summary.ns_error_bridging_sites += method.ns_error_bridging_sites;
      summary.ns_error_out_parameter_sites +=
          method.ns_error_out_parameter_sites;
      summary.ns_error_bridge_path_sites += method.ns_error_bridge_path_sites;
      summary.objc_nserror_attribute_sites +=
          method.objc_nserror_attribute_sites;
      summary.objc_status_code_attribute_sites +=
          method.objc_status_code_attribute_sites;
      summary.status_code_success_clause_sites +=
          method.status_code_success_clause_sites;
      summary.status_code_error_type_clause_sites +=
          method.status_code_error_type_clause_sites;
      summary.status_code_mapping_clause_sites +=
          method.status_code_mapping_clause_sites;
      throws_profiles_normalized =
          throws_profiles_normalized &&
          method.error_bridge_marker_profile_is_normalized;
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      if (method.typed_throws_declared) {
        ++summary.typed_throws_declaration_sites;
      }
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      if (method.typed_throws_declared) {
        ++summary.typed_throws_declaration_sites;
      }
    }
  }

  summary.throws_declaration_source_supported = true;
  summary.typed_throws_source_supported = true;
  summary.result_carrier_source_supported = true;
  summary.ns_error_bridging_source_supported = true;
  summary.error_bridge_marker_source_supported = true;
  summary.try_keyword_reserved = true;
  summary.throw_keyword_reserved = true;
  summary.catch_keyword_reserved = true;
  summary.typed_throws_fail_closed = true;
  summary.typed_throws_abi_lowering_fail_closed = false;
  summary.typed_throws_single_payload_reserved = false;
  summary.typed_throws_effect_record_status =
      "typed-and-untyped-effects-preserved-with-exact-callable-compatibility";
  summary.typed_throws_abi_status = "typed-error-out-abi";
  summary.typed_throws_interface_roundtrip_status = "typed-payload-preserved";
  summary.typed_throws_catch_compatibility_status =
      "typed-catch-exact-untyped-id-error-bridge-incompatible-rejects";
  summary.typed_throws_bridge_to_id_error_policy =
      "explicit-bridge-to-id<Error>-only";
  summary.typed_throws_foreign_carrier_fail_closed = true;
  summary.try_fail_closed = true;
  summary.throw_fail_closed = true;
  summary.do_catch_fail_closed = true;
  summary.deterministic_handoff =
      throws_profiles_normalized && result_profiles_normalized &&
      ns_error_profiles_normalized &&
      summary.status_code_success_clause_sites <=
          summary.objc_status_code_attribute_sites &&
      summary.status_code_error_type_clause_sites <=
          summary.objc_status_code_attribute_sites &&
      summary.status_code_mapping_clause_sites <=
          summary.objc_status_code_attribute_sites &&
      summary.result_success_sites + summary.result_failure_sites <=
          summary.result_like_sites &&
      summary.ns_error_bridge_path_sites <=
          summary.ns_error_out_parameter_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildErrorHandlingErrorSourceClosureReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
