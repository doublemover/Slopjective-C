#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast_container_decl_nodes.h"
#include "ast/objc3_ast_core.h"
#include "ast/objc3_ast_function_decl_nodes.h"
#include "ast/objc3_ast_method_decl_nodes.h"
#include "token/objc3_token_contract.h"

namespace objc3c::parse {

bool IsObjc3BorrowedQualifierSpelling(const std::string &text);

std::string BuildObjc3ReturnsBorrowedProfile(
    bool declared,
    std::size_t owner_index,
    bool return_borrowed_pointer_qualified);

bool ParseObjc3OptionalContainerDispatchAttributes(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    Objc3InterfaceDecl &decl);

bool ParseObjc3OptionalCallableBridgeAttributes(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    FunctionDecl &decl);

bool ParseObjc3OptionalCallableBridgeAttributes(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    Objc3MethodDecl &decl);

bool ParseObjc3LocalStorageAttribute(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    LetStmt &stmt);

bool ParseObjc3LocalStorageSugar(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    LetStmt &stmt);

void FinalizeObjc3ErrorBridgeMarkerProfile(FunctionDecl &decl);

void FinalizeObjc3ErrorBridgeMarkerProfile(Objc3MethodDecl &decl);

}  // namespace objc3c::parse
