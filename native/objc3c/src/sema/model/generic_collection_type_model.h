#pragma once

#include <string>
#include <vector>

#include "sema/objc3_sema_contract_type_handoff_canonical_records.h"

enum class Objc3GenericCollectionKind {
  None,
  Array,
  MutableArray,
  Slice,
  Map,
  MutableMap,
  Set,
  MutableSet,
  Iterator,
  MapIterator,
};

enum class Objc3GenericCollectionVariance {
  Invariant,
};

enum class Objc3GenericCollectionMutability {
  Immutable,
  Mutable,
  ImmutableView,
};

struct Objc3GenericCollectionTypeModel {
  Objc3GenericCollectionKind kind = Objc3GenericCollectionKind::None;
  Objc3GenericCollectionVariance variance =
      Objc3GenericCollectionVariance::Invariant;
  Objc3GenericCollectionMutability mutability =
      Objc3GenericCollectionMutability::Immutable;
  std::string public_spelling;
  std::vector<std::string> type_arguments_source_order;
  std::string element_type_identity;
  std::string key_type_identity;
  std::string value_type_identity;
  std::string iterator_element_type_identity;
  std::string runtime_descriptor_identity;
  std::string abi_identity;
  std::string diagnostic_code;
  std::string diagnostic_reason;
  bool value_semantics_are_handle_backed = true;
  bool ownership_is_runtime_descriptor_bound = true;
  bool supports_nested_generic_arguments = false;
  bool descriptor_identity_required = true;
  bool deterministic = false;
};
