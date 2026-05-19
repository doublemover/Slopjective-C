#include "runtime/dispatch/runtime_method_return.h"

#include <string>

namespace objc3c::runtime {

RuntimeMethodReturnKind ClassifyRuntimeReturnType(
    const char *return_type_name) {
  if (return_type_name == nullptr) {
    return RuntimeMethodReturnKind::Unsupported;
  }
  const std::string type_name = return_type_name;
  if (type_name == "void") {
    return RuntimeMethodReturnKind::Void;
  }
  if (type_name == "bool" || type_name == "BOOL") {
    return RuntimeMethodReturnKind::Bool;
  }
  if (type_name == "i32" || type_name == "int") {
    return RuntimeMethodReturnKind::Int32;
  }
  if (type_name == "id" || type_name == "instancetype" ||
      type_name == "object-pointer") {
    return RuntimeMethodReturnKind::ObjectReference;
  }
  if (type_name == "Class") {
    return RuntimeMethodReturnKind::ClassReference;
  }
  if (type_name == "SEL") {
    return RuntimeMethodReturnKind::SelectorReference;
  }
  if (type_name == "Protocol") {
    return RuntimeMethodReturnKind::ProtocolReference;
  }
  return RuntimeMethodReturnKind::Unsupported;
}

const char *RuntimeMethodReturnKindName(RuntimeMethodReturnKind return_kind) {
  switch (return_kind) {
    case RuntimeMethodReturnKind::Int32:
      return "i32";
    case RuntimeMethodReturnKind::Bool:
      return "bool";
    case RuntimeMethodReturnKind::Void:
      return "void";
    case RuntimeMethodReturnKind::ObjectReference:
      return "object-reference";
    case RuntimeMethodReturnKind::ClassReference:
      return "class-reference";
    case RuntimeMethodReturnKind::SelectorReference:
      return "selector-reference";
    case RuntimeMethodReturnKind::ProtocolReference:
      return "protocol-reference";
    case RuntimeMethodReturnKind::Unsupported:
      return "unsupported";
  }
  return "unsupported";
}

bool RuntimeMethodReturnKindIsDispatchResultSupported(
    RuntimeMethodReturnKind return_kind) {
  switch (return_kind) {
    case RuntimeMethodReturnKind::Int32:
    case RuntimeMethodReturnKind::Bool:
    case RuntimeMethodReturnKind::Void:
    case RuntimeMethodReturnKind::ObjectReference:
    case RuntimeMethodReturnKind::ClassReference:
    case RuntimeMethodReturnKind::SelectorReference:
    case RuntimeMethodReturnKind::ProtocolReference:
      return true;
    case RuntimeMethodReturnKind::Unsupported:
      return false;
  }
  return false;
}

}  // namespace objc3c::runtime
