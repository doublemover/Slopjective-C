#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "parse/objc3_parser_contract.h"

struct Objc3FrontendDiagnosticsBus {
  std::vector<std::string> lexer;
  std::vector<std::string> parser;
  std::vector<std::string> semantic;

  [[nodiscard]] bool empty() const { return lexer.empty() && parser.empty() && semantic.empty(); }

  [[nodiscard]] std::size_t size() const { return lexer.size() + parser.size() + semantic.size(); }
};

inline void TransportObjc3DiagnosticsToParsedProgram(const Objc3FrontendDiagnosticsBus &bus, Objc3ParsedProgram &program) {
  Objc3Program &ast = MutableObjc3ParsedProgramAst(program);
  ast.diagnostics.clear();
  ast.diagnostics.reserve(bus.size());
  ast.diagnostics.insert(ast.diagnostics.end(), bus.lexer.begin(), bus.lexer.end());
  ast.diagnostics.insert(ast.diagnostics.end(), bus.parser.begin(), bus.parser.end());
  ast.diagnostics.insert(ast.diagnostics.end(), bus.semantic.begin(), bus.semantic.end());
}
