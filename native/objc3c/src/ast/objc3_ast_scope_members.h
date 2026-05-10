#pragma once

#include <string>

#include "ast/objc3_ast_function_decl_nodes.h"
#include "ast/objc3_ast_method_decl_nodes.h"
#include "ast/objc3_ast_property_decl_nodes.h"

std::string BuildObjcMethodScopePathSymbol(const Objc3MethodDecl &method);
std::string BuildObjcPropertyScopePathSymbol(
    const Objc3PropertyDecl &property);
std::string BuildObjcPropertySynthesisSymbol(
    const Objc3PropertyDecl &property);
std::string BuildObjcIvarBindingSymbol(const Objc3PropertyDecl &property);
std::string BuildObjcTypecheckParamFamilySymbol(const FuncParam &param);
std::string BuildObjcTypecheckReturnFamilySymbol(const FunctionDecl &fn);
