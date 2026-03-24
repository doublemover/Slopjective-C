# M272 Override, Finality, And Sealing Legality Enforcement Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-part9-override-finality-sealing-legality/m272-b002-v1`

## Required behavior

- The semantic pipeline must publish one deterministic packet at `frontend.pipeline.semantic_surface.objc_part9_override_finality_and_sealing_legality`.
- The packet must consume the already-landed `M272-B001` semantic-model packet instead of introducing a separate legality accounting surface.
- The compiler must fail closed on:
  - inheriting from an `objc_final` superclass,
  - inheriting from an `objc_sealed` superclass,
  - overriding an `objc_final` superclass method,
  - participating in an override chain that uses `objc_direct` dispatch.

## Positive fixture proof

- `subclass_sites = 1`
- `override_sites = 1`
- `illegal_final_superclass_sites = 0`
- `illegal_sealed_superclass_sites = 0`
- `illegal_final_override_sites = 0`
- `illegal_direct_override_sites = 0`

## Negative fixture proof

- final-superclass rejection uses `O3S307`
- sealed-superclass rejection uses `O3S308`
- final-method override rejection uses `O3S309`
- direct-override rejection uses `O3S310`

## Non-goals

- This issue does not claim direct-call lowering.
- This issue does not claim metadata realization.
- This issue does not claim runnable dispatch-boundary behavior.
