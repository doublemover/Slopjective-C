#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "parse/objc3_parser_contract.h"

struct Objc3FrontendDiagnosticsBus {
  std::vector<std::string> lexer;
  std::vector<std::string> parser;
  std::vector<std::string> semantic;
  std::string owner_contract_id = std::string(kObjc3DiagnosticOwnerContractId);
  std::string owner_model = std::string(kObjc3DiagnosticNoRetiredRouteOwnerModel);
  std::string parser_stage_owner = std::string(kObjc3ParserDiagnosticStageOwner);
  std::string sema_stage_owner = std::string(kObjc3SemaDiagnosticStageOwner);
  bool retired_route_allowed = false;
  bool compatibility_gate_allowed = false;
  bool recovery_counts_as_success = false;

  [[nodiscard]] bool empty() const { return lexer.empty() && parser.empty() && semantic.empty(); }

  [[nodiscard]] std::size_t size() const { return lexer.size() + parser.size() + semantic.size(); }

  [[nodiscard]] bool hard_cutover_owned() const {
    return owner_contract_id == kObjc3DiagnosticOwnerContractId &&
           owner_model == kObjc3DiagnosticNoRetiredRouteOwnerModel &&
           Objc3DiagnosticOwnerIsExplicit(parser_stage_owner) &&
           Objc3DiagnosticOwnerIsExplicit(sema_stage_owner) &&
           !retired_route_allowed && !compatibility_gate_allowed &&
           !recovery_counts_as_success &&
           Objc3DiagnosticStageIsHardCutover(Objc3FrontendDiagnosticStage::kParser) &&
           Objc3DiagnosticStageIsHardCutover(Objc3FrontendDiagnosticStage::kSemantic);
  }
};

inline void TransportObjc3DiagnosticsToParsedProgram(const Objc3FrontendDiagnosticsBus &bus, Objc3ParsedProgram &program) {
  Objc3Program &ast = MutableObjc3ParsedProgramAst(program);
  ast.diagnostics.clear();
  ast.diagnostics.reserve(bus.size());
  ast.diagnostics.insert(ast.diagnostics.end(), bus.lexer.begin(), bus.lexer.end());
  ast.diagnostics.insert(ast.diagnostics.end(), bus.parser.begin(), bus.parser.end());
  ast.diagnostics.insert(ast.diagnostics.end(), bus.semantic.begin(), bus.semantic.end());
}
