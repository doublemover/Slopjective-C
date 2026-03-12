# M262-C003 Cleanup Emission Weak Load-Store Lowering And Lifetime Extension Hooks Core Feature Implementation Packet

Packet: `M262-C003`
Issue: `#7201`
Milestone: `M262`
Lane: `C`

## Summary

Promote the supported ARC lowering slice from helper insertion into real
scope-exit cleanup emission, weak current-property runtime-hook continuity, and
block-capture lifetime cleanup for the runnable subset.

## Dependencies

- `M262-A002`
- `M262-B003`
- `M262-C001`
- `M262-C002`

## Acceptance Criteria

- Implement cleanup emission, weak load-store lowering, and lifetime extension
  hooks as a real compiler/runtime capability rather than a manifest-only
  summary.
- Emit one canonical lane-C boundary comment and named metadata packet for the
  supported `C003` slice.
- Add deterministic checker and fixture coverage proving:
  - weak current-property helper continuity
  - scope-exit dispose-helper cleanup before merge
  - implicit-exit ARC cleanup before `ret void`
- Validation evidence lands under
  `tmp/reports/m262/M262-C003/arc_cleanup_weak_lifetime_hooks_summary.json`.

## Fixtures

- `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
- `tests/tooling/fixtures/native/m262_arc_cleanup_scope_positive.objc3`
- `tests/tooling/fixtures/native/m262_arc_implicit_cleanup_void_positive.objc3`

## Follow-On

- `M262-C004`
