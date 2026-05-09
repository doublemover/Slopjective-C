#pragma once

#include <cstddef>
#include <string>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "pipeline/frontend_metadata_handoff_ordering.h"
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
