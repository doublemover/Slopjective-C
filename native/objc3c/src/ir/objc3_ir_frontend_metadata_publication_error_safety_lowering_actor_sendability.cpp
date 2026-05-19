#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_actor_sendability.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRActorSendabilityLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!41 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_sendability_check_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_cross_actor_hop_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_non_sendable_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_sendable_transfer_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_isolation_boundary_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_actor_isolation_sendability_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
