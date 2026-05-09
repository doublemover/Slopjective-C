#pragma once

#include "artifacts/objc3_frontend_artifacts.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"

namespace objc3::artifacts::frontend {

bool CompleteObjc3FrontendArtifactIREmission(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeDispatchLoweringAbiContract
        &runtime_dispatch_lowering_abi_contract,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract);

}  // namespace objc3::artifacts::frontend
