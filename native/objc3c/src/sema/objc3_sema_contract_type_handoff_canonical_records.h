#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "sema/objc3_sema_contract_type_canonical.h"

enum class Objc3SemanticCanonicalTypeKind : std::uint8_t {
  Unknown = 0,
  Scalar = 1,
  Function = 2,
  Block = 3,
  Object = 4,
  ClassObject = 5,
  Selector = 6,
  ProtocolObject = 7,
  Instancetype = 8,
  ObjectPointer = 9,
  ForeignObject = 10,
  Vector = 11,
};

enum class Objc3SemanticCanonicalNullability : std::uint8_t {
  Unspecified = 0,
  Nullable = 1,
  Nonnull = 2,
  ImplicitlyUnwrapped = 3,
  NullResettable = 4,
  Inherited = 5,
};

enum class Objc3SemanticCanonicalOwnership : std::uint8_t {
  Unspecified = 0,
  Strong = 1,
  Copy = 2,
  Retain = 3,
  Weak = 4,
  Unowned = 5,
  UnsafeUnretained = 6,
  Assign = 7,
};

struct Objc3SemanticCanonicalType {
  ValueType value_type = ValueType::Unknown;
  Objc3SemanticCanonicalTypeKind kind =
      Objc3SemanticCanonicalTypeKind::Unknown;
  Objc3SemanticCanonicalNullability nullability =
      Objc3SemanticCanonicalNullability::Unspecified;
  Objc3SemanticCanonicalOwnership ownership =
      Objc3SemanticCanonicalOwnership::Unspecified;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool has_pointer_declarator = false;
  unsigned pointer_declarator_depth = 0;
  std::string object_pointer_type_name;
  bool has_protocol_composition = false;
  std::vector<std::string> protocol_composition_lexicographic;
  bool has_generic_suffix = false;
  std::vector<std::string> generic_arguments_source_order;
  std::vector<std::string> generic_arguments_lexicographic;
  bool has_invalid_type_suffix = false;
  bool deterministic = true;
  std::string canonical_spelling;
  std::string replay_key;
};
