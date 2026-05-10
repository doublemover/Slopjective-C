#include "parse/objc3_parser_attributes_local_storage_payloads_resource_state.inc"
#include "parse/objc3_parser_attributes_local_storage_payloads_resource_boundaries.inc"
#include "parse/objc3_parser_attributes_local_storage_payloads_resource_clauses.inc"
#include "parse/objc3_parser_attributes_local_storage_payloads_resource_required.inc"
#include "parse/objc3_parser_attributes_local_storage_payloads_resource_publication.inc"

  bool ParseLocalResourceAttribute(
      LetStmt &stmt,
      const Token &attribute_name) {
    if (!ParseLocalResourceAttributePayloadOpen()) {
      return false;
    }
    LocalResourceAttributeClauseState clauses;
    if (!ParseLocalResourceAttributeClauses(stmt, clauses)) {
      return false;
    }
    if (!ParseLocalResourceAttributePayloadClose()) {
      return false;
    }
    if (!ValidateLocalResourceAttributeRequiredClauses(
            attribute_name, clauses)) {
      return false;
    }
    PublishLocalResourceAttributeProfile(stmt);
    return true;
  }
