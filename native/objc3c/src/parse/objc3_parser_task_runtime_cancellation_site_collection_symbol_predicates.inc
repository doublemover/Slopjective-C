bool IsTaskRuntimeHookSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("task_runtime") != std::string::npos ||
         lowered.find("task_group") != std::string::npos ||
         lowered.find("group_task") != std::string::npos ||
         lowered.find("task_spawn") != std::string::npos ||
         lowered.find("spawn_task") != std::string::npos ||
         lowered.find("detached_task") != std::string::npos ||
         lowered.find("task_detach") != std::string::npos ||
         lowered.find("runtime_task") != std::string::npos ||
         lowered.find("executor") != std::string::npos;
}

bool IsCancellationCheckSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cancel") != std::string::npos ||
         lowered.find("is_cancelled") != std::string::npos ||
         lowered.find("cancellation") != std::string::npos;
}

bool IsCancellationHandlerSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cancel_handler") != std::string::npos ||
         lowered.find("with_cancellation_handler") != std::string::npos ||
         lowered.find("on_cancel") != std::string::npos;
}

bool IsSuspensionPointSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("await") != std::string::npos ||
         lowered.find("wait_next") != std::string::npos ||
         lowered.find("suspend") != std::string::npos ||
         lowered.find("yield") != std::string::npos;
}
