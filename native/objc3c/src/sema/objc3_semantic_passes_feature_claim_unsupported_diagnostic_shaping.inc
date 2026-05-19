static void DiagnoseEscapingBlockRuntimeHandleCaptureLegality(
    unsigned line, unsigned column, const SemanticTypeInfo &value,
    std::vector<std::string> &diagnostics) {
  (void)line;
  (void)column;
  (void)diagnostics;
  if (!value.callable_block_runtime_handle_candidate) {
    return;
  }
}
