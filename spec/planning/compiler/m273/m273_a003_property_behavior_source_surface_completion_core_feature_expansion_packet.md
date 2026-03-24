# M273-A003 Packet: Property-Behavior Source Surface Completion - Core Feature Expansion

## Scope

- complete the remaining Part 10 lane-A frontend/source-model surface for `behavior=...` properties
- publish one deterministic packet that records source-visible synthesized declaration state already carried by parser-owned property records

## Contract

- Contract ID: `objc3c-part10-property-behavior-source-completion/m273-a003-v1`
- Frontend surface path: `frontend.pipeline.semantic_surface.objc_part10_property_behavior_and_synthesized_declaration_source_completion`

## Required proof

- the frontend runner emits the packet for a happy-path fixture
- the packet counts interface / implementation / protocol behavior-bearing properties deterministically
- the packet counts synthesized binding / getter / setter visibility sites deterministically
- the packet remains truthful to lane-A scope and does not claim later Part 10 expansion/runtime behavior
