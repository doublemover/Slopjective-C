  void PublishBlockLiteralParameterNameModel(
      Expr &block,
      const std::vector<Objc3BlockParameterSourceModel> &parameters,
      std::vector<std::string> &parameter_names) {
    parameter_names.reserve(parameters.size());
    for (const auto &parameter : parameters) {
      parameter_names.push_back(parameter.name);
    }
    block.block_parameter_count = parameter_names.size();
    block.block_parameter_names_lexicographic =
        BuildSortedUniqueStrings(parameter_names);
  }

  void PublishBlockLiteralSignatureMetadata(
      Expr &block,
      const std::vector<Objc3BlockParameterSourceModel> &parameters) {
    // block-source-model-completion anchor: parser now publishes the
    // canonical parameter-signature, capture-inventory, and invoke-surface
    // source model directly on the AST so source-only frontend runs can carry
    // that closure forward before runnable block lowering still fails closed.
    // executable-block-object/invoke-thunk anchor: lane-C consumes
    // this retained parameter/body ordering directly when it emits one stack-resident block object plus one internal invoke thunk for the current readonly-scalar capture slice.
    block.block_parameter_signature_entries_lexicographic =
        BuildBlockParameterSignatureEntriesLexicographic(parameters);
    block.block_parameter_types_source_order =
        BuildBlockParameterTypesSourceOrder(parameters);
  }

  ValueType ResolveBlockLiteralParameterValueType(
      const Objc3BlockParameterSourceModel &parameter) {
    if (!parameter.explicit_type) {
      return ValueType::Unknown;
    }
    if (parameter.type_spelling == "i32") {
      return ValueType::I32;
    }
    if (parameter.type_spelling == "bool") {
      return ValueType::Bool;
    }
    if (parameter.type_spelling == "void") {
      return ValueType::Void;
    }
    return ValueType::Unknown;
  }

  void PublishBlockLiteralParameterSourceOrder(
      Expr &block,
      const std::vector<Objc3BlockParameterSourceModel> &parameters) {
    block.block_parameters_source_order.reserve(parameters.size());
    for (const auto &parameter : parameters) {
      Expr::BlockParameter lowered_parameter;
      lowered_parameter.name = parameter.name;
      lowered_parameter.type = ResolveBlockLiteralParameterValueType(parameter);
      block.block_parameters_source_order.push_back(std::move(lowered_parameter));
    }
  }
