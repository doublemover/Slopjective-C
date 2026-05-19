#pragma once

#include "sema/objc3_sema_contract.h"

bool IsEquivalentConcurrencyReplayRaceGuardSummary(
    const Objc3ConcurrencyReplayRaceGuardSummary &lhs,
    const Objc3ConcurrencyReplayRaceGuardSummary &rhs);
bool IsEquivalentUnsafePointerExtensionSummary(
    const Objc3UnsafePointerExtensionSummary &lhs,
    const Objc3UnsafePointerExtensionSummary &rhs);
bool IsEquivalentInlineAsmIntrinsicGovernanceSummary(
    const Objc3InlineAsmIntrinsicGovernanceSummary &lhs,
    const Objc3InlineAsmIntrinsicGovernanceSummary &rhs);
bool IsEquivalentNSErrorBridgingSummary(
    const Objc3NSErrorBridgingSummary &lhs,
    const Objc3NSErrorBridgingSummary &rhs);
bool IsEquivalentErrorDiagnosticsRecoverySummary(
    const Objc3ErrorDiagnosticsRecoverySummary &lhs,
    const Objc3ErrorDiagnosticsRecoverySummary &rhs);
bool IsEquivalentResultLikeLoweringSummary(
    const Objc3ResultLikeLoweringSummary &lhs,
    const Objc3ResultLikeLoweringSummary &rhs);
bool IsEquivalentUnwindCleanupSummary(
    const Objc3UnwindCleanupSummary &lhs,
    const Objc3UnwindCleanupSummary &rhs);
bool IsEquivalentAwaitLoweringSuspensionStateSummary(
    const Objc3AwaitLoweringSuspensionStateSummary &lhs,
    const Objc3AwaitLoweringSuspensionStateSummary &rhs);
