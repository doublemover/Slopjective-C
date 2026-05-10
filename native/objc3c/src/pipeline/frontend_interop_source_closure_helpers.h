#pragma once

#include "ast/objc3_ast_declarations.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendInteropForeignImportSourceClosureSummary
BuildInteropForeignImportSourceClosureSummary(const Objc3Program &program);

}  // namespace objc3c::pipeline::orchestration
