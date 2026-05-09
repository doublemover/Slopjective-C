#pragma once

#include <string>

#include "ast/objc3_ast_declarations.h"
#include "pipeline/frontend_source_closure_replay_keys.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
BuildMetaprogrammingMetaprogrammingSourceClosureSummary(
    const Objc3Program &program);

Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummary(
    const Objc3Program &program);

Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
BuildMetaprogrammingPropertyBehaviorSourceCompletionSummary(
    const Objc3Program &program);

}  // namespace objc3c::pipeline::orchestration
