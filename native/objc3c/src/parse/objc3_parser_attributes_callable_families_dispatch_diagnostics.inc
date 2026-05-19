  bool ReportDuplicateDispatchIntentCallableAttribute(
      const Token &attribute_name,
      const char *code,
      const std::string &message) {
    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, code, message));
    return false;
  }
