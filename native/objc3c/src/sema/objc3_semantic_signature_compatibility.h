#pragma once

#include <string>
#include <vector>

#include "sema/objc3_sema_contract_type_handoff.h"

bool AreEquivalentProtocolCompositions(
    bool lhs_has_composition, const std::vector<std::string> &lhs_names,
    bool rhs_has_composition, const std::vector<std::string> &rhs_names);

bool IsCompatibleCanonicalSemanticType(
    const Objc3SemanticCanonicalType &lhs,
    const Objc3SemanticCanonicalType &rhs);

bool IsCompatiblePropertySignature(const Objc3PropertyInfo &lhs,
                                   const Objc3PropertyInfo &rhs);

bool IsCompatibleMethodSignature(const Objc3MethodInfo &lhs,
                                 const Objc3MethodInfo &rhs);
