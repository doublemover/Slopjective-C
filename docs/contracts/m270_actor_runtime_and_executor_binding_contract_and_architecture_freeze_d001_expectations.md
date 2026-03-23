# M270 actor runtime and executor binding contract expectations

Contract ID: `objc3c-part7-actor-runtime-and-executor-binding/m270-d001-v1`

This issue is complete when:

- the private actor helper cluster and `objc3_runtime_actor_runtime_state_snapshot` are frozen as the lane-D actor runtime proof surface
- mixed-module runtime link planning continues to require one shared packaged runtime archive path for actor helper traffic
- `tests/tooling/runtime/m270_d001_actor_runtime_executor_contract_probe.cpp` proves deterministic actor-state and executor-binding snapshot publication
- the next issue remains `M270-D002`
