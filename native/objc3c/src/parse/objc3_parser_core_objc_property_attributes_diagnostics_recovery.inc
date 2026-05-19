  bool ReportInvalidObjcPropertyAttribute(const Token &token) {
    diagnostics_.push_back(
        MakeDiag(token.line, token.column, "O3P101", "invalid Objective-C @property attribute"));
    return false;
  }

  bool ReportMissingObjcPropertyAttributeValue(const Token &token) {
    diagnostics_.push_back(
        MakeDiag(token.line, token.column, "O3P100", "missing Objective-C @property attribute value"));
    return false;
  }

  bool ReportMissingObjcPropertyAttributeListClose(const Token &token) {
    diagnostics_.push_back(
        MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C @property attribute list"));
    return false;
  }

  bool ValidateObjcPropertyAttributePayload(const Objc3PropertyAttributeDecl &attribute) {
    if (attribute.name == "behavior" && !attribute.has_value) {
      diagnostics_.push_back(MakeDiag(
          attribute.line, attribute.column, "O3P355",
          "Objective-C @property behavior requires '=Name' payload"));
      return false;
    }
    return true;
  }
