#include "sema/objc3_semantic_type_factory.h"

#include <utility>

#include "support/objc3_type_profile_helpers.h"
#include "support/objc3_value_type_names.h"

SemanticTypeInfo MakeScalarSemanticType(ValueType type) {
  SemanticTypeInfo info;
  info.type = type;
  info.canonical_type.value_type = type;
  info.canonical_type.kind =
      objc3c::support::IsObjCReferenceAliasValueType(type)
          ? Objc3SemanticCanonicalTypeKind::Object
          : Objc3SemanticCanonicalTypeKind::Scalar;
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
