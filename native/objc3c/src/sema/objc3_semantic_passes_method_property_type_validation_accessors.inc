static std::string MethodSelectorName(const Objc3MethodDecl &method) {
  const MethodSelectorNormalizationContractInfo selector_contract =
      BuildMethodSelectorNormalizationContractInfo(method);
  return selector_contract.normalized_selector;
}
