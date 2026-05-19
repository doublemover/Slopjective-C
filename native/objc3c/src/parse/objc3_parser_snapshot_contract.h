#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>

#include "parse/objc3_parser_ast_fingerprints_contract.h"
#include "parse/objc3_parser_contract_fingerprints.h"
#include "parse/objc3_parser_contract_types.h"
#include "parse/objc3_parser_draft_syntax_surface_contract.h"
#include "parse/objc3_parser_long_tail_grammar_contract.h"

inline std::uint64_t BuildObjc3ParserContractSnapshotFingerprint(const Objc3ParserContractSnapshot &snapshot) {
#include "parse/objc3_parser_snapshot_contract_fingerprint_mixing.inc"
}

inline Objc3ParserContractSnapshot BuildObjc3ParserContractSnapshot(
    const Objc3ParsedProgram &program,
    const std::size_t parser_diagnostic_count,
    const std::size_t token_count) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  Objc3ParserContractSnapshot snapshot;
#include "parse/objc3_parser_snapshot_contract_declaration_counts.inc"
#include "parse/objc3_parser_snapshot_contract_draft_syntax_handoff.inc"
#include "parse/objc3_parser_snapshot_contract_diagnostic_handoff.inc"
#include "parse/objc3_parser_snapshot_contract_long_tail_handoff.inc"
  return snapshot;
}

#include "parse/objc3_parser_snapshot_contract_final_overloads.inc"
