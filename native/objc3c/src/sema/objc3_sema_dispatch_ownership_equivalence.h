#pragma once

#include "sema/objc3_sema_contract.h"

bool IsEquivalentMessageSendSelectorLoweringSummary(
    const Objc3MessageSendSelectorLoweringSummary &lhs,
    const Objc3MessageSendSelectorLoweringSummary &rhs);
bool IsEquivalentDispatchAbiMarshallingSummary(
    const Objc3DispatchAbiMarshallingSummary &lhs,
    const Objc3DispatchAbiMarshallingSummary &rhs);
bool IsEquivalentNilReceiverSemanticsFoldabilitySummary(
    const Objc3NilReceiverSemanticsFoldabilitySummary &lhs,
    const Objc3NilReceiverSemanticsFoldabilitySummary &rhs);
bool IsEquivalentSuperDispatchMethodFamilySummary(
    const Objc3SuperDispatchMethodFamilySummary &lhs,
    const Objc3SuperDispatchMethodFamilySummary &rhs);
bool IsEquivalentRuntimeLinkHostLinkSummary(
    const Objc3RuntimeLinkHostLinkSummary &lhs,
    const Objc3RuntimeLinkHostLinkSummary &rhs);
bool IsEquivalentRetainReleaseOperationSummary(
    const Objc3RetainReleaseOperationSummary &lhs,
    const Objc3RetainReleaseOperationSummary &rhs);
bool IsEquivalentWeakUnownedSemanticsSummary(
    const Objc3WeakUnownedSemanticsSummary &lhs,
    const Objc3WeakUnownedSemanticsSummary &rhs);
bool IsEquivalentArcDiagnosticsFixitSummary(
    const Objc3ArcDiagnosticsFixitSummary &lhs,
    const Objc3ArcDiagnosticsFixitSummary &rhs);
bool IsEquivalentAutoreleasePoolScopeSummary(
    const Objc3AutoreleasePoolScopeSummary &lhs,
    const Objc3AutoreleasePoolScopeSummary &rhs);
