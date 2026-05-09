#pragma once

#include "ast/objc3_ast.h"

#include <string>

bool ParseOwnershipResourceInvalidLiteral(const std::string &text, int &value);
std::string BuildSynthesizedInstanceMethodOwnerIdentity(
    const std::string &declaration_owner_identity,
    const std::string &selector);
std::string BuildSynthesizedPropertyStorageSymbol(
    const std::string &binding_symbol);
std::string BuildDirectDispatchMethodKey(
    const std::string &implementation_name, const std::string &selector,
    bool is_class_method);
ValueType RuntimeMetadataValueType(const std::string &type_name);
