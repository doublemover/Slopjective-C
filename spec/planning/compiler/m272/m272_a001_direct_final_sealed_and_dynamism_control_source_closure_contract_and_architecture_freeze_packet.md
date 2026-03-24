# M272-A001 Packet: Direct, Final, Sealed, and Dynamism-Control Source Closure - Contract and Architecture Freeze

## Goal

Freeze the Part 9 frontend/source-model contract for dispatch-intent and dynamism-control attributes.

## Scope

This issue is parser/frontend only.
It does not claim:
- legality enforcement,
- direct-call lowering,
- metadata realization,
- runtime dispatch-boundary behavior.

## Required packet

The emitted frontend manifest must publish:
- `frontend.pipeline.semantic_surface.objc_part9_dispatch_intent_and_dynamism_source_closure`

## Positive fixture proof

The positive fixture must prove deterministic publication for:
- `direct_callable_sites = 5`
- `final_callable_sites = 3`
- `dynamic_callable_sites = 3`
- `direct_members_container_sites = 1`
- `final_container_sites = 1`
- `sealed_container_sites = 2`
- `actor_container_sites = 1`
