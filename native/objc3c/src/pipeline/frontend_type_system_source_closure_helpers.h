#pragma once

#include "ast/objc3_ast_declarations.h"
#include "sema/model/frontend_linkage_summaries.h"
#include "sema/model/frontend_type_source_closure.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendTypeSystemTypeSourceClosureSummary
BuildTypeSystemTypeSourceClosureSummary(
    const Objc3Program &program,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_summary);

}  // namespace objc3c::pipeline::orchestration
