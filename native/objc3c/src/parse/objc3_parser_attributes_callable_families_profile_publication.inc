  template <typename TCallableDecl>
  void PublishRetainableCFamilyCallableProfile(TCallableDecl &decl) {
    decl.retainable_c_family_profile_is_normalized = true;
    decl.retainable_c_family_profile = BuildRetainableCFamilyCallableProfile(
        decl.retainable_c_family_callable_attributes,
        decl.retainable_c_family_names);
  }

  template <typename TCallableDecl>
  void RecordRetainableCFamilyCallableFlagAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    decl.retainable_c_family_callable_attributes.push_back(attribute_name.text);
    PublishRetainableCFamilyCallableProfile(decl);
  }
