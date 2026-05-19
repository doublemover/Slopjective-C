std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
BuildMetaprogrammingMacroArtifactBundles(const Objc3Program &program) {
  std::vector<Objc3IRMetaprogrammingMacroArtifactBundle> bundles;
  for (const auto &fn : program.functions) {
    const bool has_macro_marker =
        fn.objc_macro_declared || !fn.objc_macro_name.empty();
    const bool has_macro_package =
        fn.objc_macro_package_declared || !fn.objc_macro_package_name.empty();
    const bool has_macro_provenance =
        fn.objc_macro_provenance_declared ||
        !fn.objc_macro_provenance_name.empty();
    if (!has_macro_marker || !has_macro_package || !has_macro_provenance ||
        !fn.is_pure || fn.is_prototype || fn.async_declared ||
        fn.throws_declared ||
        !IsSupportedMetaprogrammingMacroPackageNameForEmission(
            fn.objc_macro_package_name) ||
        !IsSupportedMetaprogrammingMacroProvenanceNameForEmission(
            fn.objc_macro_provenance_name)) {
      continue;
    }
    Objc3IRMetaprogrammingMacroArtifactBundle bundle;
    bundle.function_name = fn.name;
    bundle.macro_name = fn.objc_macro_name;
    bundle.package_name = fn.objc_macro_package_name;
    bundle.provenance_name = fn.objc_macro_provenance_name;
    bundle.cache_key_name = fn.objc_macro_cache_key_name;
    bundle.sandbox_policy_name = fn.objc_macro_sandbox_name;
    bundle.emitted_symbol =
        BuildMetaprogrammingMacroArtifactSymbol(fn.name, fn.objc_macro_name);
    bundle.line = fn.line;
    bundle.column = fn.column;
    bundles.push_back(std::move(bundle));
  }
  std::sort(bundles.begin(), bundles.end(),
            [](const Objc3IRMetaprogrammingMacroArtifactBundle &lhs,
               const Objc3IRMetaprogrammingMacroArtifactBundle &rhs) {
              return std::tie(lhs.function_name, lhs.macro_name,
                              lhs.provenance_name, lhs.cache_key_name,
                              lhs.sandbox_policy_name) <
                     std::tie(rhs.function_name, rhs.macro_name,
                              rhs.provenance_name, rhs.cache_key_name,
                              rhs.sandbox_policy_name);
            });
  return bundles;
}
