#pragma once

#include <vector>

#include "ast/objc3_ast_declarations.h"
#include "lex/objc3_lexer.h"
#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"
#include "sema/model/semantic_symbol_core_source_closures.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendConcurrencyAsyncSourceClosureSummary
BuildConcurrencyAsyncSourceClosureSummary(
    const Objc3Program &program,
    const std::vector<Objc3LexToken> &tokens);

Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
BuildConcurrencyActorMemberIsolationSourceClosureSummary(
    const Objc3Program &program);

Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
BuildConcurrencyTaskGroupCancellationSourceClosureSummary(
    const Objc3Program &program);

}  // namespace objc3c::pipeline::orchestration
