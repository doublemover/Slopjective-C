# M270 Frontend Actor-Member And Isolation-Annotation Surface Completion Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-part7-actor-member-isolation-source-closure/m270-a002-v1`

## Required outcomes

1. The compiler must admit contextual `actor class` interface declarations as a real parser/frontend capability rather than a contract-only placeholder.
2. The compiler must admit callable `__attribute__((objc_nonisolated))` on actor methods.
3. Actor methods must continue to admit existing `async` and `__attribute__((objc_executor(...)))` spellings on the same frontend path.
4. The frontend must publish a dedicated packet at `frontend.pipeline.semantic_surface.objc_part7_actor_member_and_isolation_source_closure`.
5. The emitted packet must preserve deterministic counts for actor interfaces, actor methods, actor properties, nonisolated annotations, executor annotations on actor members, async actor methods, and actor member metadata sites.
6. The happy-path proof for this issue must run through `objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`.
7. This issue must not widen the implementation claim into actor-member legality, cross-actor diagnostics, sendability enforcement, or runnable actor runtime behavior.
8. Validation evidence must land under `tmp/reports/m270/M270-A002/`.
