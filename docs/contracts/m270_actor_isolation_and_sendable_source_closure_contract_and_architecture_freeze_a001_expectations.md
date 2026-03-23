# M270 Actor, Isolation, And Sendable Source Closure Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-part7-actor-isolation-sendable-source-closure/m270-a001-v1`

## Required outcomes

1. The compiler must admit one truthful frontend source boundary for actor, isolation, and sendability markers without inventing new reserved `actor`, `sendable`, or `nonisolated` keywords.
2. The admitted source surface must remain the existing parser-owned symbol and attribute profiling already carried by the Part 7 async semantic packet.
3. Parser-owned profiling must remain the deterministic source contract for:
   - actor-isolation declaration markers
   - actor-hop markers
   - sendable markers
   - non-sendable crossing markers
4. The happy-path proof for this issue must run through `objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`.
5. The emitted manifest must preserve the current actor/isolation/sendability counts under `frontend.pipeline.semantic_surface.objc_part7_async_effect_and_suspension_semantic_model`.
6. This issue must not widen the implementation claim into dedicated actor grammar, live actor-member semantics, isolation diagnostics, executor-boundary enforcement, or runnable actor runtime behavior.
7. Validation evidence must land under `tmp/reports/m270/M270-A001/`.
