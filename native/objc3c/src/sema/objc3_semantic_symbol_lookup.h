#pragma once

#include <string>
#include <vector>

#include "sema/objc3_semantic_type_helpers.h"

SemanticTypeInfo ScopeLookupType(
    const std::vector<SemanticScope> &scopes,
    const std::string &name);
const SemanticTypeInfo *ScopeLookupTypeOrNull(
    const std::vector<SemanticScope> &scopes,
    const std::string &name);
OwnershipResourceMoveBindingState *LookupOwnershipResourceMoveBinding(
    std::vector<OwnershipResourceMoveScope> &scopes,
    const std::string &name);
const OwnershipResourceMoveBindingState *LookupOwnershipResourceMoveBinding(
    const std::vector<OwnershipResourceMoveScope> &scopes,
    const std::string &name);
