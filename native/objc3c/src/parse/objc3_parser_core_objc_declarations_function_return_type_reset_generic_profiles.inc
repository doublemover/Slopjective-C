  void ResetFunctionReturnGenericProfileState(FunctionDecl &fn) {
    fn.has_return_generic_suffix = false;
    fn.return_generic_suffix_terminated = true;
    fn.return_generic_suffix_text.clear();
    fn.return_generic_line = 1;
    fn.return_generic_column = 1;
    fn.return_lightweight_generic_constraint_profile_is_normalized = false;
    fn.return_lightweight_generic_constraint_profile.clear();
    fn.return_nullability_flow_profile_is_normalized = false;
    fn.return_nullability_flow_profile.clear();
    fn.return_protocol_qualified_object_type_profile_is_normalized = false;
    fn.return_protocol_qualified_object_type_profile.clear();
    fn.return_variance_bridge_cast_profile_is_normalized = false;
    fn.return_variance_bridge_cast_profile.clear();
    fn.return_generic_metadata_abi_profile_is_normalized = false;
    fn.return_generic_metadata_abi_profile.clear();
  }
