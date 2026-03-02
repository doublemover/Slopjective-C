#pragma once

#include <cstddef>

#include "ast/objc3_ast.h"

// Parser-to-sema contract types. Keep parser outputs wrapped so downstream
// lowering/IR/runtime consumers bind to explicit parser contracts.
struct Objc3ParsedProgram {
  Objc3Program ast;
};

struct Objc3ParserContractSnapshot {
  std::size_t token_count = 0;
  std::size_t top_level_declaration_count = 0;
  std::size_t global_decl_count = 0;
  std::size_t protocol_decl_count = 0;
  std::size_t interface_decl_count = 0;
  std::size_t implementation_decl_count = 0;
  std::size_t function_decl_count = 0;
  std::size_t parser_diagnostic_count = 0;
  bool deterministic_handoff = true;
  bool parser_recovery_replay_ready = true;
};

using Objc3ParsedGlobalDecl = GlobalDecl;
using Objc3ParsedProtocolDecl = Objc3ProtocolDecl;
using Objc3ParsedInterfaceDecl = Objc3InterfaceDecl;
using Objc3ParsedImplementationDecl = Objc3ImplementationDecl;
using Objc3ParsedFunctionDecl = FunctionDecl;

inline Objc3Program &MutableObjc3ParsedProgramAst(Objc3ParsedProgram &program) {
  return program.ast;
}

inline const Objc3Program &Objc3ParsedProgramAst(const Objc3ParsedProgram &program) {
  return program.ast;
}

inline Objc3ParserContractSnapshot BuildObjc3ParserContractSnapshot(
    const Objc3ParsedProgram &program,
    const std::size_t parser_diagnostic_count,
    const std::size_t token_count) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  Objc3ParserContractSnapshot snapshot;
  snapshot.token_count = token_count;
  snapshot.global_decl_count = ast.globals.size();
  snapshot.protocol_decl_count = ast.protocols.size();
  snapshot.interface_decl_count = ast.interfaces.size();
  snapshot.implementation_decl_count = ast.implementations.size();
  snapshot.function_decl_count = ast.functions.size();
  snapshot.top_level_declaration_count = snapshot.global_decl_count + snapshot.protocol_decl_count +
                                         snapshot.interface_decl_count + snapshot.implementation_decl_count +
                                         snapshot.function_decl_count;
  snapshot.parser_diagnostic_count = parser_diagnostic_count;
  snapshot.deterministic_handoff = true;
  snapshot.parser_recovery_replay_ready = true;
  return snapshot;
}
