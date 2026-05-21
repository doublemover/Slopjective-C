#pragma once

#include "runtime/state/runtime_bootstrap_contracts.h"

#include <cstdint>
#include <string>

namespace objc3c::runtime {

struct RealizedClassNode;
struct RuntimeState;

struct ProtocolConformanceMatch {
  std::string matched_protocol_owner_identity;
  std::string matched_attachment_owner_identity;
  std::string matched_class_name;
  std::string matched_class_owner_identity;
  std::uint64_t matched_protocol_depth = 0;
  bool matched_from_category = false;
  bool matched_from_superclass = false;
  bool matched_via_inherited_protocol = false;
};

inline constexpr const char *kObjc3ProtocolExistentialWitnessMetadataKey =
    "protocol-witness-conformance-metadata";
inline constexpr const char *kObjc3ProtocolExistentialRuntimeLookupAnchor =
    "QueryRealizedClassProtocolConformanceUnlocked";
inline constexpr const char
    *kObjc3ProtocolExistentialAssociatedTypeDiagnosticCode = "O3P100";
inline constexpr const char
    *kObjc3ProtocolExistentialDynamicDispatchDiagnosticCode = "O3S314";

struct ProtocolExistentialWitnessMetadata {
  std::string existential_canonical_spelling;
  std::string object_representation;
  std::string conforming_type_canonical_spelling;
  std::string conforming_type_owner_identity;
  std::string protocol_name;
  std::string protocol_owner_identity;
  std::string conformance_owner_identity;
  std::string attachment_owner_identity;
  std::string runtime_lookup_anchor;
  std::string witness_metadata_key;
  std::string requirement_resolution_policy;
  std::string unsupported_associated_type_diagnostic;
  std::string unsupported_dynamic_dispatch_diagnostic;
  std::uint64_t matched_protocol_depth = 0;
  bool matched_from_category = false;
  bool matched_from_superclass = false;
  bool matched_via_inherited_protocol = false;
  bool conformance_edge_materializable = false;
  bool associated_types_supported = false;
  bool dynamic_existential_dispatch_supported = false;
  bool fail_closed_for_unsupported_semantics = true;
};

bool ProtocolExistsByNameUnlocked(const RuntimeState &state,
                                  const char *protocol_name);
bool QueryRealizedClassProtocolConformanceUnlocked(
    RuntimeState &state,
    const RealizedClassNode *start_node,
    const char *protocol_name,
    std::uint64_t &visited_protocol_count,
    ProtocolConformanceMatch &match,
    std::string &failure_reason);
bool RuntimeProtocolConformanceEdgeIsMaterializable(const char *class_name,
                                                    const char *protocol_name);
bool BuildRuntimeProtocolExistentialWitnessMetadata(
    const char *class_name,
    const char *protocol_name,
    const ProtocolConformanceMatch &match,
    ProtocolExistentialWitnessMetadata &metadata,
    std::string &failure_reason);
bool RuntimeProtocolExistentialWitnessMetadataIsSupported(
    const ProtocolExistentialWitnessMetadata &metadata);
void ClearRuntimeProtocolCategoryDiagnosticFieldsUnlocked(RuntimeState &state);
void RecordRuntimeProtocolCategoryDiagnosticFieldsUnlocked(
    RuntimeState &state,
    const std::string &diagnostic_reason);
bool RuntimeProtocolCategoryMetadataTableIsSupported(
    const RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    std::string &diagnostic_reason);

}  // namespace objc3c::runtime
