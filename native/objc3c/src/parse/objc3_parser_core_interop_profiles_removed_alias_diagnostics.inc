  bool AtRemovedOptionalTemplateAlias() const {
    return IsObjc3RemovedOptionalTemplateAliasLead(tokens_, index_);
  }

  bool AtReservedValueOptionalType() const {
    return IsObjc3ReservedValueOptionalTypeLead(tokens_, index_);
  }

  bool RejectRemovedOptionalTemplateAlias(const Token &token) {
    diagnostics_.push_back(
        BuildObjc3RemovedOptionalTemplateAliasDiagnostic(token));
    return false;
  }

  bool RejectReservedValueOptionalType(const Token &token) {
    diagnostics_.push_back(
        BuildObjc3ReservedValueOptionalTypeDiagnostic(token));
    return false;
  }
