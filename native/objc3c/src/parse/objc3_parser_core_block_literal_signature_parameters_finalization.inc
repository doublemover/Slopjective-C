  void FinalizeBlockLiteralSignatureProfile(
      Expr &block,
      const std::vector<Objc3BlockParameterSourceModel> &parameters) {
    block.block_explicit_typed_parameter_count = static_cast<std::size_t>(
        std::count_if(parameters.begin(), parameters.end(),
                      [](const Objc3BlockParameterSourceModel &parameter) {
                        return parameter.explicit_type;
                      }));
    block.block_implicit_parameter_count =
        block.block_parameter_count - block.block_explicit_typed_parameter_count;
    block.block_signature_profile = BuildBlockSignatureProfile(parameters);
  }
