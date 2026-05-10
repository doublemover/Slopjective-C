#pragma once

#include "parser/frontend/objc3_parser_type_projection.h"

namespace objc3c::parse {

void CopyObjc3MethodReturnTypeCoreProjection(const FunctionDecl &source,
                                             Objc3MethodDecl &target);

void CopyObjc3MethodReturnTypeErrorProjection(const FunctionDecl &source,
                                              Objc3MethodDecl &target);

void CopyObjc3MethodReturnTypeConcurrencyProjection(const FunctionDecl &source,
                                                    Objc3MethodDecl &target);

void CopyObjc3MethodReturnTypeUnsafeProjection(const FunctionDecl &source,
                                               Objc3MethodDecl &target);

}  // namespace objc3c::parse
