#include "pipeline/frontend_metadata_handoff_helpers.h"

#include "lower/objc3_lowering_contract.h"
#include "support/objc3_property_storage_profile_helpers.h"

std::string BuildExecutablePropertyOwnerKey(const std::string &owner_name,
                                            const std::string &property_name) {
  return owner_name + "|" + property_name;
}

bool ShouldSynthesizeExecutablePropertyAccessors(
    const std::string &owner_kind, const std::string &owner_name,
    const std::string &property_name,
    const std::unordered_set<std::string> &class_implementation_names,
    const std::unordered_set<std::string> &implementation_property_keys) {
  if (owner_kind == "class-implementation" ||
      owner_kind == "category-implementation") {
    return true;
  }
  if (owner_kind != "class-interface") {
    return false;
  }
  return class_implementation_names.find(owner_name) !=
             class_implementation_names.end() &&
         implementation_property_keys.find(
             BuildExecutablePropertyOwnerKey(owner_name, property_name)) ==
             implementation_property_keys.end();
}

std::string BuildGetterStorageRuntimeHelperSymbol(
    bool synthesizes_executable_accessors,
    const std::string &ownership_runtime_hook_profile) {
  if (!synthesizes_executable_accessors) {
    return {};
  }
  return objc3c::support::UsesWeakCurrentPropertyRuntimeHelper(
             ownership_runtime_hook_profile)
             ? kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
             : kObjc3RuntimeReadCurrentPropertyI32Symbol;
}

std::string BuildSetterStorageRuntimeHelperSymbol(
    bool synthesizes_executable_accessors, bool effective_setter_available,
    const std::string &ownership_lifetime_profile,
    const std::string &ownership_runtime_hook_profile,
    const std::string &accessor_ownership_profile) {
  if (!synthesizes_executable_accessors || !effective_setter_available) {
    return {};
  }
  if (objc3c::support::UsesWeakCurrentPropertyRuntimeHelper(
          ownership_runtime_hook_profile)) {
    return kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
  }
  return objc3c::support::UsesStrongOwnedCurrentPropertyExchange(
             ownership_lifetime_profile, accessor_ownership_profile)
             ? kObjc3RuntimeExchangeCurrentPropertyI32Symbol
             : kObjc3RuntimeWriteCurrentPropertyI32Symbol;
}
