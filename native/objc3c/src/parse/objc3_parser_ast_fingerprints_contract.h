#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "parse/objc3_parser_contract_fingerprints.h"
#include "parse/objc3_parser_contract_types.h"

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
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(ast.draft_syntax_surface_summary.draft_syntax_surface_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      ast.draft_syntax_surface_summary.normalized ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      ast.draft_syntax_surface_summary.replay_key);

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

inline std::uint64_t BuildObjc3ParsedProgramTopLevelLayoutFingerprint(const Objc3ParsedProgram &program) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;

  struct Objc3TopLevelLayoutEntry {
    std::uint64_t kind_tag = 0;
    std::string symbol;
    unsigned line = 1;
    unsigned column = 1;
  };

  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::vector<Objc3TopLevelLayoutEntry> entries;
  entries.reserve(ast.globals.size() + ast.protocols.size() + ast.interfaces.size() + ast.implementations.size() +
                  ast.functions.size());

  for (const auto &global : ast.globals) {
    entries.push_back(Objc3TopLevelLayoutEntry{1ull, global.name, global.line, global.column});
  }
  for (const auto &protocol_decl : ast.protocols) {
    entries.push_back(Objc3TopLevelLayoutEntry{2ull, protocol_decl.name, protocol_decl.line, protocol_decl.column});
  }
  for (const auto &interface_decl : ast.interfaces) {
    entries.push_back(Objc3TopLevelLayoutEntry{3ull, interface_decl.name, interface_decl.line, interface_decl.column});
  }
  for (const auto &implementation_decl : ast.implementations) {
    entries.push_back(
        Objc3TopLevelLayoutEntry{4ull, implementation_decl.name, implementation_decl.line, implementation_decl.column});
  }
  for (const auto &function_decl : ast.functions) {
    entries.push_back(Objc3TopLevelLayoutEntry{5ull, function_decl.name, function_decl.line, function_decl.column});
  }

  std::sort(entries.begin(), entries.end(), [](const Objc3TopLevelLayoutEntry &lhs, const Objc3TopLevelLayoutEntry &rhs) {
    if (lhs.line != rhs.line) {
      return lhs.line < rhs.line;
    }
    if (lhs.column != rhs.column) {
      return lhs.column < rhs.column;
    }
    if (lhs.kind_tag != rhs.kind_tag) {
      return lhs.kind_tag < rhs.kind_tag;
    }
    return lhs.symbol < rhs.symbol;
  });

  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, ast.module_name);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(entries.size()));
  for (const auto &entry : entries) {
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, entry.kind_tag);
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, entry.symbol);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(entry.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(entry.column));
  }
  return fingerprint;
}
