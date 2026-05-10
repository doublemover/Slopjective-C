#include "pipeline/frontend_source_closure_replay_keys.h"

#include <sstream>

namespace objc3c::pipeline::orchestration {

std::string BuildOwnershipSystemExtensionSourceClosureReplayKey(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";resource_sites=" << summary.resource_attribute_sites << ":"
      << summary.resource_close_clause_sites << ":"
      << summary.resource_invalid_clause_sites
      << ";borrowed_sites=" << summary.borrowed_pointer_sites
      << ";returns_borrowed_sites=" << summary.returns_borrowed_attribute_sites
      << ";capture_sites=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites << ":"
      << summary.explicit_capture_weak_sites << ":"
      << summary.explicit_capture_unowned_sites << ":"
      << summary.explicit_capture_move_sites << ":"
      << summary.explicit_capture_plain_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildOwnershipCleanupResourceCaptureSourceCompletionReplayKey(
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";cleanup_sites=" << summary.cleanup_attribute_sites << ":"
      << summary.cleanup_sugar_sites
      << ";resource_sites=" << summary.resource_attribute_sites << ":"
      << summary.resource_sugar_sites << ":"
      << summary.resource_close_clause_sites << ":"
      << summary.resource_invalid_clause_sites
      << ";capture_sites=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites << ":"
      << summary.explicit_capture_weak_sites << ":"
      << summary.explicit_capture_unowned_sites << ":"
      << summary.explicit_capture_move_sites << ":"
      << summary.explicit_capture_plain_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildOwnershipRetainableCFamilySourceCompletionReplayKey(
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";family_sites=" << summary.family_retain_sites << ":"
      << summary.family_release_sites << ":" << summary.family_autorelease_sites
      << ";compat_sites=" << summary.compatibility_returns_retained_sites << ":"
      << summary.compatibility_returns_not_retained_sites << ":"
      << summary.compatibility_consumed_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildDispatchDispatchIntentSourceClosureReplayKey(
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";callable_sites=" << summary.direct_callable_sites << ":"
      << summary.final_callable_sites << ":" << summary.dynamic_callable_sites
      << ";container_sites=" << summary.direct_members_container_sites << ":"
      << summary.final_container_sites << ":" << summary.sealed_container_sites
      << ":" << summary.actor_container_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildDispatchDispatchIntentSourceCompletionReplayKey(
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";prefixed_container_sites="
      << summary.prefixed_container_attribute_sites
      << ";container_sites=" << summary.direct_members_container_sites << ":"
      << summary.final_container_sites << ":" << summary.sealed_container_sites
      << ";defaulting_sites=" << summary.effective_direct_member_sites << ":"
      << summary.direct_members_defaulted_method_sites << ":"
      << summary.direct_members_dynamic_opt_out_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

}  // namespace objc3c::pipeline::orchestration
