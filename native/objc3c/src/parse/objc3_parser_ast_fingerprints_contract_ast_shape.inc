inline std::uint64_t BuildObjc3ParsedProgramAstShapeFingerprint(const Objc3ParsedProgram &program) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, ast.module_name);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.module_imports.size()));
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

  for (const auto &module_import : ast.module_imports) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, module_import.module_name);
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, module_import.module_identity_key);
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, module_import.package_resolution);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(module_import.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(module_import.column));
  }
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
