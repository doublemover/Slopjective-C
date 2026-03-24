# M273-B004 Packet: Property-Behavior Legality and Interaction Completion - Edge-case and Compatibility Completion

- Issue: `M273-B004`
- Milestone: `M273`
- Lane: `B`
- Contract ID: `objc3c-part10-property-behavior-legality-interaction-completion/m273-b004-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part10_property_behavior_legality_and_interaction_completion`

## Summary

This packet closes the remaining semantic legality gaps for `behavior=...` properties.

## Live rules

- supported behaviors: `Observed`, `Projected`
- unsupported behavior names fail with `O3S326`
- behavior-bearing properties must be Objective-C object properties or fail with `O3S327`
- `Observed` protocol properties fail with `O3S328`
- `Observed` readonly or setter-invisible properties fail with `O3S329`
- `Projected` writable or setter-visible properties fail with `O3S330`

## Deferred boundary

- no runnable property-behavior hook installation is claimed here
- no lowering or runtime materialization is claimed here
