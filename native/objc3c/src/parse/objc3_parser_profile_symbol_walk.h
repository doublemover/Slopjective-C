#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "ast/objc3_ast_method_decl_nodes.h"

namespace objc3c::parse {

using Objc3ProfileSymbolVisitCallback =
    void (*)(const std::string &symbol, void *context);

struct Objc3ProfileSymbolWalker {
  Objc3ProfileSymbolVisitCallback visit = nullptr;
  void *context = nullptr;

  void Visit(const std::string &symbol) const;
};

void WalkObjc3ProfileSymbolsInExpr(
    const Expr *expr,
    const Objc3ProfileSymbolWalker &walker);

void WalkObjc3ProfileSymbolsInStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker);

void WalkObjc3ProfileSymbolsInBody(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const Objc3ProfileSymbolWalker &walker);

void WalkObjc3ProfileSymbolsInOpaqueMethodBody(
    const Objc3MethodDecl &method,
    const Objc3ProfileSymbolWalker &walker);

}  // namespace objc3c::parse
