# M272 Compatibility Diagnostics For Dynamism Controls Edge-Case And Compatibility Completion Expectations (B003)

Contract ID: `objc3c-part9-dynamism-control-compatibility-diagnostics/m272-b003-v1`

## Required behavior

- The semantic pipeline must publish one deterministic packet at `frontend.pipeline.semantic_surface.objc_part9_dynamism_control_compatibility_diagnostics`.
- The packet must consume the already-landed `M272-B002` legality packet instead of introducing a separate compatibility accounting surface.
- The compiler must fail closed on:
  - combining `objc_direct` with `objc_dynamic`,
  - combining `objc_final` with `objc_dynamic`,
  - using Part 9 dispatch-control callable attributes on free functions,
  - using Part 9 dispatch-control callable attributes on protocol methods,
  - using Part 9 dispatch-control callable attributes on category methods,
  - using `objc_direct_members`, `objc_final`, or `objc_sealed` on categories.

## Positive fixture proof

- `callable_dispatch_intent_sites = 4`
- `container_dispatch_intent_sites = 1`
- `illegal_direct_dynamic_conflict_sites = 0`
- `illegal_final_dynamic_conflict_sites = 0`
- `illegal_non_method_callable_sites = 0`
- `illegal_protocol_method_sites = 0`
- `illegal_category_method_sites = 0`
- `illegal_category_container_sites = 0`

## Negative fixture proof

- direct/dynamic conflict rejection uses `O3S311`
- final/dynamic conflict rejection uses `O3S312`
- free-function dispatch-intent rejection uses `O3S313`
- protocol dispatch-intent rejection uses `O3S314`
- category-method dispatch-intent rejection uses `O3S315`
- category-container dispatch-intent rejection uses `O3S316`

## Non-goals

- This issue does not claim direct-call lowering.
- This issue does not claim metadata realization.
- This issue does not claim runnable dispatch-boundary behavior.
