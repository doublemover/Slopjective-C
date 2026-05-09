#include "pipeline/frontend_metadata_handoff_helpers.h"

#include <algorithm>
#include <tuple>

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

bool IsClassSourceRecordLess(const Objc3RuntimeMetadataClassSourceRecord &lhs,
                             const Objc3RuntimeMetadataClassSourceRecord &rhs) {
  return std::tie(lhs.name, lhs.record_kind, lhs.objc_final_declared,
                  lhs.objc_sealed_declared, lhs.line, lhs.column) <
         std::tie(rhs.name, rhs.record_kind, rhs.objc_final_declared,
                  rhs.objc_sealed_declared, rhs.line, rhs.column);
}

bool IsProtocolSourceRecordLess(
    const Objc3RuntimeMetadataProtocolSourceRecord &lhs,
    const Objc3RuntimeMetadataProtocolSourceRecord &rhs) {
  return std::tie(lhs.name, lhs.line, lhs.column) <
         std::tie(rhs.name, rhs.line, rhs.column);
}

bool IsCategorySourceRecordLess(
    const Objc3RuntimeMetadataCategorySourceRecord &lhs,
    const Objc3RuntimeMetadataCategorySourceRecord &rhs) {
  return std::tie(lhs.class_name, lhs.category_name, lhs.record_kind, lhs.line,
                  lhs.column) <
         std::tie(rhs.class_name, rhs.category_name, rhs.record_kind, rhs.line,
                  rhs.column);
}

bool IsPropertySourceRecordLess(
    const Objc3RuntimeMetadataPropertySourceRecord &lhs,
    const Objc3RuntimeMetadataPropertySourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name, lhs.line,
                  lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name, rhs.line,
                  rhs.column);
}

bool IsMethodSourceRecordLess(const Objc3RuntimeMetadataMethodSourceRecord &lhs,
                              const Objc3RuntimeMetadataMethodSourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.selector,
                  lhs.effective_direct_dispatch, lhs.objc_final_declared,
                  lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.selector,
                  rhs.effective_direct_dispatch, rhs.objc_final_declared,
                  rhs.line, rhs.column);
}

bool IsIvarSourceRecordLess(const Objc3RuntimeMetadataIvarSourceRecord &lhs,
                            const Objc3RuntimeMetadataIvarSourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name,
                  lhs.ivar_binding_symbol, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name,
                  rhs.ivar_binding_symbol, rhs.line, rhs.column);
}

bool IsExecutableMetadataInterfaceNodeLess(
    const Objc3ExecutableMetadataInterfaceGraphNode &lhs,
    const Objc3ExecutableMetadataInterfaceGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataImplementationNodeLess(
    const Objc3ExecutableMetadataImplementationGraphNode &lhs,
    const Objc3ExecutableMetadataImplementationGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataClassNodeLess(
    const Objc3ExecutableMetadataClassGraphNode &lhs,
    const Objc3ExecutableMetadataClassGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataMetaclassNodeLess(
    const Objc3ExecutableMetadataMetaclassGraphNode &lhs,
    const Objc3ExecutableMetadataMetaclassGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataProtocolNodeLess(
    const Objc3ExecutableMetadataProtocolGraphNode &lhs,
    const Objc3ExecutableMetadataProtocolGraphNode &rhs) {
  return std::tie(lhs.protocol_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.protocol_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataCategoryNodeLess(
    const Objc3ExecutableMetadataCategoryGraphNode &lhs,
    const Objc3ExecutableMetadataCategoryGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.category_name, lhs.owner_identity, lhs.line,
                  lhs.column) <
         std::tie(rhs.class_name, rhs.category_name, rhs.owner_identity, rhs.line,
                  rhs.column);
}

bool IsExecutableMetadataPropertyNodeLess(
    const Objc3ExecutableMetadataPropertyGraphNode &lhs,
    const Objc3ExecutableMetadataPropertyGraphNode &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name,
                  lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name,
                  rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataMethodNodeLess(
    const Objc3ExecutableMetadataMethodGraphNode &lhs,
    const Objc3ExecutableMetadataMethodGraphNode &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.selector,
                  lhs.is_class_method, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.selector,
                  rhs.is_class_method, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataIvarNodeLess(
    const Objc3ExecutableMetadataIvarGraphNode &lhs,
    const Objc3ExecutableMetadataIvarGraphNode &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name,
                  lhs.ivar_binding_symbol, lhs.owner_identity, lhs.line,
                  lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name,
                  rhs.ivar_binding_symbol, rhs.owner_identity, rhs.line,
                  rhs.column);
}

bool IsExecutableMetadataGraphEdgeLess(
    const Objc3ExecutableMetadataGraphEdge &lhs,
    const Objc3ExecutableMetadataGraphEdge &rhs) {
  return std::tie(lhs.edge_kind, lhs.source_owner_identity, lhs.target_owner_identity,
                  lhs.line, lhs.column) <
         std::tie(rhs.edge_kind, rhs.source_owner_identity, rhs.target_owner_identity,
                  rhs.line, rhs.column);
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
