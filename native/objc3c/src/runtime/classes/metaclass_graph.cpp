#include "runtime/classes/metaclass_graph.h"

namespace objc3c::runtime {

namespace {

bool RuntimeGraphTextPresent(const char *value) {
  return value != nullptr && value[0] != '\0';
}

bool RuntimeGraphTextEquals(const char *left, const char *right) {
  if (!RuntimeGraphTextPresent(left) || !RuntimeGraphTextPresent(right)) {
    return false;
  }
  while (*left != '\0' && *right != '\0') {
    if (*left != *right) {
      return false;
    }
    ++left;
    ++right;
  }
  return *left == '\0' && *right == '\0';
}

std::string RuntimeGraphClassNameForDiagnostic(const char *class_name) {
  return RuntimeGraphTextPresent(class_name) ? std::string(class_name)
                                             : std::string("<unknown>");
}

void StoreMetaclassFailure(std::string *failure_reason,
                           const std::string &reason) {
  if (failure_reason != nullptr) {
    *failure_reason = reason;
  }
}

void ClearMetaclassFailure(std::string *failure_reason) {
  if (failure_reason != nullptr) {
    failure_reason->clear();
  }
}

}  // namespace

bool RuntimeMetaclassEdgeIsMaterializable(const char *class_owner_identity,
                                          const char *metaclass_owner_identity) {
  return RuntimeGraphTextPresent(class_owner_identity) &&
         RuntimeGraphTextPresent(metaclass_owner_identity);
}

bool RuntimeMetaclassMetadataIsConsistent(
    const char *class_name, const char *class_owner_identity,
    const char *metaclass_owner_identity,
    const char *super_class_owner_identity,
    const char *super_metaclass_owner_identity, bool is_root_class,
    std::string *failure_reason) {
  ClearMetaclassFailure(failure_reason);
  const std::string diagnostic_class_name =
      RuntimeGraphClassNameForDiagnostic(class_name);
  if (!RuntimeMetaclassEdgeIsMaterializable(class_owner_identity,
                                           metaclass_owner_identity)) {
    StoreMetaclassFailure(
        failure_reason,
        "class/metaclass owner edge is incomplete for " +
            diagnostic_class_name);
    return false;
  }

  const bool has_super_class_owner =
      RuntimeGraphTextPresent(super_class_owner_identity);
  const bool has_super_metaclass_owner =
      RuntimeGraphTextPresent(super_metaclass_owner_identity);
  if (is_root_class) {
    if (has_super_class_owner || has_super_metaclass_owner) {
      StoreMetaclassFailure(
          failure_reason,
          "root class publishes superclass metaclass metadata for " +
              diagnostic_class_name);
      return false;
    }
    return true;
  }

  if (!has_super_class_owner || !has_super_metaclass_owner) {
    StoreMetaclassFailure(
        failure_reason,
        "subclass class/metaclass superclass edge is incomplete for " +
            diagnostic_class_name);
    return false;
  }
  return true;
}

bool RuntimeMetaclassSuperclassLinkIsConsistent(
    const char *class_name, const char *super_class_owner_identity,
    const char *super_metaclass_owner_identity,
    const char *resolved_super_class_owner_identity,
    const char *resolved_super_metaclass_owner_identity,
    std::string *failure_reason) {
  ClearMetaclassFailure(failure_reason);
  const std::string diagnostic_class_name =
      RuntimeGraphClassNameForDiagnostic(class_name);
  if (!RuntimeGraphTextEquals(super_class_owner_identity,
                              resolved_super_class_owner_identity)) {
    StoreMetaclassFailure(
        failure_reason,
        "subclass superclass owner does not match realized superclass for " +
            diagnostic_class_name);
    return false;
  }
  if (!RuntimeGraphTextEquals(super_metaclass_owner_identity,
                              resolved_super_metaclass_owner_identity)) {
    StoreMetaclassFailure(
        failure_reason,
        "subclass metaclass superclass owner does not match realized "
        "superclass metaclass for " +
            diagnostic_class_name);
    return false;
  }
  return true;
}

}  // namespace objc3c::runtime
