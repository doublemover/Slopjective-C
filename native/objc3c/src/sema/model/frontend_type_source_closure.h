#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "contracts/objc3_language_evolution_reserved_diagnostic_codes.h"
#include "sema/objc3_sema_contract.h"
#include "token/objc3_token_contract.h"

inline constexpr const char *kObjc3TypeSystemTypeSourceClosureContractId =
    "objc3c.type_system.type.source.closure.v1";
inline constexpr const char *kObjc3TypeSystemTypeSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_type_system_type_source_closure";
inline constexpr const char *kObjc3TypeSystemTypeSourceClosureSourceModel =
    "protocol-optional-partitions-object-pointer-nullability-generic-suffixes-optional-bindings-optional-sends-optional-member-access-nil-coalescing-and-typed-keypaths-are-live-parser-owned-source-surfaces-while-Optional-value-optionals-remain-reserved";
inline constexpr const char *kObjc3TypeSystemTypeSourceClosureFailureModel =
    "value-optionals-remain-parse-owned-fail-closed-until-ABI-interface-roundtrip-and-lowering-support-land";
inline constexpr const char *kObjc3TypeSystemTypeSemanticModelContractId =
    "objc3c.type_system.type.semantic.model.v1";
inline constexpr const char *kObjc3TypeSystemTypeSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_type_system_type_semantic_model";
inline constexpr const char *kObjc3TypeSystemTypeSemanticModelCoreRule =
    "optional-bindings-optional-sends-erased-generic-metadata-and-typed-keypath-shape-obey-one-fail-closed-sema-model-before-lowering";
inline constexpr const char *kObjc3TypeSystemGenericContractPreservationContractId =
    "objc3c.type_system.generic.contract.preservation.v1";
inline constexpr const char *kObjc3TypeSystemNullabilityContractPreservationContractId =
    "objc3c.type_system.nullability.contract.preservation.v1";
inline constexpr const char *kObjc3TypeSystemProtocolContractPreservationContractId =
    "objc3c.type_system.protocol.contract.preservation.v1";

struct Objc3FrontendTypeSystemTypeSourceClosureSummary {
  std::string contract_id = kObjc3TypeSystemTypeSourceClosureContractId;
  std::string frontend_surface_path = kObjc3TypeSystemTypeSourceClosureSurfacePath;
  std::string source_model = kObjc3TypeSystemTypeSourceClosureSourceModel;
  std::string failure_model = kObjc3TypeSystemTypeSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimProtocolOptionalPartitions,
      kObjc3SourceOnlyFeatureClaimObjectPointerNullabilitySuffixes,
      kObjc3SourceOnlyFeatureClaimPragmaticGenericSuffixes,
      kObjc3SourceOnlyFeatureClaimOptionalBindings,
      kObjc3SourceOnlyFeatureClaimOptionalSends,
      kObjc3SourceOnlyFeatureClaimNilCoalescing,
      kObjc3SourceOnlyFeatureClaimTypedKeyPathLiterals,
  };
  std::vector<std::string> unsupported_claim_ids = {
      kObjc3UnsupportedFeatureClaimValueOptionals,
  };
  std::vector<std::string> fail_closed_construct_ids = {
      kObjc3TypeSystemFailClosedConstructValueOptionals,
  };
  std::size_t protocol_required_method_count = 0;
  std::size_t protocol_optional_method_count = 0;
  std::size_t protocol_required_property_count = 0;
  std::size_t protocol_optional_property_count = 0;
  std::size_t object_pointer_type_spelling_sites = 0;
  std::size_t pointer_declarator_entries = 0;
  std::size_t nullability_suffix_entries = 0;
  std::size_t generic_suffix_entries = 0;
  std::size_t optional_binding_sites = 0;
  std::size_t guard_binding_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t optional_member_access_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  bool protocol_optional_partition_source_supported = false;
  bool object_pointer_nullability_source_supported = false;
  bool pragmatic_generic_suffix_source_supported = false;
  bool optional_binding_source_supported = false;
  bool optional_send_source_supported = false;
  bool nil_coalescing_source_supported = false;
  bool typed_keypath_literal_source_supported = false;
  bool optional_member_access_fail_closed = false;
  bool value_optional_type_fail_closed = false;
  std::size_t value_optional_issue_ref = 8234;
  std::string value_optional_canonical_spelling = "Optional<T>";
  std::string value_optional_reserved_diagnostic_code =
      kObjc3ParserDiagnosticReservedValueOptionalCode;
  std::string lowercase_optional_alias_diagnostic_code =
      kObjc3ParserDiagnosticRemovedOptionalAliasCode;
  bool lowercase_optional_alias_rejected = true;
  bool value_optional_nil_to_scalar_coercion_allowed = false;
  bool value_optional_nullable_pointer_conversion_allowed = false;
  bool value_optional_throws_conversion_allowed = false;
  std::string value_optional_abi_status = "reserved-no-layout";
  bool nil_coalescing_fail_closed = false;
  bool typed_keypath_literal_fail_closed = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};
