#pragma once

#include <cstddef>
#include <string>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "pipeline/objc3_frontend_types.h"

const char *RuntimeMetadataTypeName(ValueType type);

std::string BuildCategoryOwnerName(const std::string &class_name,
                                   const std::string &category_name);
std::string BuildRuntimeClassOwnerIdentity(const std::string &class_name);
std::string BuildRuntimeMetaclassOwnerIdentity(const std::string &class_name);
std::string BuildRuntimeCategoryOwnerIdentity(const std::string &class_name,
                                              const std::string &category_name);

std::string BuildPropertyNodeOwnerIdentity(
    const std::string &declaration_owner_identity,
    const Objc3PropertyDecl &property);
std::string BuildMethodNodeOwnerIdentity(
    const std::string &declaration_owner_identity,
    const Objc3MethodDecl &method);
std::string BuildIvarNodeOwnerIdentity(
    const std::string &declaration_owner_identity,
    const Objc3PropertyDecl &property);

std::size_t CountClassMethods(const std::vector<Objc3MethodDecl> &methods);

bool IsClassSourceRecordLess(const Objc3RuntimeMetadataClassSourceRecord &lhs,
                             const Objc3RuntimeMetadataClassSourceRecord &rhs);
bool IsProtocolSourceRecordLess(
    const Objc3RuntimeMetadataProtocolSourceRecord &lhs,
    const Objc3RuntimeMetadataProtocolSourceRecord &rhs);
bool IsCategorySourceRecordLess(
    const Objc3RuntimeMetadataCategorySourceRecord &lhs,
    const Objc3RuntimeMetadataCategorySourceRecord &rhs);
bool IsPropertySourceRecordLess(
    const Objc3RuntimeMetadataPropertySourceRecord &lhs,
    const Objc3RuntimeMetadataPropertySourceRecord &rhs);
bool IsMethodSourceRecordLess(const Objc3RuntimeMetadataMethodSourceRecord &lhs,
                              const Objc3RuntimeMetadataMethodSourceRecord &rhs);
bool IsIvarSourceRecordLess(const Objc3RuntimeMetadataIvarSourceRecord &lhs,
                            const Objc3RuntimeMetadataIvarSourceRecord &rhs);

bool IsExecutableMetadataInterfaceNodeLess(
    const Objc3ExecutableMetadataInterfaceGraphNode &lhs,
    const Objc3ExecutableMetadataInterfaceGraphNode &rhs);
bool IsExecutableMetadataImplementationNodeLess(
    const Objc3ExecutableMetadataImplementationGraphNode &lhs,
    const Objc3ExecutableMetadataImplementationGraphNode &rhs);
bool IsExecutableMetadataClassNodeLess(
    const Objc3ExecutableMetadataClassGraphNode &lhs,
    const Objc3ExecutableMetadataClassGraphNode &rhs);
bool IsExecutableMetadataMetaclassNodeLess(
    const Objc3ExecutableMetadataMetaclassGraphNode &lhs,
    const Objc3ExecutableMetadataMetaclassGraphNode &rhs);
bool IsExecutableMetadataProtocolNodeLess(
    const Objc3ExecutableMetadataProtocolGraphNode &lhs,
    const Objc3ExecutableMetadataProtocolGraphNode &rhs);
bool IsExecutableMetadataCategoryNodeLess(
    const Objc3ExecutableMetadataCategoryGraphNode &lhs,
    const Objc3ExecutableMetadataCategoryGraphNode &rhs);
bool IsExecutableMetadataPropertyNodeLess(
    const Objc3ExecutableMetadataPropertyGraphNode &lhs,
    const Objc3ExecutableMetadataPropertyGraphNode &rhs);
bool IsExecutableMetadataMethodNodeLess(
    const Objc3ExecutableMetadataMethodGraphNode &lhs,
    const Objc3ExecutableMetadataMethodGraphNode &rhs);
bool IsExecutableMetadataIvarNodeLess(
    const Objc3ExecutableMetadataIvarGraphNode &lhs,
    const Objc3ExecutableMetadataIvarGraphNode &rhs);
bool IsExecutableMetadataGraphEdgeLess(
    const Objc3ExecutableMetadataGraphEdge &lhs,
    const Objc3ExecutableMetadataGraphEdge &rhs);

std::string BuildExecutablePropertyOwnerKey(const std::string &owner_name,
                                            const std::string &property_name);
bool ShouldSynthesizeExecutablePropertyAccessors(
    const std::string &owner_kind, const std::string &owner_name,
    const std::string &property_name,
    const std::unordered_set<std::string> &class_implementation_names,
    const std::unordered_set<std::string> &implementation_property_keys);

std::string BuildGetterStorageRuntimeHelperSymbol(
    bool synthesizes_executable_accessors,
    const std::string &ownership_runtime_hook_profile);
std::string BuildSetterStorageRuntimeHelperSymbol(
    bool synthesizes_executable_accessors, bool effective_setter_available,
    const std::string &ownership_lifetime_profile,
    const std::string &ownership_runtime_hook_profile,
    const std::string &accessor_ownership_profile);
