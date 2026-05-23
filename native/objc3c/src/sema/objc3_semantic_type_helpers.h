#pragma once

#include "sema/model/generic_collection_type_model.h"
#include "sema/objc3_sema_contract.h"

#include <string>
#include <unordered_map>
#include <vector>

enum class SemanticOwnershipKind {
  None,
  Retained,
  Weak,
  Unowned,
};

struct SemanticTypeInfo {
  ValueType type = ValueType::Unknown;
  Objc3SemanticCanonicalType canonical_type;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool is_callable = false;
  bool is_generic_collection = false;
  Objc3GenericCollectionTypeModel generic_collection_model;
  std::vector<ValueType> callable_param_types;
  ValueType callable_return_type = ValueType::Unknown;
  bool callable_block_runtime_handle_candidate = false;
  bool callable_block_runtime_handle_has_byref_capture = false;
  bool callable_block_runtime_handle_has_owned_object_capture = false;
  std::string callable_block_runtime_first_byref_capture_name;
  std::string callable_block_runtime_first_owned_capture_name;
  SemanticOwnershipKind ownership_kind = SemanticOwnershipKind::None;
  bool has_nullability_suffix = false;
  bool is_refined_nonnull_reference = false;
  bool is_mutable_binding = false;
  std::string object_pointer_type_name;
};

using SemanticScope = std::unordered_map<std::string, SemanticTypeInfo>;

struct OwnershipResourceMoveBindingState {
  bool cleanup_owned = false;
  bool moved = false;
  unsigned move_line = 1;
  unsigned move_column = 1;
};

using OwnershipResourceMoveScope =
    std::unordered_map<std::string, OwnershipResourceMoveBindingState>;
