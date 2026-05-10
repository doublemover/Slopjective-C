#pragma once

#include <cstddef>
#include <string>

inline constexpr const char
    *kObjc3ErrorHandlingErrorSemanticModelFrontendDependencyContractId =
        "objc3c.error_handling.error.source.closure.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelContractId =
    "objc3c.error_handling.error.semantic.model.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_error_semantic_model";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelRule =
    "throws-declaration-semantics-plus-deterministic-result-and-nserror-profile-carriage-are-live-while-try-throw-do-catch-propagation-and-native-error-runtime-behavior-remain-deferred";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelDeferredRule =
    "try-throw-do-catch-postfix-propagation-status-to-error-execution-bridge-temporaries-and-native-thrown-error-abi-remain-fail-closed-or-later-lane-work";

struct Objc3ErrorHandlingErrorSemanticModelSummary {
  std::string contract_id = kObjc3ErrorHandlingErrorSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ErrorHandlingErrorSemanticModelFrontendDependencyContractId;
  std::string surface_path = kObjc3ErrorHandlingErrorSemanticModelSurfacePath;
  std::string semantic_model = kObjc3ErrorHandlingErrorSemanticModelRule;
  std::string deferred_model =
      kObjc3ErrorHandlingErrorSemanticModelDeferredRule;
  std::size_t throws_declaration_sites = 0;
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
  std::size_t placeholder_throws_propagation_sites = 0;
  std::size_t placeholder_unwind_cleanup_sites = 0;
  bool source_dependency_required = false;
  bool throws_declaration_semantics_landed = false;
  bool result_carrier_profile_semantics_landed = false;
  bool ns_error_bridging_profile_semantics_landed = false;
  bool bridge_marker_semantics_landed = false;
  bool parser_fail_closed_boundary_required = false;
  bool parser_fail_closed_boundary_preserved = false;
  bool propagation_runtime_deferred = false;
  bool status_to_error_runtime_deferred = false;
  bool native_error_abi_deferred = false;
  bool placeholder_throws_summary_carried = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};
