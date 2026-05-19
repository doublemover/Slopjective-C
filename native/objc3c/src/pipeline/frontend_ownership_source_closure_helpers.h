#pragma once

#include <vector>

#include "ast/objc3_ast_declarations.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {
namespace detail {

void CollectOwnershipSystemExtensionStmtSites(
    const Stmt *stmt,
    Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary);

void CollectOwnershipSystemExtensionExprSites(
    const Expr *expr,
    Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary);

}  // namespace detail

Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
BuildOwnershipSystemExtensionSourceClosureSummary(const Objc3Program &program);

}  // namespace objc3c::pipeline::orchestration
