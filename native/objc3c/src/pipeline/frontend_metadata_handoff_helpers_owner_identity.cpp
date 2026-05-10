#include "pipeline/frontend_metadata_handoff_helpers.h"

#include <algorithm>

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
      std::count_if(methods.begin(), methods.end(),
                    [](const Objc3MethodDecl &method) {
                      return method.is_class_method;
                    }));
}
