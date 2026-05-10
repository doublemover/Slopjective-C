  template <typename TCallableDecl>
  void CountDraftSyntaxCallableMacroDiagnostics(
      const TCallableDecl &decl,
      Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (decl.objc_macro_declared) {
      ++summary.macro_attribute_sites;
    }
    if (decl.objc_macro_package_declared) {
      ++summary.macro_package_sites;
    }
    if (decl.objc_macro_provenance_declared) {
      ++summary.macro_provenance_sites;
    }
    if (decl.objc_macro_cache_key_declared) {
      ++summary.macro_cache_key_sites;
    }
    if (decl.objc_macro_sandbox_declared) {
      ++summary.macro_sandbox_policy_sites;
    }
  }
