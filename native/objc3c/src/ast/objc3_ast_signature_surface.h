#pragma once

#include <cstddef>
#include <string>

#include "ast/objc3_ast_declarations.h"

bool Objc3MethodDeclHasRuntimeBody(const Objc3MethodDecl &method);
bool Objc3MethodDeclRequiresRuntimeDispatch(const Objc3MethodDecl &method);
std::string Objc3MethodSignatureReplayKey(const Objc3MethodDecl &method);
std::string Objc3FunctionSignatureReplayKey(const FunctionDecl &function);
bool Objc3PropertyDeclHasRuntimeBackedStorage(
    const Objc3PropertyDecl &property);
bool Objc3PropertyDeclRequiresOwnershipRuntime(
    const Objc3PropertyDecl &property);
std::string Objc3PropertyDeclLoweringReplayKey(
    const Objc3PropertyDecl &property);
std::size_t Objc3ProgramDeclarationCount(const Objc3Program &program);
std::string Objc3ProgramLoweringReplayKey(const Objc3Program &program);
