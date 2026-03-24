# M272 Frontend Attribute and Defaulting Surface Completion Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-part9-dispatch-intent-source-completion/m272-a002-v1`

## Required behavior

- The compiler must admit prefixed container `__attribute__((...))` lists before `@interface` declarations for the supported Part 9 dispatch-intent attributes.
- The compiler must admit prefixed container `__attribute__((...))` lists before `actor class` declarations for the supported Part 9 dispatch-intent attributes.
- The emitted frontend manifest must publish `frontend.pipeline.semantic_surface.objc_part9_dispatch_intent_attribute_and_defaulting_source_completion`.
- The emitted frontend packet must preserve effective direct-member defaulting and explicit `objc_dynamic` opt-out sites under `objc_direct_members`.

## Positive fixture proof

- `prefixed_container_attribute_sites = 1`
- `direct_members_container_sites = 1`
- `final_container_sites = 1`
- `sealed_container_sites = 1`
- `effective_direct_member_sites = 4`
- `direct_members_defaulted_method_sites = 2`
- `direct_members_dynamic_opt_out_sites = 2`

## Non-goals

- This issue does not claim Part 9 legality enforcement.
- This issue does not claim direct-call lowering.
- This issue does not claim metadata or runtime dispatch-boundary realization.
