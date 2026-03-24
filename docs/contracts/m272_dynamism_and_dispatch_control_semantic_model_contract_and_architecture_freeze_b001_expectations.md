# M272 Dynamism And Dispatch-Control Semantic Model Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-part9-dynamism-dispatch-control-semantic-model/m272-b001-v1`

## Required behavior

- The semantic pipeline must publish one deterministic packet at `frontend.pipeline.semantic_surface.objc_part9_dynamism_and_dispatch_control_semantic_model`.
- The packet must consume the already-landed `M272-A002` Part 9 source-completion packet instead of inventing another source-only boundary.
- The packet must reuse the established override lookup/conflict accounting surface and preserve deterministic counts for override hits, misses, conflicts, and unresolved base-interface sites.
- The packet must also preserve Part 9 container/defaulting counts for `objc_direct_members`, `objc_final`, `objc_sealed`, effective direct members, defaulted direct-member sites, and `objc_dynamic` opt-out sites.

## Positive fixture proof

- `prefixed_container_attribute_sites = 1`
- `direct_members_container_sites = 1`
- `final_container_sites = 1`
- `sealed_container_sites = 1`
- `effective_direct_member_sites = 4`
- `direct_members_defaulted_method_sites = 2`
- `direct_members_dynamic_opt_out_sites = 2`
- `override_lookup_sites = 3`
- `override_lookup_hits = 1`
- `override_lookup_misses = 2`
- `override_conflicts = 0`
- `unresolved_base_interfaces = 0`

## Non-goals

- This issue does not claim direct-call lowering.
- This issue does not claim final/sealed legality enforcement.
- This issue does not claim metadata realization or runtime dispatch-boundary behavior.
