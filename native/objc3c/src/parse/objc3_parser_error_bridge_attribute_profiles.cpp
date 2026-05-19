#include "parse/objc3_parser_attributes.h"

namespace objc3c::parse {

namespace {

template <typename TCallableDecl>
void FinalizeObjc3ErrorBridgeMarkerProfileImpl(TCallableDecl &decl) {
  decl.objc_nserror_attribute_sites = decl.objc_nserror_declared ? 1u : 0u;
  decl.objc_status_code_attribute_sites =
      decl.objc_status_code_declared ? 1u : 0u;
  decl.status_code_success_clause_sites =
      decl.objc_status_code_success_literal.empty() ? 0u : 1u;
  decl.status_code_error_type_clause_sites =
      decl.objc_status_code_error_type_spelling.empty() ? 0u : 1u;
  decl.status_code_mapping_clause_sites =
      decl.objc_status_code_mapping_symbol.empty() ? 0u : 1u;
  decl.error_bridge_marker_contract_violation_sites = 0u;
  if (decl.objc_nserror_declared && decl.objc_status_code_declared) {
    decl.error_bridge_marker_contract_violation_sites += 1u;
  }
  if (decl.objc_status_code_declared &&
      (decl.status_code_success_clause_sites != 1u ||
       decl.status_code_error_type_clause_sites != 1u ||
       decl.status_code_mapping_clause_sites != 1u)) {
    decl.error_bridge_marker_contract_violation_sites += 1u;
  }
  decl.error_bridge_marker_profile =
      std::string("error-bridge-markers:objc_nserror=") +
      (decl.objc_nserror_declared ? "true" : "false") +
      ";objc_status_code=" +
      (decl.objc_status_code_declared ? "true" : "false") +
      ";success=" + decl.objc_status_code_success_literal +
      ";error_type=" + decl.objc_status_code_error_type_spelling +
      ";mapping=" + decl.objc_status_code_mapping_symbol +
      ";contract_violation_sites=" +
      std::to_string(decl.error_bridge_marker_contract_violation_sites);
  decl.error_bridge_marker_profile_is_normalized =
      decl.error_bridge_marker_contract_violation_sites == 0u &&
      (!decl.objc_status_code_declared ||
       (decl.status_code_success_clause_sites == 1u &&
        decl.status_code_error_type_clause_sites == 1u &&
        decl.status_code_mapping_clause_sites == 1u));
}

}  // namespace

void FinalizeObjc3ErrorBridgeMarkerProfile(FunctionDecl &decl) {
  FinalizeObjc3ErrorBridgeMarkerProfileImpl(decl);
}

void FinalizeObjc3ErrorBridgeMarkerProfile(Objc3MethodDecl &decl) {
  FinalizeObjc3ErrorBridgeMarkerProfileImpl(decl);
}

}  // namespace objc3c::parse
