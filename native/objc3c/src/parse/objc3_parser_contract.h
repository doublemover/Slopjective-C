#pragma once

#include <cstddef>
#include <cstdint>

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
  std::uint64_t ast_shape_fingerprint = 0;
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

inline std::uint64_t MixObjc3ParserContractFingerprint(const std::uint64_t fingerprint, const std::uint64_t value) {
  constexpr std::uint64_t kMixConstant = 1099511628211ull;
  return (fingerprint ^ value) * kMixConstant;
}

inline std::uint64_t MixObjc3ParserContractFingerprintString(std::uint64_t fingerprint, const std::string &value) {
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(value.size()));
  for (const unsigned char c : value) {
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(c));
  }
  return fingerprint;
}

inline std::uint64_t BuildObjc3ParsedProgramAstShapeFingerprint(const Objc3ParsedProgram &program) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, ast.module_name);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.globals.size()));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.protocols.size()));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.interfaces.size()));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.implementations.size()));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.functions.size()));

  for (const auto &global : ast.globals) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, global.name);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(global.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(global.column));
  }
  for (const auto &protocol_decl : ast.protocols) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, protocol_decl.name);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(protocol_decl.properties.size()));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(protocol_decl.methods.size()));
    fingerprint =
        MixObjc3ParserContractFingerprint(fingerprint, protocol_decl.is_forward_declaration ? 1ull : 0ull);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(protocol_decl.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(protocol_decl.column));
  }
  for (const auto &interface_decl : ast.interfaces) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, interface_decl.name);
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, interface_decl.super_name);
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, interface_decl.category_name);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, interface_decl.has_category ? 1ull : 0ull);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(interface_decl.properties.size()));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(interface_decl.methods.size()));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(interface_decl.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(interface_decl.column));
  }
  for (const auto &implementation_decl : ast.implementations) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, implementation_decl.name);
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, implementation_decl.category_name);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, implementation_decl.has_category ? 1ull : 0ull);
    fingerprint =
        MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(implementation_decl.properties.size()));
    fingerprint =
        MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(implementation_decl.methods.size()));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(implementation_decl.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(implementation_decl.column));
  }
  for (const auto &function_decl : ast.functions) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, function_decl.name);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(function_decl.params.size()));
    fingerprint = MixObjc3ParserContractFingerprint(
        fingerprint,
        static_cast<std::uint64_t>(static_cast<std::uint8_t>(function_decl.return_type)));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, function_decl.is_prototype ? 1ull : 0ull);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, function_decl.is_pure ? 1ull : 0ull);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(function_decl.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(function_decl.column));
  }
  return fingerprint;
}

inline std::uint64_t BuildObjc3ParserContractSnapshotFingerprint(const Objc3ParserContractSnapshot &snapshot) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.token_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.top_level_declaration_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.global_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.protocol_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.interface_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.implementation_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.function_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.parser_diagnostic_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_shape_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.deterministic_handoff ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.parser_recovery_replay_ready ? 1ull : 0ull);
  return fingerprint;
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
  snapshot.ast_shape_fingerprint = BuildObjc3ParsedProgramAstShapeFingerprint(program);
  snapshot.top_level_declaration_count = snapshot.global_decl_count + snapshot.protocol_decl_count +
                                         snapshot.interface_decl_count + snapshot.implementation_decl_count +
                                         snapshot.function_decl_count;
  snapshot.parser_diagnostic_count = parser_diagnostic_count;
  snapshot.deterministic_handoff = true;
  snapshot.parser_recovery_replay_ready = true;
  return snapshot;
}
