#include "sema/objc3_semantic_type_factory.h"

#include <algorithm>
#include <utility>

#include "sema/objc3_semantic_generic_collection_type_model.h"
#include "support/objc3_string_join.h"
#include "support/objc3_type_profile_helpers.h"
#include "support/objc3_value_type_names.h"

SemanticTypeInfo MakeScalarSemanticType(ValueType type) {
  SemanticTypeInfo info;
  info.type = type;
  info.canonical_type.value_type = type;
  info.canonical_type.kind =
      type == ValueType::Optional
          ? Objc3SemanticCanonicalTypeKind::ValueOptional
          : (objc3c::support::IsObjCReferenceAliasValueType(type)
                 ? Objc3SemanticCanonicalTypeKind::Object
                 : Objc3SemanticCanonicalTypeKind::Scalar);
  info.canonical_type.canonical_spelling = objc3c::support::ValueTypeName(type);
  if (objc3c::support::IsObjCReferenceAliasValueType(type)) {
    info.ownership_kind = SemanticOwnershipKind::Retained;
  }
  return info;
}

SemanticTypeInfo MakeVectorSemanticType(
    ValueType base_type,
    const std::string &base_spelling,
    unsigned lane_count) {
  SemanticTypeInfo info;
  info.type = base_type;
  info.canonical_type.value_type = base_type;
  info.canonical_type.kind = Objc3SemanticCanonicalTypeKind::Vector;
  info.canonical_type.is_vector = true;
  info.canonical_type.vector_base_spelling = base_spelling;
  info.canonical_type.vector_lane_count = lane_count;
  info.canonical_type.canonical_spelling =
      "vector<" + base_spelling + "," + std::to_string(lane_count) + ">";
  info.is_vector = true;
  info.vector_base_spelling = base_spelling;
  info.vector_lane_count = lane_count;
  return info;
}

SemanticTypeInfo MakeCallableSemanticType(
    std::vector<ValueType> param_types,
    ValueType return_type) {
  SemanticTypeInfo info;
  info.type = ValueType::Function;
  info.canonical_type.value_type = ValueType::Function;
  info.canonical_type.kind = Objc3SemanticCanonicalTypeKind::Block;
  info.canonical_type.canonical_spelling = "block";
  info.canonical_type.replay_key = "value=Function;kind=block";
  info.is_callable = true;
  info.callable_param_types = std::move(param_types);
  info.callable_return_type = return_type;
  return info;
}

SemanticTypeInfo MakeCallableSemanticTypeFromBlockLiteral(const Expr &expr) {
  std::vector<ValueType> param_types = expr.block_parameter_types_source_order;
  if (param_types.size() < expr.block_parameter_count) {
    param_types.resize(expr.block_parameter_count, ValueType::Unknown);
  }
  SemanticTypeInfo info =
      MakeCallableSemanticType(std::move(param_types), ValueType::Unknown);
  info.callable_block_runtime_handle_candidate =
      expr.block_escape_shape_promotes_to_heap_candidate;
  info.callable_block_runtime_handle_has_byref_capture =
      expr.block_byref_capture_count > 0u;
  info.callable_block_runtime_handle_has_owned_object_capture =
      expr.block_runtime_owned_object_capture_count > 0u;
  if (!expr.block_byref_capture_names_lexicographic.empty()) {
    info.callable_block_runtime_first_byref_capture_name =
        expr.block_byref_capture_names_lexicographic.front();
  }
  if (!expr.block_runtime_owned_object_capture_names_lexicographic.empty()) {
    info.callable_block_runtime_first_owned_capture_name =
        expr.block_runtime_owned_object_capture_names_lexicographic.front();
  }
  return info;
}

SemanticTypeInfo MakeSemanticTypeFromGlobal(ValueType type) {
  return MakeScalarSemanticType(type);
}

SemanticTypeInfo MakeSemanticTypeFromCanonicalType(
    const Objc3SemanticCanonicalType &canonical_type) {
  if (canonical_type.is_objc_named_object_pointer &&
      canonical_type.has_generic_suffix &&
      IsObjc3GenericCollectionKind(Objc3GenericCollectionKindFromSpelling(
          canonical_type.object_pointer_type_name))) {
    SemanticTypeInfo info = MakeGenericCollectionSemanticType(
        canonical_type.object_pointer_type_name,
        canonical_type.generic_arguments_source_order);
    info.canonical_type.nullability = canonical_type.nullability;
    info.canonical_type.has_explicit_nullability =
        canonical_type.has_explicit_nullability;
    info.has_nullability_suffix = canonical_type.has_explicit_nullability;
    return info;
  }
  if (canonical_type.is_vector) {
    return MakeVectorSemanticType(canonical_type.value_type,
                                  canonical_type.vector_base_spelling,
                                  canonical_type.vector_lane_count);
  }
  if (canonical_type.is_value_optional) {
    SemanticTypeInfo info = MakeScalarSemanticType(ValueType::Optional);
    info.canonical_type = canonical_type;
    return info;
  }
  SemanticTypeInfo info = MakeScalarSemanticType(canonical_type.value_type);
  info.canonical_type = canonical_type;
  if (!canonical_type.object_pointer_type_name.empty()) {
    info.object_pointer_type_name = canonical_type.object_pointer_type_name;
  }
  if (canonical_type.is_objc_object_reference) {
    info.ownership_kind = SemanticOwnershipKind::Retained;
  }
  info.has_nullability_suffix = canonical_type.has_explicit_nullability;
  return info;
}

SemanticTypeInfo MakeGenericCollectionSemanticType(
    const std::string &type_name,
    const std::vector<std::string> &arguments_source_order) {
  SemanticTypeInfo info;
  info.type = ValueType::ObjCObjectPtr;
  info.canonical_type.value_type = ValueType::ObjCObjectPtr;
  info.canonical_type.kind = Objc3SemanticCanonicalTypeKind::ObjectPointer;
  info.canonical_type.object_pointer_type_name = type_name;
  info.canonical_type.has_generic_suffix = true;
  info.canonical_type.generic_arguments_source_order = arguments_source_order;
  info.canonical_type.generic_arguments_lexicographic = arguments_source_order;
  std::sort(info.canonical_type.generic_arguments_lexicographic.begin(),
            info.canonical_type.generic_arguments_lexicographic.end());
  info.canonical_type.object_type_facts_authoritative = true;
  info.canonical_type.is_objc_object_reference = true;
  info.canonical_type.is_objc_named_object_pointer = true;
  info.canonical_type.generic_arguments_facts_authoritative = true;
  info.canonical_type.nullability_facts_authoritative = true;
  info.canonical_type.protocol_composition_facts_authoritative = true;
  info.canonical_type.canonical_spelling =
      type_name + "<" +
      objc3c::support::JoinStringVector(arguments_source_order, ",") + ">";
  info.generic_collection_model =
      BuildObjc3GenericCollectionTypeModel(type_name, arguments_source_order);
  info.is_generic_collection =
      IsObjc3GenericCollectionKind(info.generic_collection_model.kind);
  info.object_pointer_type_name = type_name;
  info.ownership_kind = SemanticOwnershipKind::Retained;
  info.canonical_type.has_invalid_generic_suffix =
      !IsReadyObjc3GenericCollectionTypeModel(info.generic_collection_model);
  info.canonical_type.has_invalid_type_suffix =
      info.canonical_type.has_invalid_generic_suffix;
  info.canonical_type.deterministic = true;
  return info;
}
