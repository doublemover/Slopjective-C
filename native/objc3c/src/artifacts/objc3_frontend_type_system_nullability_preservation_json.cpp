#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"
#include "sema/objc3_sema_contract_core.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildTypeSystemNullabilityContractPreservationReplayKey(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  std::ostringstream out;
  out << kObjc3FrontendTypeSystemNullabilityContractPreservationContractId
      << ";source_contract="
      << kObjc3FrontendTypeSystemTypeSemanticModelContractId
      << ";type_semantic_replay=" << summary.replay_key
      << ";canonical_types=" << summary.canonical_type_entries
      << ";object_types=" << summary.canonical_object_type_entries
      << ";nullable=" << summary.canonical_nullable_entries
      << ";nonnull=" << summary.canonical_nonnull_entries
      << ";iuo=" << summary.canonical_implicitly_unwrapped_entries
      << ";null_resettable=" << summary.canonical_null_resettable_entries
      << ";unspecified=" << summary.canonical_unspecified_nullability_entries
      << ";invalid=" << summary.canonical_invalid_type_entries;
  return out.str();
}

}  // namespace

std::string BuildTypeSystemNullabilityContractPreservationJson(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  const std::string replay_key =
      BuildTypeSystemNullabilityContractPreservationReplayKey(summary);
  const bool nullability_count_consistent =
      summary.canonical_type_entries ==
      summary.canonical_nullable_entries + summary.canonical_nonnull_entries +
          summary.canonical_implicitly_unwrapped_entries +
          summary.canonical_null_resettable_entries +
          summary.canonical_unspecified_nullability_entries;
  const bool ready = summary.ready_for_lowering_and_runtime &&
                     summary.deterministic &&
                     nullability_count_consistent &&
                     !summary.replay_key.empty();
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(
             kObjc3FrontendTypeSystemNullabilityContractPreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(kObjc3FrontendTypeSystemTypeSemanticModelContractId)
      << "\",\"preservation_model\":\"runtime-import-surface-preserves-canonical-nullability-counts-and-type-semantic-replay-boundary\""
      << ",\"canonical_type_count\":" << summary.canonical_type_entries
      << ",\"object_type_count\":" << summary.canonical_object_type_entries
      << ",\"nullable_entry_count\":" << summary.canonical_nullable_entries
      << ",\"nonnull_entry_count\":" << summary.canonical_nonnull_entries
      << ",\"implicitly_unwrapped_entry_count\":"
      << summary.canonical_implicitly_unwrapped_entries
      << ",\"null_resettable_entry_count\":"
      << summary.canonical_null_resettable_entries
      << ",\"unspecified_nullability_entry_count\":"
      << summary.canonical_unspecified_nullability_entries
      << ",\"invalid_nullability_entry_count\":"
      << summary.canonical_invalid_type_entries
      << ",\"ready\":" << (ready ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"type_semantic_replay_key\":\""
      << EscapeJsonString(summary.replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
