#pragma once

namespace objc3c::runtime {

enum class RuntimeMethodReturnKind {
  Unsupported = 0,
  Int32 = 1,
  Bool = 2,
  Void = 3,
  ObjectReference = 4,
  ClassReference = 5,
  SelectorReference = 6,
  ProtocolReference = 7,
};

RuntimeMethodReturnKind ClassifyRuntimeReturnType(
    const char *return_type_name);
const char *RuntimeMethodReturnKindName(RuntimeMethodReturnKind return_kind);
bool RuntimeMethodReturnKindIsDispatchResultSupported(
    RuntimeMethodReturnKind return_kind);

}  // namespace objc3c::runtime
