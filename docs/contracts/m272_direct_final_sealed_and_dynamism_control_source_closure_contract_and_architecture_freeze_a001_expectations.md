# M272 Direct, Final, Sealed, and Dynamism-Control Source Closure Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-part9-dispatch-intent-source-closure/m272-a001-v1`

## Required behavior

- The compiler must admit callable `__attribute__((objc_direct))`.
- The compiler must admit callable `__attribute__((objc_final))`.
- The compiler must admit callable `__attribute__((objc_dynamic))`.
- The compiler must admit container `__attribute__((objc_direct_members))`.
- The compiler must admit container `__attribute__((objc_final))`.
- The compiler must admit container `__attribute__((objc_sealed))`.
- The emitted frontend manifest must publish `frontend.pipeline.semantic_surface.objc_part9_dispatch_intent_and_dynamism_source_closure`.

## Positive fixture proof

- `direct_callable_sites = 5`
- `final_callable_sites = 3`
- `dynamic_callable_sites = 3`
- `direct_members_container_sites = 1`
- `final_container_sites = 1`
- `sealed_container_sites = 2`
- `actor_container_sites = 1`

## Non-goals

- This issue does not claim Part 9 legality enforcement.
- This issue does not claim direct-call lowering or selector-dispatch divergence.
- This issue does not claim metadata or runtime dispatch-boundary realization.
