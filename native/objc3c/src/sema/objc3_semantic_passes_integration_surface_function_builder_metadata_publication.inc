      info.has_definition = !fn.is_prototype;
      info.is_pure_annotation = fn.is_pure;
      info.throws_declared = fn.throws_declared;
      info.typed_throws_declared = fn.typed_throws_declared;
      info.typed_throws_error_type_spelling =
          fn.typed_throws_payload.canonical_spelling;
      info.typed_throws_effect_signature_key =
          Objc3BuildTypedThrowsEffectSignatureKey(
              fn.throws_declared,
              fn.typed_throws_declared,
              fn.typed_throws_payload.canonical_spelling);
      info.typed_throws_callable_compatibility_policy =
          Objc3BuildTypedThrowsCallableCompatibilityPolicy(
              fn.throws_declared,
              fn.typed_throws_declared);
      info.typed_throws_abi_lowering_ready = false;
      const bool has_macro_marker = fn.objc_macro_declared;
      const bool has_macro_package = fn.objc_macro_package_declared;
      const bool has_macro_provenance = fn.objc_macro_provenance_declared;
      const bool has_macro_cache_key = fn.objc_macro_cache_key_declared;
      const bool has_macro_sandbox_policy = fn.objc_macro_sandbox_declared;
      if (has_macro_package || has_macro_provenance || has_macro_cache_key ||
          has_macro_sandbox_policy) {
        if (!has_macro_marker) {
          diagnostics.push_back(MakeDiag(
              fn.line, fn.column, "O3S321",
              "macro package/provenance/cache/sandbox markers require objc_macro on function '" +
                  fn.name + "'"));
        }
      }
      if (has_macro_marker) {
        if (!has_macro_package || !has_macro_provenance) {
          diagnostics.push_back(MakeDiag(
              fn.line, fn.column, "O3S320",
              "macro callable '" + fn.name +
                  "' requires both objc_macro_package and objc_macro_provenance"));
        }
        if (has_macro_package &&
            !IsSupportedMetaprogrammingMacroPackageName(fn.objc_macro_package_name)) {
          diagnostics.push_back(MakeDiag(
              fn.line, fn.column, "O3S322",
              "macro sandbox rejected package '" +
                  NormalizeMetaprogrammingRawMacroToken(fn.objc_macro_package_name) +
                  "' on function '" + fn.name + "'"));
        }
        if (has_macro_provenance &&
            !IsSupportedMetaprogrammingMacroProvenanceToken(
                fn.objc_macro_provenance_name)) {
          diagnostics.push_back(MakeDiag(
              fn.line, fn.column, "O3S323",
              "macro provenance must be a lowercase sha256 digest on function '" +
                  fn.name + "'"));
        }
        if (has_macro_package && has_macro_provenance) {
          if (!has_macro_cache_key ||
              !IsSupportedMetaprogrammingMacroCacheKeyToken(
                  fn.objc_macro_cache_key_name)) {
            diagnostics.push_back(MakeDiag(
                fn.line, fn.column, "O3S331",
                "macro callable '" + fn.name +
                    "' requires stable objc_macro_cache_key for deterministic cache invalidation"));
          }
          if (!has_macro_sandbox_policy ||
              !IsSupportedMetaprogrammingMacroSandboxPolicy(
                  fn.objc_macro_sandbox_name)) {
            diagnostics.push_back(MakeDiag(
                fn.line, fn.column, "O3S332",
                "macro callable '" + fn.name +
                    "' requires objc_macro_sandbox(named(\"deterministic\")) for deny-by-default sandbox admission"));
          }
        }
        if (!fn.is_pure || fn.is_prototype || fn.async_declared ||
            fn.throws_declared) {
          diagnostics.push_back(MakeDiag(
              fn.line, fn.column, "O3S324",
              "macro callable '" + fn.name +
                  "' must be pure, body-backed, non-async, and non-throws"));
        }
      }
      surface.functions.emplace(fn.name, std::move(info));
      continue;
