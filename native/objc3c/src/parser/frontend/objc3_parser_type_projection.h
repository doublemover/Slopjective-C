#pragma once

#include "ast/objc3_ast_declarations.h"

namespace objc3c::parse {

void CopyObjc3MethodReturnTypeFromFunctionDecl(const FunctionDecl &source,
                                               Objc3MethodDecl &target);

void CopyObjc3PropertyTypeFromParam(const FuncParam &source,
                                    Objc3PropertyDecl &target);

}  // namespace objc3c::parse
