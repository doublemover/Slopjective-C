#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast_declarations.h"

std::vector<std::string> BuildScopePathLexicographic(std::string owner_symbol,
                                                     std::string entry_symbol);
std::string BuildObjcContainerScopeOwner(const std::string &container_kind,
                                         const std::string &name,
                                         bool has_category,
                                         const std::string &category_name);
std::string BuildObjcMethodScopePathSymbol(const Objc3MethodDecl &method);
std::string BuildObjcPropertyScopePathSymbol(
    const Objc3PropertyDecl &property);
std::string BuildObjcPropertySynthesisSymbol(
    const Objc3PropertyDecl &property);
std::string BuildObjcIvarBindingSymbol(const Objc3PropertyDecl &property);
std::string BuildObjcTypecheckParamFamilySymbol(const FuncParam &param);
std::string BuildObjcTypecheckReturnFamilySymbol(const FunctionDecl &fn);
std::vector<std::string> BuildProtocolSemanticLinkTargetsLexicographic(
    const std::vector<std::string> &protocol_names);
std::string BuildObjcCategorySemanticLinkSymbol(
    const std::string &owner_name,
    const std::string &category_name);
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
