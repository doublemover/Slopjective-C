#include "parse/objc3_parse_phase_io.h"

#include <utility>

#include "parse/objc3_parser_contract.h"

Objc3ParseResult BuildObjc3ParseResult(
    Objc3ParsedProgram program,
    std::vector<std::string> diagnostics,
    std::size_t token_count) {
  Objc3ParseResult result;
  result.program = std::move(program);
  result.diagnostics = std::move(diagnostics);
  result.contract_snapshot = BuildObjc3ParserContractSnapshot(
      result.program, result.diagnostics.size(), token_count);
  return result;
}
