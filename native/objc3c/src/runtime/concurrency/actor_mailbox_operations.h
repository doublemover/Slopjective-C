#pragma once

namespace objc3c::runtime {

struct RuntimeActorState;

int BindRuntimeActorMailboxExecutor(RuntimeActorState &state,
                                    int actor_handle,
                                    int executor_tag);
int EnqueueRuntimeActorMailboxValue(RuntimeActorState &state,
                                    int actor_handle,
                                    int value,
                                    int executor_tag);
int DrainRuntimeActorMailboxNextValue(RuntimeActorState &state,
                                      int actor_handle,
                                      int executor_tag);
int CancelRuntimeActorMailbox(RuntimeActorState &state,
                              int actor_handle,
                              int executor_tag);
int RecordRuntimeActorMailboxError(RuntimeActorState &state,
                                   int actor_handle,
                                   int error_code,
                                   int executor_tag);
int ShutdownRuntimeActorMailbox(RuntimeActorState &state,
                                int actor_handle,
                                int executor_tag);

}  // namespace objc3c::runtime
