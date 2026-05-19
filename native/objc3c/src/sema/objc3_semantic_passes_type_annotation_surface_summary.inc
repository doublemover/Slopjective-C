static void AccumulateTypeAnnotationSummaryFromFunctionInfo(const FunctionInfo &function_info,
                                                            Objc3TypeAnnotationSurfaceSummary &summary) {
  const std::size_t arity = function_info.arity;
  if (function_info.param_has_generic_suffix.size() != arity ||
      function_info.param_has_pointer_declarator.size() != arity ||
      function_info.param_has_nullability_suffix.size() != arity ||
      function_info.param_has_ownership_qualifier.size() != arity ||
      function_info.param_id_spelling.size() != arity ||
      function_info.param_class_spelling.size() != arity ||
      function_info.param_instancetype_spelling.size() != arity ||
      function_info.param_object_pointer_type_spelling.size() != arity ||
      function_info.param_has_invalid_generic_suffix.size() != arity ||
      function_info.param_has_invalid_pointer_declarator.size() != arity ||
      function_info.param_has_invalid_nullability_suffix.size() != arity ||
      function_info.param_has_invalid_ownership_qualifier.size() != arity ||
      function_info.param_has_invalid_type_suffix.size() != arity) {
    summary.deterministic = false;
    return;
  }

  for (std::size_t i = 0; i < arity; ++i) {
    if (function_info.param_has_generic_suffix[i]) {
      ++summary.generic_suffix_sites;
    }
    if (function_info.param_has_pointer_declarator[i]) {
      ++summary.pointer_declarator_sites;
    }
    if (function_info.param_has_nullability_suffix[i]) {
      ++summary.nullability_suffix_sites;
    }
    if (function_info.param_has_ownership_qualifier[i]) {
      ++summary.ownership_qualifier_sites;
    }
    if (IsObjCReferenceAnnotationSite(function_info.param_id_spelling[i],
                                      function_info.param_class_spelling[i],
                                      function_info.param_instancetype_spelling[i],
                                      function_info.param_object_pointer_type_spelling[i])) {
      ++summary.object_pointer_type_sites;
    }
    if (function_info.param_has_invalid_generic_suffix[i]) {
      ++summary.invalid_generic_suffix_sites;
    }
    if (function_info.param_has_invalid_pointer_declarator[i]) {
      ++summary.invalid_pointer_declarator_sites;
    }
    if (function_info.param_has_invalid_nullability_suffix[i]) {
      ++summary.invalid_nullability_suffix_sites;
    }
    if (function_info.param_has_invalid_ownership_qualifier[i]) {
      ++summary.invalid_ownership_qualifier_sites;
    }
    const bool expected_invalid =
        function_info.param_has_invalid_generic_suffix[i] ||
        function_info.param_has_invalid_pointer_declarator[i] ||
        function_info.param_has_invalid_nullability_suffix[i] ||
        function_info.param_has_invalid_ownership_qualifier[i];
    if (function_info.param_has_invalid_type_suffix[i] != expected_invalid) {
      summary.deterministic = false;
    }
  }

  if (function_info.return_has_generic_suffix) {
    ++summary.generic_suffix_sites;
  }
  if (function_info.return_has_pointer_declarator) {
    ++summary.pointer_declarator_sites;
  }
  if (function_info.return_has_nullability_suffix) {
    ++summary.nullability_suffix_sites;
  }
  if (function_info.return_has_ownership_qualifier) {
    ++summary.ownership_qualifier_sites;
  }
  if (IsObjCReferenceAnnotationSite(function_info.return_id_spelling,
                                    function_info.return_class_spelling,
                                    function_info.return_instancetype_spelling,
                                    function_info.return_object_pointer_type_spelling)) {
    ++summary.object_pointer_type_sites;
  }
  if (function_info.return_has_invalid_generic_suffix) {
    ++summary.invalid_generic_suffix_sites;
  }
  if (function_info.return_has_invalid_pointer_declarator) {
    ++summary.invalid_pointer_declarator_sites;
  }
  if (function_info.return_has_invalid_nullability_suffix) {
    ++summary.invalid_nullability_suffix_sites;
  }
  if (function_info.return_has_invalid_ownership_qualifier) {
    ++summary.invalid_ownership_qualifier_sites;
  }

  const bool expected_return_invalid = function_info.return_has_invalid_generic_suffix ||
                                       function_info.return_has_invalid_pointer_declarator ||
                                       function_info.return_has_invalid_nullability_suffix ||
                                       function_info.return_has_invalid_ownership_qualifier;
  if (function_info.return_has_invalid_type_suffix != expected_return_invalid) {
    summary.deterministic = false;
  }
}

static void AccumulateTypeAnnotationSummaryFromMethodInfo(const Objc3MethodInfo &method_info,
                                                          Objc3TypeAnnotationSurfaceSummary &summary) {
  const std::size_t arity = method_info.arity;
  if (method_info.param_has_generic_suffix.size() != arity ||
      method_info.param_has_pointer_declarator.size() != arity ||
      method_info.param_has_nullability_suffix.size() != arity ||
      method_info.param_has_ownership_qualifier.size() != arity ||
      method_info.param_id_spelling.size() != arity ||
      method_info.param_class_spelling.size() != arity ||
      method_info.param_instancetype_spelling.size() != arity ||
      method_info.param_object_pointer_type_spelling.size() != arity ||
      method_info.param_has_invalid_generic_suffix.size() != arity ||
      method_info.param_has_invalid_pointer_declarator.size() != arity ||
      method_info.param_has_invalid_nullability_suffix.size() != arity ||
      method_info.param_has_invalid_ownership_qualifier.size() != arity ||
      method_info.param_has_invalid_type_suffix.size() != arity) {
    summary.deterministic = false;
    return;
  }

  for (std::size_t i = 0; i < arity; ++i) {
    if (method_info.param_has_generic_suffix[i]) {
      ++summary.generic_suffix_sites;
    }
    if (method_info.param_has_pointer_declarator[i]) {
      ++summary.pointer_declarator_sites;
    }
    if (method_info.param_has_nullability_suffix[i]) {
      ++summary.nullability_suffix_sites;
    }
    if (method_info.param_has_ownership_qualifier[i]) {
      ++summary.ownership_qualifier_sites;
    }
    if (IsObjCReferenceAnnotationSite(method_info.param_id_spelling[i],
                                      method_info.param_class_spelling[i],
                                      method_info.param_instancetype_spelling[i],
                                      method_info.param_object_pointer_type_spelling[i])) {
      ++summary.object_pointer_type_sites;
    }
    if (method_info.param_has_invalid_generic_suffix[i]) {
      ++summary.invalid_generic_suffix_sites;
    }
    if (method_info.param_has_invalid_pointer_declarator[i]) {
      ++summary.invalid_pointer_declarator_sites;
    }
    if (method_info.param_has_invalid_nullability_suffix[i]) {
      ++summary.invalid_nullability_suffix_sites;
    }
    if (method_info.param_has_invalid_ownership_qualifier[i]) {
      ++summary.invalid_ownership_qualifier_sites;
    }
    const bool expected_invalid = method_info.param_has_invalid_generic_suffix[i] ||
                                  method_info.param_has_invalid_pointer_declarator[i] ||
                                  method_info.param_has_invalid_nullability_suffix[i] ||
                                  method_info.param_has_invalid_ownership_qualifier[i];
    if (method_info.param_has_invalid_type_suffix[i] != expected_invalid) {
      summary.deterministic = false;
    }
  }

  if (method_info.return_has_generic_suffix) {
    ++summary.generic_suffix_sites;
  }
  if (method_info.return_has_pointer_declarator) {
    ++summary.pointer_declarator_sites;
  }
  if (method_info.return_has_nullability_suffix) {
    ++summary.nullability_suffix_sites;
  }
  if (method_info.return_has_ownership_qualifier) {
    ++summary.ownership_qualifier_sites;
  }
  if (IsObjCReferenceAnnotationSite(method_info.return_id_spelling,
                                    method_info.return_class_spelling,
                                    method_info.return_instancetype_spelling,
                                    method_info.return_object_pointer_type_spelling)) {
    ++summary.object_pointer_type_sites;
  }
  if (method_info.return_has_invalid_generic_suffix) {
    ++summary.invalid_generic_suffix_sites;
  }
  if (method_info.return_has_invalid_pointer_declarator) {
    ++summary.invalid_pointer_declarator_sites;
  }
  if (method_info.return_has_invalid_nullability_suffix) {
    ++summary.invalid_nullability_suffix_sites;
  }
  if (method_info.return_has_invalid_ownership_qualifier) {
    ++summary.invalid_ownership_qualifier_sites;
  }
  const bool expected_return_invalid = method_info.return_has_invalid_generic_suffix ||
                                       method_info.return_has_invalid_pointer_declarator ||
                                       method_info.return_has_invalid_nullability_suffix ||
                                       method_info.return_has_invalid_ownership_qualifier;
  if (method_info.return_has_invalid_type_suffix != expected_return_invalid) {
    summary.deterministic = false;
  }
}

static void AccumulateTypeAnnotationSummaryFromPropertyInfo(const Objc3PropertyInfo &property_info,
                                                            Objc3TypeAnnotationSurfaceSummary &summary) {
  if (property_info.has_generic_suffix) {
    ++summary.generic_suffix_sites;
  }
  if (property_info.has_pointer_declarator) {
    ++summary.pointer_declarator_sites;
  }
  if (property_info.has_nullability_suffix) {
    ++summary.nullability_suffix_sites;
  }
  if (property_info.has_ownership_qualifier) {
    ++summary.ownership_qualifier_sites;
  }
  if (IsObjCReferenceAnnotationSite(property_info.id_spelling,
                                    property_info.class_spelling,
                                    property_info.instancetype_spelling,
                                    property_info.object_pointer_type_spelling)) {
    ++summary.object_pointer_type_sites;
  }
  if (property_info.has_invalid_generic_suffix) {
    ++summary.invalid_generic_suffix_sites;
  }
  if (property_info.has_invalid_pointer_declarator) {
    ++summary.invalid_pointer_declarator_sites;
  }
  if (property_info.has_invalid_nullability_suffix) {
    ++summary.invalid_nullability_suffix_sites;
  }
  if (property_info.has_invalid_ownership_qualifier) {
    ++summary.invalid_ownership_qualifier_sites;
  }
  const bool expected_invalid = property_info.has_invalid_generic_suffix ||
                                property_info.has_invalid_pointer_declarator ||
                                property_info.has_invalid_nullability_suffix ||
                                property_info.has_invalid_ownership_qualifier;
  if (property_info.has_invalid_type_suffix != expected_invalid) {
    summary.deterministic = false;
  }
}

static Objc3TypeAnnotationSurfaceSummary BuildTypeAnnotationSurfaceSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3TypeAnnotationSurfaceSummary summary;
  for (const auto &function_entry : surface.functions) {
    AccumulateTypeAnnotationSummaryFromFunctionInfo(function_entry.second, summary);
  }
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &method_entry : interface_entry.second.methods) {
      AccumulateTypeAnnotationSummaryFromMethodInfo(method_entry.second, summary);
    }
    for (const auto &property_entry : interface_entry.second.properties) {
      AccumulateTypeAnnotationSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &method_entry : implementation_entry.second.methods) {
      AccumulateTypeAnnotationSummaryFromMethodInfo(method_entry.second, summary);
    }
    for (const auto &property_entry : implementation_entry.second.properties) {
      AccumulateTypeAnnotationSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.invalid_generic_suffix_sites <= summary.generic_suffix_sites &&
                          summary.invalid_pointer_declarator_sites <= summary.pointer_declarator_sites &&
                          summary.invalid_nullability_suffix_sites <= summary.nullability_suffix_sites &&
                          summary.invalid_ownership_qualifier_sites <= summary.ownership_qualifier_sites &&
                          summary.invalid_type_annotation_sites() <= summary.total_type_annotation_sites();
  return summary;
}
