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
  entries.reserve(ast.module_imports.size() + ast.globals.size() + ast.protocols.size() +
                  ast.interfaces.size() + ast.implementations.size() + ast.functions.size());

  for (const auto &module_import : ast.module_imports) {
    entries.push_back(Objc3TopLevelLayoutEntry{6ull, module_import.module_name, module_import.line, module_import.column});
  }
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
