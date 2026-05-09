#pragma once

#include "sema/objc3_sema_contract.h"

bool IsEquivalentSymbolGraphScopeResolutionSummary(
    const Objc3SymbolGraphScopeResolutionSummary &lhs,
    const Objc3SymbolGraphScopeResolutionSummary &rhs);
bool IsEquivalentClassProtocolCategoryLinkingSummary(
    const Objc3ClassProtocolCategoryLinkingSummary &lhs,
    const Objc3ClassProtocolCategoryLinkingSummary &rhs);
bool IsEquivalentMethodLookupOverrideConflictSummary(
    const Objc3MethodLookupOverrideConflictSummary &lhs,
    const Objc3MethodLookupOverrideConflictSummary &rhs);
bool IsEquivalentPropertySynthesisIvarBindingSummary(
    const Objc3PropertySynthesisIvarBindingSummary &lhs,
    const Objc3PropertySynthesisIvarBindingSummary &rhs);
bool IsEquivalentIdClassSelObjectPointerTypeCheckingSummary(
    const Objc3IdClassSelObjectPointerTypeCheckingSummary &lhs,
    const Objc3IdClassSelObjectPointerTypeCheckingSummary &rhs);
