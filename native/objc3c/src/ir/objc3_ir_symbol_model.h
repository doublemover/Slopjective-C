#pragma once

#include <string>

std::string BuildSynthesizedInstanceMethodOwnerIdentity(
    const std::string &declaration_owner_identity,
    const std::string &selector);
std::string BuildSynthesizedPropertyStorageSymbol(
    const std::string &binding_symbol);
std::string BuildDirectDispatchMethodKey(
    const std::string &implementation_name, const std::string &selector,
    bool is_class_method);
std::string BuildImplementationMethodFunctionSymbol(
    const std::string &implementation_name, const std::string &selector,
    bool is_class_method);
