#pragma once

#include <vector>

#include "ast/objc3_ast_declarations.h"
#include "lex/objc3_lexer.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {
namespace detail {

void CollectControlFlowControlFlowSourceClosureStmtSites(
    const Stmt *stmt,
    Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary);

}  // namespace detail

Objc3FrontendControlFlowControlFlowSourceClosureSummary
BuildControlFlowControlFlowSourceClosureSummary(
    const Objc3Program &program,
    const std::vector<Objc3LexToken> &tokens);

}  // namespace objc3c::pipeline::orchestration
