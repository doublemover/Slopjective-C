#pragma once

#include <cstddef>
#include <string>

#include "frontend/types/canonical_literal_rejection_counts.h"
#include "parse/objc3_parser_contract_types.h"

struct Objc3ParserCanonicalLiteralRejectionHandoff {
  Objc3CanonicalLiteralRejectionCounts counts;
  std::size_t parser_token_count = 0;
  bool token_budget_consistent = false;
  bool canonical_literals_rejected = false;
  bool deterministic = false;
  std::string replay_key;
};

inline Objc3ParserCanonicalLiteralRejectionHandoff
BuildObjc3ParserCanonicalLiteralRejectionHandoff(
    const Objc3ParserContractSnapshot &snapshot,
    const Objc3CanonicalLiteralRejectionCounts &counts) {
  Objc3ParserCanonicalLiteralRejectionHandoff handoff;
  handoff.counts = counts;
  handoff.parser_token_count = snapshot.token_count;
  handoff.token_budget_consistent =
      DoObjc3CanonicalLiteralRejectionsFitTokenBudget(
          counts, handoff.parser_token_count);
  handoff.canonical_literals_rejected =
      AreObjc3CanonicalLiteralRejectionsCleared(counts);
  handoff.deterministic =
      snapshot.deterministic_handoff && handoff.token_budget_consistent &&
      handoff.canonical_literals_rejected;
  handoff.replay_key = BuildObjc3CanonicalLiteralRejectionReplayKey(
      counts, handoff.parser_token_count);
  return handoff;
}

inline bool IsReadyObjc3ParserCanonicalLiteralRejectionHandoff(
    const Objc3ParserCanonicalLiteralRejectionHandoff &handoff) {
  return handoff.token_budget_consistent &&
         handoff.canonical_literals_rejected && handoff.deterministic &&
         !handoff.replay_key.empty();
}
