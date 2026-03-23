# M270 Isolation And Sendable Semantic Model Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-part7-actor-isolation-sendable-semantic-model/m270-b001-v1`

## Required outcomes

1. The semantic pipeline must publish one deterministic packet at `frontend.pipeline.semantic_surface.objc_part7_actor_isolation_and_sendable_semantic_model`.
2. The packet must consume the existing `M270-A002` actor-member/isolation source closure rather than inventing a second source boundary.
3. The packet must preserve deterministic counts for actor-member source sites together with the already-landed aggregated actor/sendability sema counters.
4. The packet must truthfully describe the current implementation as a sema/accounting boundary rather than a full actor runtime or broad cross-actor legality implementation.
5. Strict-concurrency selection/reporting must remain explicitly fail-closed in this issue.
6. The happy-path proof for this issue must run through `objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`.
7. This issue must not widen the implementation claim into dedicated actor-isolation diagnostics, full sendability enforcement, executor scheduling, or runnable actor runtime behavior.
8. Validation evidence must land under `tmp/reports/m270/M270-B001/`.
