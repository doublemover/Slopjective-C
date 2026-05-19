#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast_property_decl_nodes.h"

std::string BuildObjcMethodLookupSymbol(const Objc3MethodDecl &method);
std::string BuildObjcMethodOverrideLookupSymbol(const Objc3MethodDecl &method);
std::string BuildObjcMethodConflictLookupSymbol(const Objc3MethodDecl &method);
std::vector<std::string> BuildObjcMethodLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods);
std::vector<std::string> BuildObjcMethodOverrideLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods);
std::vector<std::string> BuildObjcMethodConflictLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods);
std::vector<std::string> BuildObjcPropertySynthesisSymbolsLexicographic(
    const std::vector<Objc3PropertyDecl> &properties);
std::vector<std::string> BuildObjcIvarBindingSymbolsLexicographic(
    const std::vector<Objc3PropertyDecl> &properties);
