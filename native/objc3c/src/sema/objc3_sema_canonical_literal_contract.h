#pragma once

#include <cstddef>

#include "frontend/types/canonical_literal_rejection_counts.h"

using Objc3SemaCanonicalLiteralRejectionCounts =
    Objc3CanonicalLiteralRejectionCounts;

inline bool IsReadyObjc3SemaCanonicalLiteralRejectionContract(
    const Objc3SemaCanonicalLiteralRejectionCounts &counts,
    std::size_t parser_token_count) {
  return DoObjc3CanonicalLiteralRejectionsFitTokenBudget(
             counts, parser_token_count) &&
         AreObjc3CanonicalLiteralRejectionsCleared(counts);
}
