inline Objc3ParserContractSnapshot BuildObjc3ParserContractSnapshot(
    const Objc3ParsedProgram &program,
    const std::size_t parser_diagnostic_count) {
  return BuildObjc3ParserContractSnapshot(program, parser_diagnostic_count, 0u);
}
