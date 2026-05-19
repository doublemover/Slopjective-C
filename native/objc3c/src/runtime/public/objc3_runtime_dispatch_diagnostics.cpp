#include "runtime/public/objc3_runtime_dispatch_diagnostics.h"

namespace objc3c::runtime {

namespace {

constexpr RuntimeDispatchDiagnosticRecord kRuntimeDispatchDiagnostics[] = {
    {OBJC3_RUNTIME_DISPATCH_STATUS_OK, "", ""},
    {OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER, "O3RT008",
     "runtime dispatch failed: nil receiver has no value dispatch result"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR, "O3RT001",
     "runtime dispatch failed: unknown selector"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS, "O3RT002",
     "runtime dispatch failed: unknown receiver class"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH, "O3RT003",
     "runtime dispatch failed: missing class graph"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, "O3RT004",
     "runtime dispatch failed: malformed metadata"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, "O3RT005",
     "runtime dispatch failed: rejected return shape"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT, "O3RT006",
     "runtime dispatch failed: rejected argument layout"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT, "O3RT007",
     "runtime dispatch failed: category conflict"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_STALE_METHOD_CACHE, "O3RT009",
     "runtime dispatch failed: stale method cache entry"}};

constexpr RuntimeDispatchDiagnosticRecord kInternalDispatchDiagnostic = {
    OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, "O3RT000",
    "runtime dispatch failed: internal dispatch status"};

}  // namespace

const RuntimeDispatchDiagnosticRecord &RuntimeDispatchDiagnosticForStatus(
    objc3_runtime_dispatch_status_code status_code) {
  for (const RuntimeDispatchDiagnosticRecord &diagnostic :
       kRuntimeDispatchDiagnostics) {
    if (diagnostic.status_code == status_code) {
      return diagnostic;
    }
  }
  return kInternalDispatchDiagnostic;
}

const char *RuntimeDispatchReturnKindName(
    objc3_runtime_dispatch_return_kind_code return_kind) {
  switch (return_kind) {
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32:
      return "i32";
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_BOOL:
      return "bool";
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_VOID:
      return "void";
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_OBJECT_REFERENCE:
      return "object-reference";
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_CLASS_REFERENCE:
      return "class-reference";
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_SELECTOR_REFERENCE:
      return "selector-reference";
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_PROTOCOL_REFERENCE:
      return "protocol-reference";
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED:
      return "unsupported";
  }
  return "unsupported";
}

}  // namespace objc3c::runtime
