  void RebuildUnsafePointerExtensionProfile(FunctionDecl &fn) {
    fn.unsafe_pointer_extension_profile = BuildUnsafePointerExtensionProfile(
        fn.unsafe_pointer_extension_sites,
        fn.unsafe_keyword_sites,
        fn.pointer_arithmetic_sites,
        fn.raw_pointer_type_sites,
        fn.unsafe_operation_sites,
        fn.unsafe_pointer_extension_normalized_sites,
        fn.unsafe_pointer_extension_gate_blocked_sites,
        fn.unsafe_pointer_extension_contract_violation_sites,
        fn.deterministic_unsafe_pointer_extension_handoff);
  }

  void RebuildUnsafePointerExtensionProfile(Objc3MethodDecl &method) {
    method.unsafe_pointer_extension_profile = BuildUnsafePointerExtensionProfile(
        method.unsafe_pointer_extension_sites,
        method.unsafe_keyword_sites,
        method.pointer_arithmetic_sites,
        method.raw_pointer_type_sites,
        method.unsafe_operation_sites,
        method.unsafe_pointer_extension_normalized_sites,
        method.unsafe_pointer_extension_gate_blocked_sites,
        method.unsafe_pointer_extension_contract_violation_sites,
        method.deterministic_unsafe_pointer_extension_handoff);
  }
