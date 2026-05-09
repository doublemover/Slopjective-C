#pragma once

#include "ast/objc3_ast_declarations.h"

namespace objc3c::parse {

void FinalizeObjc3ParserCoreInlineAsmIntrinsicGovernanceProfile(
    FunctionDecl &fn);

void FinalizeObjc3ParserCoreInlineAsmIntrinsicGovernanceProfile(
    Objc3MethodDecl &method);

}  // namespace objc3c::parse
