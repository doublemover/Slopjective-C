#pragma once

#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_method_definition_plan.h"

int Objc3IRStablePositiveAsyncTag(const std::string &text);
int Objc3IRExecutorAffinityTag(const FunctionDecl &fn);
int Objc3IRExecutorAffinityTag(const Objc3MethodDecl &method);
int Objc3IRAsyncResumeEntryTag(const FunctionDecl &fn);
int Objc3IRAsyncResumeEntryTag(const Objc3IRMethodDefinition &method_def);
