# M255-B003 Super, Direct, And Dynamic Legality Plus Method-Family Runtime Rules Core Feature Expansion Packet

Issue: `#7118`
Milestone: `M255`
Lane: `B`
Contract ID: `objc3c-super-dynamic-method-family/m255-b003-v1`
Dependencies: `M255-B002`

## Scope

Close the remaining selector legality/runtime-rule edges after live concrete
selector resolution landed in `M255-B002`.

This issue is specifically about:

- illegal `super` use failing closed
- preserving the reserved/non-goal direct-dispatch boundary
- ensuring dynamic receiver sites remain runtime-dispatched
- keeping runtime-visible method-family accounting aligned across sema,
  manifest surfaces, lowering contracts, and the runtime-shim host-link packet

## Acceptance criteria

- Expand super, direct, and dynamic legality plus method-family runtime rules to
  cover the identified edge inventory without breaking the frozen boundary.
- Extend evidence coverage so regressions fail closed.
- Code/spec anchors remain explicit and deterministic.
- Validation evidence lands under `tmp/` with a stable path for the issue.

## Proof inventory

### Positive

`tests/tooling/fixtures/native/m255_super_dynamic_method_family_edges.objc3`

Expected semantic-surface evidence:

- `objc_dispatch_surface_classification_surface.super_dispatch_sites = 4`
- `objc_dispatch_surface_classification_surface.dynamic_dispatch_sites = 3`
- `objc_dispatch_surface_classification_surface.direct_dispatch_sites = 0`
- `objc_super_dispatch_method_family_surface.message_send_sites = 7`
- `objc_super_dispatch_method_family_surface.receiver_super_identifier_sites = 4`
- `objc_super_dispatch_method_family_surface.method_family_init_sites = 1`
- `objc_super_dispatch_method_family_surface.method_family_copy_sites = 2`
- `objc_super_dispatch_method_family_surface.method_family_mutable_copy_sites = 1`
- `objc_super_dispatch_method_family_surface.method_family_new_sites = 2`
- `objc_super_dispatch_method_family_surface.method_family_none_sites = 1`
- `objc_runtime_shim_host_link_surface.runtime_shim_required_sites = 7`
- object backend remains `llvm-direct`

### Negative

`tests/tooling/fixtures/native/m255_super_outside_method.objc3`
- must fail with `O3S216`

`tests/tooling/fixtures/native/m255_super_root_dispatch.objc3`
- must fail with `O3S216`

## Explicit boundary decisions

- direct dispatch remains reserved/non-goal
- no new syntax is introduced for direct dispatch
- no runtime-side selector lookup behavior changes here
- no second lowering pass is allowed to "fix up" illegal super sites later
