#include "parse/objc3_parser_core_top_level_function_declarations.inc"

  bool AtIdentifierColon() const {
    return IsObjc3IdentifierColonStatementLead(tokens_, index_);
  }

  bool AtIdentifierAssignment() const {
    return IsObjc3IdentifierAssignmentStatementLead(tokens_, index_);
  }

  bool AtIdentifierUpdate() const {
    return IsObjc3IdentifierUpdateStatementLead(tokens_, index_);
  }

  bool AtPrefixUpdate() const {
    return IsObjc3PrefixUpdateStatementLead(tokens_, index_);
  }

  bool MatchAssignmentOperator(std::string &op) {
    if (!objc3c::parse::IsObjc3AssignmentOperatorToken(Peek().kind)) {
      return false;
    }
    op = objc3c::parse::Objc3AssignmentOperatorSpelling(Peek().kind);
    Advance();
    return true;
  }

  bool MatchUpdateOperator(std::string &op) {
    if (!objc3c::parse::IsObjc3UpdateOperatorToken(Peek().kind)) {
      return false;
    }
    op = objc3c::parse::Objc3UpdateOperatorSpelling(Peek().kind);
    Advance();
    return true;
  }
