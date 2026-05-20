#include "artifacts/objc3_frontend_artifact_runtime_concurrency_abi_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "lower/contracts/concurrency_actor_contracts.h"
#include "lower/contracts/concurrency_continuation_runtime_contracts.h"
#include "lower/contracts/concurrency_task_runtime_helper_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeUnifiedConcurrencyRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_unified_concurrency_runtime_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencyRuntimeAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"unified_concurrency_source_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"async_task_actor_normalization_completion_surface_contract_id\":\""
           << kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId
           << "\",\"unified_concurrency_lowering_metadata_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencyLoweringMetadataSurfaceContractId
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_unified_concurrency_runtime_abi_boundary\":[\""
           << kObjc3RuntimeAllocateAsyncContinuationI32Symbol << "\",\""
           << kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol << "\",\""
           << kObjc3RuntimeResumeAsyncContinuationI32Symbol << "\",\""
           << kObjc3RuntimeCancelAsyncContinuationI32Symbol << "\",\""
           << kObjc3RuntimeSpawnTaskI32Symbol << "\",\""
           << kObjc3RuntimeEnterTaskGroupScopeI32Symbol << "\",\""
           << kObjc3RuntimeAddTaskGroupTaskI32Symbol << "\",\""
           << kObjc3RuntimeWaitTaskGroupNextI32Symbol << "\",\""
           << kObjc3RuntimeCancelTaskGroupI32Symbol << "\",\""
           << kObjc3RuntimeTaskIsCancelledI32Symbol << "\",\""
           << kObjc3RuntimeTaskOnCancelI32Symbol << "\",\""
           << kObjc3RuntimeExecutorHopI32Symbol << "\",\""
           << kObjc3RuntimeActorEnterIsolationThunkI32Symbol << "\",\""
           << kObjc3RuntimeActorEnterNonisolatedI32Symbol << "\",\""
           << kObjc3RuntimeActorHopToExecutorI32Symbol << "\",\""
           << kObjc3RuntimeActorRecordReplayProofI32Symbol << "\",\""
           << kObjc3RuntimeActorRecordRaceGuardI32Symbol << "\",\""
           << kObjc3RuntimeActorBindExecutorI32Symbol << "\",\""
           << kObjc3RuntimeActorMailboxEnqueueI32Symbol << "\",\""
           << kObjc3RuntimeActorMailboxDrainNextI32Symbol << "\",\""
           << "objc3_runtime_copy_async_continuation_state_for_testing"
           << "\",\"objc3_runtime_copy_task_runtime_state_for_testing"
           << "\",\"objc3_runtime_copy_actor_runtime_state_for_testing\"]"
           << ",\"async_continuation_state_snapshot_symbol\":\"objc3_runtime_copy_async_continuation_state_for_testing\""
           << ",\"task_runtime_state_snapshot_symbol\":\"objc3_runtime_copy_task_runtime_state_for_testing\""
           << ",\"actor_runtime_state_snapshot_symbol\":\"objc3_runtime_copy_actor_runtime_state_for_testing\""
           << ",\"runtime_abi_boundary_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyRuntimeAbiBoundaryModel
           << "\",\"continuation_runtime_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyContinuationRuntimeModel
           << "\",\"task_runtime_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyTaskRuntimeModel
           << "\",\"actor_runtime_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyActorRuntimeModel
           << "\",\"fail_closed_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyRuntimeAbiFailClosedModel
           << "\",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_abi_completion_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
