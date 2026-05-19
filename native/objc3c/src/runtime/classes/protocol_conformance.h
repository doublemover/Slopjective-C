#pragma once

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
    ProtocolConformanceMatch &match);
bool RuntimeProtocolConformanceEdgeIsMaterializable(const char *class_name,
                                                    const char *protocol_name);

}  // namespace objc3c::runtime
