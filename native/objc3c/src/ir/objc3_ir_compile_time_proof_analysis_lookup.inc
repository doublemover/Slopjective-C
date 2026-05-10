namespace {

int LookupImmediateIdentifierValue(const FunctionContext &ctx,
                                   const std::string &name) {
  const auto value_it = ctx.immediate_identifiers.find(name);
  if (value_it == ctx.immediate_identifiers.end()) {
    return 0;
  }
  return value_it->second;
}

}  // namespace
