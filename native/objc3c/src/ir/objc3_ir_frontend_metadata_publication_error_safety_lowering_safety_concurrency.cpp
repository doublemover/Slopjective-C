#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_concurrency.h"

#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_actor_sendability.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_concurrency_race_guard.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_governance.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_task_interop.h"

void EmitObjc3IRSafetyConcurrencyLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRSafetyGovernanceLoweringCounterNodes(metadata, out);
  EmitObjc3IRConcurrencyRaceGuardLoweringCounterNode(metadata, out);
  EmitObjc3IRTaskInteropCancellationLoweringCounterNode(metadata, out);
  EmitObjc3IRActorSendabilityLoweringCounterNode(metadata, out);
}
