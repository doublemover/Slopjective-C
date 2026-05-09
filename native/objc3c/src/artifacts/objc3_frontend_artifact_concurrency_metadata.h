#pragma once

#include <string>

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/concurrency_actor_contracts.h"
#include "lower/contracts/concurrency_continuation_runtime_contracts.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendConcurrencyMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &async_continuation_lowering_replay_key,
    const Objc3AsyncContinuationLoweringContract
        &async_continuation_lowering_contract,
    const std::string &await_lowering_suspension_state_lowering_replay_key,
    const Objc3AwaitLoweringSuspensionStateLoweringContract
        &await_lowering_suspension_state_lowering_contract,
    const std::string &actor_isolation_sendability_lowering_replay_key,
    const Objc3ActorIsolationSendabilityLoweringContract
        &actor_isolation_sendability_lowering_contract,
    const std::string &actor_lowering_metadata_replay_key,
    const Objc3ActorLoweringMetadataContract &actor_lowering_metadata_contract);

}  // namespace objc3::artifacts::frontend
