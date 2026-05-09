#include "pipeline/frontend_metadata_handoff_helpers.h"

#include <algorithm>

#include "lower/objc3_lowering_contract.h"
#include "support/objc3_property_storage_profile_helpers.h"

const char *RuntimeMetadataTypeName(ValueType type) {
  switch (type) {
    case ValueType::I32:
      return "i32";
    case ValueType::Bool:
      return "bool";
    case ValueType::Void:
      return "void";
    case ValueType::ObjCId:
      return "id";
    case ValueType::ObjCClass:
      return "Class";
    case ValueType::ObjCSel:
      return "SEL";
    case ValueType::ObjCProtocol:
      return "Protocol";
    case ValueType::ObjCInstancetype:
      return "instancetype";
    case ValueType::ObjCObjectPtr:
      return "object-pointer";
    default:
      return "unknown";
  }
}

std::string BuildCategoryOwnerName(const std::string &class_name,
                                   const std::string &category_name) {
  return class_name + "(" + category_name + ")";
}

std::string BuildRuntimeClassOwnerIdentity(const std::string &class_name) {
  return "class:" + class_name;
}

std::string BuildRuntimeMetaclassOwnerIdentity(const std::string &class_name) {
  return "metaclass:" + class_name;
}

std::string BuildRuntimeCategoryOwnerIdentity(const std::string &class_name,
                                              const std::string &category_name) {
  return "category:" + class_name + "(" + category_name + ")";
}

std::string BuildPropertyNodeOwnerIdentity(
    const std::string &declaration_owner_identity,
    const Objc3PropertyDecl &property) {
  return property.scope_path_symbol.empty()
             ? declaration_owner_identity + "::property:" + property.name
             : property.scope_path_symbol;
}

std::string BuildMethodNodeOwnerIdentity(
    const std::string &declaration_owner_identity,
    const Objc3MethodDecl &method) {
  return method.scope_path_symbol.empty()
             ? declaration_owner_identity + "::" +
                   (method.is_class_method ? "class_method:" : "instance_method:") +
                   method.selector
             : method.scope_path_symbol;
}

std::string BuildIvarNodeOwnerIdentity(
    const std::string &declaration_owner_identity,
    const Objc3PropertyDecl &property) {
  if (!property.ivar_binding_symbol.empty()) {
    if (property.ivar_binding_symbol.find("::") != std::string::npos) {
      return property.ivar_binding_symbol;
    }
    return declaration_owner_identity + "::" + property.ivar_binding_symbol;
  }
  return declaration_owner_identity + "::ivar:" + property.name;
}

std::size_t CountClassMethods(const std::vector<Objc3MethodDecl> &methods) {
  return static_cast<std::size_t>(
      std::count_if(methods.begin(), methods.end(), [](const Objc3MethodDecl &method) {
        return method.is_class_method;
      }));
}

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
