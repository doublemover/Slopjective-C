  void FinalizeThrowsDeclarationProfile(FunctionDecl &fn, bool has_return_annotation) {
    fn.throws_declaration_profile = BuildThrowsDeclarationProfile(
        fn.throws_declared,
        has_return_annotation,
        fn.is_prototype,
        !fn.is_prototype,
        false,
        false,
        fn.params.size(),
        0u,
        fn.typed_throws_declared,
        fn.typed_throws_payload.canonical_spelling,
        fn.typed_throws_payload.generic_suffix_terminated,
        fn.typed_throws_payload.pointer_declarator_depth);
    fn.throws_declaration_profile_is_normalized = IsThrowsDeclarationProfileNormalized(
        fn.is_prototype,
        !fn.is_prototype,
        false,
        0u,
        fn.throws_declared,
        fn.typed_throws_declared,
        fn.typed_throws_payload.canonical_spelling,
        fn.typed_throws_payload.generic_suffix_terminated);
  }

  void FinalizeThrowsDeclarationProfile(Objc3MethodDecl &method) {
    method.throws_declaration_profile = BuildThrowsDeclarationProfile(
        method.throws_declared,
        true,
        !method.has_body,
        method.has_body,
        true,
        method.is_class_method,
        method.params.size(),
        method.selector_pieces.size(),
        method.typed_throws_declared,
        method.typed_throws_payload.canonical_spelling,
        method.typed_throws_payload.generic_suffix_terminated,
        method.typed_throws_payload.pointer_declarator_depth);
    method.throws_declaration_profile_is_normalized = IsThrowsDeclarationProfileNormalized(
        !method.has_body,
        method.has_body,
        true,
        method.selector_pieces.size(),
        method.throws_declared,
        method.typed_throws_declared,
        method.typed_throws_payload.canonical_spelling,
        method.typed_throws_payload.generic_suffix_terminated);
  }
