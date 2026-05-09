#pragma once

#include <cstdint>
#include <string>

namespace objc3c::runtime {

struct RealizedClassNode;
struct RuntimeState;

bool ProtocolExistsByNameUnlocked(const RuntimeState &state,
                                  const char *protocol_name);
bool QueryRealizedClassProtocolConformanceUnlocked(
    RuntimeState &state,
    const RealizedClassNode *start_node,
    const char *protocol_name,
    std::uint64_t &visited_protocol_count,
    std::string &matched_protocol_owner_identity,
    std::string &matched_attachment_owner_identity);
bool RuntimeProtocolConformanceEdgeIsMaterializable(const char *class_name,
                                                    const char *protocol_name);

}  // namespace objc3c::runtime
