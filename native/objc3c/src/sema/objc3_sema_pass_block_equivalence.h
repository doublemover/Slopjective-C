#pragma once

#include <vector>

#include "sema/objc3_sema_contract.h"

bool IsEquivalentBlockLiteralCaptureSemanticsSummary(
    const Objc3BlockLiteralCaptureSemanticsSummary &lhs,
    const Objc3BlockLiteralCaptureSemanticsSummary &rhs);
bool IsEquivalentBlockAbiInvokeTrampolineSemanticsSummary(
    const Objc3BlockAbiInvokeTrampolineSemanticsSummary &lhs,
    const Objc3BlockAbiInvokeTrampolineSemanticsSummary &rhs);
bool IsEquivalentBlockStorageEscapeSemanticsSummary(
    const Objc3BlockStorageEscapeSemanticsSummary &lhs,
    const Objc3BlockStorageEscapeSemanticsSummary &rhs);
bool IsEquivalentBlockCopyDisposeSemanticsSummary(
    const Objc3BlockCopyDisposeSemanticsSummary &lhs,
    const Objc3BlockCopyDisposeSemanticsSummary &rhs);
bool IsEquivalentBlockDeterminismPerfBaselineSummary(
    const Objc3BlockDeterminismPerfBaselineSummary &lhs,
    const Objc3BlockDeterminismPerfBaselineSummary &rhs);
bool AreEquivalentBlockDeterminismPerfBaselineSites(
    const std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &lhs,
    const std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &rhs);
