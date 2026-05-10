  summary.canonical_reference_form_count = kObjc3CanonicalReferenceTypeForms.size();
  summary.canonical_message_scalar_form_count = kObjc3CanonicalScalarMessageSendTypeForms.size();
  summary.canonical_bridge_top_form_count = kObjc3CanonicalBridgeTopReferenceTypeForms.size();
  summary.canonical_reference_forms_unique = HasUniqueValueTypes(kObjc3CanonicalReferenceTypeForms);
  summary.canonical_message_scalar_forms_unique = HasUniqueValueTypes(kObjc3CanonicalScalarMessageSendTypeForms);
  summary.canonical_bridge_top_forms_unique = HasUniqueValueTypes(kObjc3CanonicalBridgeTopReferenceTypeForms);
  summary.canonical_message_scalars_disjoint_from_reference =
      AreDisjointFromCanonicalReferenceForms(kObjc3CanonicalScalarMessageSendTypeForms);
  summary.canonical_bridge_top_subset_of_reference =
      IsSubsetOfCanonicalReferenceForms(kObjc3CanonicalBridgeTopReferenceTypeForms);
  summary.canonical_bridge_top_excludes_sel =
      !ContainsValueType(kObjc3CanonicalBridgeTopReferenceTypeForms, ValueType::ObjCSel);
  summary.canonical_reference_includes_sel =
      ContainsValueType(kObjc3CanonicalReferenceTypeForms, ValueType::ObjCSel);
  summary.canonical_message_scalars_include_i32 =
      ContainsValueType(kObjc3CanonicalScalarMessageSendTypeForms, ValueType::I32);
  summary.canonical_message_scalars_include_bool =
      ContainsValueType(kObjc3CanonicalScalarMessageSendTypeForms, ValueType::Bool);
  summary.canonical_forms_exclude_unknown =
      ExcludesUnknownValueType(kObjc3CanonicalReferenceTypeForms) &&
      ExcludesUnknownValueType(kObjc3CanonicalScalarMessageSendTypeForms) &&
      ExcludesUnknownValueType(kObjc3CanonicalBridgeTopReferenceTypeForms);
  summary.canonical_bridge_top_matches_reference_without_sel =
      IsCanonicalBridgeTopEquivalentToReferenceWithoutSel(kObjc3CanonicalReferenceTypeForms,
                                                          kObjc3CanonicalBridgeTopReferenceTypeForms);
