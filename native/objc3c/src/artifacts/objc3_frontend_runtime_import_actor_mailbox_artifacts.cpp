#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>

#include "io/objc3_json.h"
#include "lower/contracts/concurrency_actor_contracts.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

constexpr char kObjc3ConcurrencyActorMailboxRuntimeImportContractId[] =
    "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1";
constexpr char kObjc3ConcurrencyActorMailboxRuntimeImportSurfacePath[] =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface";
constexpr char kObjc3ConcurrencyActorMailboxRuntimeImportSourceModel[] =
    "runtime-import-surface-preserves-actor-lowering-and-isolation-replay-facts-for-cross-module-runtime-link-planning";
constexpr char kObjc3ConcurrencyActorMailboxRuntimeImportFailClosedModel[] =
    "missing-or-drifted-actor-mailbox-runtime-import-packets-disable-cross-module-actor-runtime-preservation-claims";

}  // namespace

Objc3ConcurrencyActorMailboxRuntimeImportSummary
BuildConcurrencyActorMailboxRuntimeImportSummary(
    const Objc3ActorLoweringMetadataContract &actor_contract,
    const std::string &actor_lowering_replay_key,
    const std::string &actor_isolation_lowering_replay_key) {
  Objc3ConcurrencyActorMailboxRuntimeImportSummary summary;
  summary.contract_id = kObjc3ConcurrencyActorMailboxRuntimeImportContractId;
  summary.source_contract_id = kObjc3ConcurrencyActorLoweringMetadataContractId;
  summary.surface_path = kObjc3ConcurrencyActorMailboxRuntimeImportSurfacePath;
  summary.source_model = kObjc3ConcurrencyActorMailboxRuntimeImportSourceModel;
  summary.fail_closed_model =
      kObjc3ConcurrencyActorMailboxRuntimeImportFailClosedModel;
  summary.actor_lowering_replay_key = actor_lowering_replay_key;
  summary.actor_isolation_lowering_replay_key =
      actor_isolation_lowering_replay_key;
  const bool actor_sites_present = actor_contract.actor_interface_sites != 0u ||
                                   actor_contract.actor_method_sites != 0u;
  summary.actor_mailbox_runtime_ready =
      actor_sites_present &&
      IsValidObjc3ActorLoweringMetadataContract(actor_contract) &&
      !actor_lowering_replay_key.empty() &&
      !actor_isolation_lowering_replay_key.empty();
  summary.deterministic = actor_contract.deterministic;
  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << ";source_contract_id=" << summary.source_contract_id
             << ";actor_mailbox_runtime_ready="
             << (summary.actor_mailbox_runtime_ready ? "true" : "false")
             << ";deterministic=" << (summary.deterministic ? "true" : "false")
             << ";actor_lowering_replay_key=" << actor_lowering_replay_key
             << ";actor_isolation_lowering_replay_key="
             << actor_isolation_lowering_replay_key;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildConcurrencyActorMailboxRuntimeImportSummaryJson(
    const Objc3ConcurrencyActorMailboxRuntimeImportSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"actor_mailbox_runtime_ready\":"
      << (summary.actor_mailbox_runtime_ready ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"actor_lowering_replay_key\":\""
      << EscapeJsonString(summary.actor_lowering_replay_key)
      << "\",\"actor_isolation_lowering_replay_key\":\""
      << EscapeJsonString(summary.actor_isolation_lowering_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
