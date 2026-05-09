#include "ir/objc3_ir_concurrency_identity.h"

#include <cstdint>

int Objc3IRStablePositiveAsyncTag(const std::string &text) {
  std::uint32_t hash = 2166136261u;
  for (unsigned char ch : text) {
    hash ^= static_cast<std::uint32_t>(ch);
    hash *= 16777619u;
  }
  return static_cast<int>((hash % 2147483646u) + 1u);
}

int Objc3IRExecutorAffinityTag(const FunctionDecl &fn) {
  if (!fn.executor_affinity_declared) {
    return 0;
  }
  if (fn.executor_affinity_kind == "main") {
    return 1;
  }
  if (fn.executor_affinity_named && !fn.executor_affinity_name.empty()) {
    return Objc3IRStablePositiveAsyncTag("executor:" +
                                         fn.executor_affinity_name);
  }
  return Objc3IRStablePositiveAsyncTag("executor-kind:" +
                                       fn.executor_affinity_kind);
}

int Objc3IRExecutorAffinityTag(const Objc3MethodDecl &method) {
  if (!method.executor_affinity_declared) {
    return 0;
  }
  if (method.executor_affinity_kind == "main") {
    return 1;
  }
  if (method.executor_affinity_named &&
      !method.executor_affinity_name.empty()) {
    return Objc3IRStablePositiveAsyncTag("executor:" +
                                         method.executor_affinity_name);
  }
  return Objc3IRStablePositiveAsyncTag("executor-kind:" +
                                       method.executor_affinity_kind);
}

int Objc3IRAsyncResumeEntryTag(const FunctionDecl &fn) {
  return Objc3IRStablePositiveAsyncTag("resume-entry:function:" + fn.name);
}

int Objc3IRAsyncResumeEntryTag(const Objc3IRMethodDefinition &method_def) {
  return Objc3IRStablePositiveAsyncTag("resume-entry:method:" +
                                       method_def.symbol);
}
