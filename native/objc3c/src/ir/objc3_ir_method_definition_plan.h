#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "ir/objc3_ir_value_optional_carrier_model.h"
#include "runtime/metadata/runtime_metadata_typed_bundles.h"

struct Objc3IRFrontendMetadata;
struct Objc3MethodDecl;
struct Objc3Program;

enum class Objc3IRSyntheticMethodKind {
  None,
  PropertyGetter,
  PropertySetter,
  MetaprogrammingDerivedEquality,
  MetaprogrammingDerivedHash,
  MetaprogrammingDerivedDebugDescription,
};

struct Objc3IRMethodDefinition {
  std::string symbol;
  std::string implementation_name;
  std::string method_owner_identity;
  std::string superclass_name;
  const Objc3MethodDecl *method = nullptr;
  Objc3IRSyntheticMethodKind synthetic_method_kind =
      Objc3IRSyntheticMethodKind::None;
  ValueType synthesized_value_type = ValueType::Unknown;
  std::size_t synthesized_parameter_count = 0;
  std::string synthesized_ownership_lifetime_profile;
  std::string synthesized_ownership_runtime_hook_profile;
  std::string synthesized_accessor_ownership_profile;
};

struct Objc3IRMetaprogrammingGlobalArtifact {
  std::string symbol;
  std::string payload;
};

struct Objc3IRDirectDispatchSignature {
  ValueType return_type = ValueType::I32;
  std::vector<ValueType> param_types;
  Objc3IRValueOptionalCarrierMetadata return_value_optional_carrier;
  std::vector<Objc3IRValueOptionalCarrierMetadata>
      param_value_optional_carriers;
  bool throws_declared = false;
  bool typed_throws_declared = false;
  bool throws_error_out_abi_ready = false;
  std::string typed_throws_error_type_spelling;
  bool has_value_optional_type_signature = false;
  bool value_optional_lowering_supported = false;
  std::string value_optional_payload_type_spelling;
};

struct Objc3IRMethodDefinitionPlan {
  std::vector<Objc3IRMethodDefinition> method_definitions;
  std::unordered_map<std::string, std::string> direct_dispatch_symbols_by_key;
  std::unordered_map<std::string, Objc3IRDirectDispatchSignature>
      direct_dispatch_signatures_by_key;
  std::unordered_map<std::string, ValueType> runtime_dispatch_return_types_by_key;
  std::unordered_map<std::string, Objc3IRValueOptionalCarrierMetadata>
      runtime_dispatch_return_value_optional_carriers_by_key;
  std::unordered_map<std::string, std::string> runtime_dispatch_superclass_by_name;
  std::vector<Objc3IRMetaprogrammingGlobalArtifact>
      metaprogramming_global_artifacts;
  std::size_t synthesized_property_accessor_count = 0;
  std::size_t metaprogramming_derived_method_count = 0;
  std::string error;
};

bool Objc3IRRuntimeMetadataPropertyBundleIsImplementationOwned(
    const Objc3IRRuntimeMetadataPropertyBundle &bundle);
Objc3IRMethodDefinitionPlan BuildObjc3IRMethodDefinitionPlan(
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &frontend_metadata);
