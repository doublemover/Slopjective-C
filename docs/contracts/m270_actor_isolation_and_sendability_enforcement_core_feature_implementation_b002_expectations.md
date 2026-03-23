# M270-B002 Expectations

- Contract id: `objc3c-part7-actor-isolation-sendability-enforcement/m270-b002-v1`
- Dependency: `objc3c-part7-actor-isolation-sendable-semantic-model/m270-b001-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part7_actor_isolation_and_sendability_enforcement`
- Live sema must fail closed for:
  - non-actor `objc_nonisolated`
  - `objc_nonisolated` actor methods combined with `async`
  - `objc_nonisolated` actor methods combined with `objc_executor(...)`
  - actor hop sites in non-async actor methods
  - non-sendable crossings in actor methods
- Positive fixture must compile through the frontend runner and emit a deterministic packet.
