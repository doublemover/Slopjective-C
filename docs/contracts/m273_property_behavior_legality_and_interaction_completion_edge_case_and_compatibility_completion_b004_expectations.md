# M273 Property-Behavior Legality and Interaction Completion Edge-case and Compatibility Completion Expectations (B004)

Contract ID: `objc3c-part10-property-behavior-legality-interaction-completion/m273-b004-v1`

## Required outcomes

- the frontend manifest emits `frontend.pipeline.semantic_surface.objc_part10_property_behavior_legality_and_interaction_completion`
- supported property behaviors are limited to `Observed` and `Projected`
- unsupported property behavior names fail closed with `O3S326`
- behavior-bearing properties must remain Objective-C object properties or fail closed with `O3S327`
- `Observed` behavior on protocol properties fails closed with `O3S328`
- `Observed` behavior on readonly or setter-invisible properties fails closed with `O3S329`
- `Projected` behavior on writable or setter-visible properties fails closed with `O3S330`
- property-behavior runtime hooks remain deferred in this tranche
