#pragma once

#include <string>

#include "ast/objc3_ast_method_decl_nodes.h"

bool Objc3MethodDeclHasRuntimeBody(const Objc3MethodDecl &method);
bool Objc3MethodDeclRequiresRuntimeDispatch(const Objc3MethodDecl &method);
std::string Objc3MethodSignatureReplayKey(const Objc3MethodDecl &method);
