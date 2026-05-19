#pragma once

namespace objc3c::runtime {

struct RuntimeActorState;

int RecordRuntimeActorReplayProof(RuntimeActorState &state,
                                  int executor_tag);
int RecordRuntimeActorRaceGuard(RuntimeActorState &state,
                                int executor_tag);

}  // namespace objc3c::runtime
