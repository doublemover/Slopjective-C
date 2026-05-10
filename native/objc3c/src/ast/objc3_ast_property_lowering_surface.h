#pragma once

#include <string>

#include "ast/objc3_ast_property_decl_nodes.h"

bool Objc3PropertyDeclHasRuntimeBackedStorage(
    const Objc3PropertyDecl &property);
bool Objc3PropertyDeclRequiresOwnershipRuntime(
    const Objc3PropertyDecl &property);
std::string Objc3PropertyDeclLoweringReplayKey(
    const Objc3PropertyDecl &property);
