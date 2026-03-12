# M262-C004 ARC And Block-Interaction Lowering With Autorelease-Return Conventions Core Feature Expansion Packet

Packet: `M262-C004`
Issue: `#7202`
Milestone: `M262`
Lane: `C`

## Summary

Close the supported escaping-block plus autoreleasing-return lowering edge
inventory so common ARC patterns do not require manual ownership management.

## Dependencies

- `M262-C003`
- `M261-C004`

## Acceptance Criteria

- Expand ARC and block-interaction lowering with autorelease-return
  conventions to cover the identified edge inventory without breaking the
  frozen boundary.
- Emit one canonical lane-C boundary comment and named metadata packet for the
  supported `C004` slice.
- Add deterministic checker and fixture coverage proving:
  - direct autoreleasing-return lowering remains live
  - escaping block promotion remains live under `-fobjc-arc`
  - terminal branch cleanup does not consume sibling-branch ARC cleanup state
  - autoreleasing returns still execute required dispose/release cleanup after
    block interaction on both branch paths
- Validation evidence lands under
  `tmp/reports/m262/M262-C004/arc_block_autorelease_return_lowering_summary.json`.

## Fixtures

- `tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3`
- `tests/tooling/fixtures/native/m262_arc_block_autorelease_return_positive.objc3`

## Follow-On

- `M262-D001`
