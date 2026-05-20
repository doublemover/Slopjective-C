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
void ClearRuntimeProtocolCategoryDiagnosticFieldsUnlocked(RuntimeState &state);
void RecordRuntimeProtocolCategoryDiagnosticFieldsUnlocked(
    RuntimeState &state,
    const std::string &diagnostic_reason);
bool RuntimeProtocolCategoryMetadataTableIsSupported(
    const RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    std::string &diagnostic_reason);

}  // namespace objc3c::runtime
