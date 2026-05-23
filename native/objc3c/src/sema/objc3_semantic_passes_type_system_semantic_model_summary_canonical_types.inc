static void AccumulateCanonicalTypeSemanticModelSummary(
    const Objc3SemanticCanonicalType &type,
    Objc3TypeSystemTypeSemanticModelSummary &summary) {
  ++summary.canonical_type_entries;
  if (type.kind == Objc3SemanticCanonicalTypeKind::Object ||
      type.kind == Objc3SemanticCanonicalTypeKind::ClassObject ||
      type.kind == Objc3SemanticCanonicalTypeKind::ProtocolObject ||
      type.kind == Objc3SemanticCanonicalTypeKind::Instancetype ||
      type.kind == Objc3SemanticCanonicalTypeKind::ObjectPointer ||
      type.kind == Objc3SemanticCanonicalTypeKind::ForeignObject) {
    ++summary.canonical_object_type_entries;
  }
  if (type.has_protocol_composition) {
    ++summary.canonical_protocol_qualified_entries;
  }
  summary.canonical_generic_argument_entries +=
      type.generic_arguments_lexicographic.size();
  if (type.is_value_optional) {
    ++summary.canonical_value_optional_entries;
  }
  const Objc3GenericCollectionTypeModel collection_model =
      BuildObjc3GenericCollectionTypeModel(type);
  if (IsObjc3GenericCollectionKind(collection_model.kind)) {
    ++summary.generic_collection_type_entries;
    if (!collection_model.runtime_descriptor_identity.empty()) {
      ++summary.generic_collection_descriptor_entries;
    }
    if (collection_model.kind == Objc3GenericCollectionKind::Iterator ||
        collection_model.kind == Objc3GenericCollectionKind::MapIterator) {
      ++summary.generic_collection_iterator_entries;
    }
    if (collection_model.mutability ==
        Objc3GenericCollectionMutability::Mutable) {
      ++summary.generic_collection_mutable_entries;
    }
    if (collection_model.variance ==
        Objc3GenericCollectionVariance::Invariant) {
      ++summary.generic_collection_invariant_entries;
    }
    if (!IsReadyObjc3GenericCollectionTypeModel(collection_model)) {
      ++summary.generic_collection_invalid_entries;
    }
  }
  switch (type.nullability) {
    case Objc3SemanticCanonicalNullability::Nullable:
      ++summary.canonical_nullable_entries;
      break;
    case Objc3SemanticCanonicalNullability::Nonnull:
      ++summary.canonical_nonnull_entries;
      break;
    case Objc3SemanticCanonicalNullability::ImplicitlyUnwrapped:
      ++summary.canonical_implicitly_unwrapped_entries;
      break;
    case Objc3SemanticCanonicalNullability::NullResettable:
      ++summary.canonical_null_resettable_entries;
      break;
    case Objc3SemanticCanonicalNullability::Inherited:
    case Objc3SemanticCanonicalNullability::Unspecified:
      ++summary.canonical_unspecified_nullability_entries;
      break;
  }
  if (type.has_invalid_type_suffix || !type.deterministic) {
    ++summary.canonical_invalid_type_entries;
  }
}

static void AccumulateCanonicalTypeSemanticModelSummaryFromSurface(
    const Objc3SemanticIntegrationSurface &surface,
    Objc3TypeSystemTypeSemanticModelSummary &summary) {
  for (const auto &function_entry : surface.functions) {
    const FunctionInfo &function = function_entry.second;
    for (const auto &param_type : function.param_canonical_types) {
      AccumulateCanonicalTypeSemanticModelSummary(param_type, summary);
    }
    AccumulateCanonicalTypeSemanticModelSummary(function.return_canonical_type,
                                               summary);
  }
  const auto accumulate_container =
      [&summary](const auto &container_map) {
        for (const auto &container_entry : container_map) {
          for (const auto &property_entry : container_entry.second.properties) {
            AccumulateCanonicalTypeSemanticModelSummary(
                property_entry.second.canonical_type, summary);
          }
          for (const auto &method_entry : container_entry.second.methods) {
            const Objc3MethodInfo &method = method_entry.second;
            for (const auto &param_type : method.param_canonical_types) {
              AccumulateCanonicalTypeSemanticModelSummary(param_type, summary);
            }
            AccumulateCanonicalTypeSemanticModelSummary(
                method.return_canonical_type, summary);
          }
        }
      };
  accumulate_container(surface.interfaces);
  accumulate_container(surface.implementations);
  accumulate_container(surface.category_interfaces);
  accumulate_container(surface.category_implementations);
}
