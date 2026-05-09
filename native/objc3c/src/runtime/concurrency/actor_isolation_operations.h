#pragma once

namespace objc3c::runtime {

struct RuntimeActorState;

int EnterRuntimeActorIsolationThunk(RuntimeActorState &state,
                                    int executor_tag);
int EnterRuntimeActorNonisolated(RuntimeActorState &state,
                                 int value,
                                 int executor_tag);
int HopRuntimeActorToExecutor(RuntimeActorState &state,
                              int value,
                              int executor_tag);

}  // namespace objc3c::runtime
