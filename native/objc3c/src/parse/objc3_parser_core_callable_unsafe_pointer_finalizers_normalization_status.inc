  void PublishUnsafePointerExtensionNormalizationStatus(FunctionDecl &fn) {
    fn.unsafe_pointer_extension_profile_is_normalized =
        IsUnsafePointerExtensionProfileNormalized(
            fn.unsafe_pointer_extension_sites,
            fn.unsafe_keyword_sites,
            fn.pointer_arithmetic_sites,
            fn.raw_pointer_type_sites,
            fn.unsafe_operation_sites,
            fn.unsafe_pointer_extension_normalized_sites,
            fn.unsafe_pointer_extension_gate_blocked_sites,
            fn.unsafe_pointer_extension_contract_violation_sites);
  }

  void PublishUnsafePointerExtensionNormalizationStatus(
      Objc3MethodDecl &method) {
    method.unsafe_pointer_extension_profile_is_normalized =
        IsUnsafePointerExtensionProfileNormalized(
            method.unsafe_pointer_extension_sites,
            method.unsafe_keyword_sites,
            method.pointer_arithmetic_sites,
            method.raw_pointer_type_sites,
            method.unsafe_operation_sites,
            method.unsafe_pointer_extension_normalized_sites,
            method.unsafe_pointer_extension_gate_blocked_sites,
            method.unsafe_pointer_extension_contract_violation_sites);
  }
