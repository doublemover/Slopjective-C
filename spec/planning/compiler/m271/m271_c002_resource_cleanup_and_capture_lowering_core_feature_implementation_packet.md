# M271-C002 Packet: Resource Cleanup And Capture Lowering - Core Feature Implementation

## Objective

Turn the frozen `M271-C001` Part 8 lowering contract into real native lowering for the supported stack/local cleanup-resource path.

## Implementation Requirements

1. Keep `M271-C001` as the only Part 8 lowering manifest surface.
2. Implement real cleanup/resource lowering through emitted lexical cleanup calls and block dispose helpers.
3. Allow supported stack/local `move` capture lowering for cleanup-backed locals.
4. Keep actual escaping promotion of move-based cleanup/resource captures fail-closed until later runtime ownership-transfer work lands.
5. Add explicit issue-local checker, readiness runner, pytest, expectations doc, positive fixture, and negative fail-closed fixture.

## Positive Proof

- A positive fixture must compile through `objc3c-native.exe`.
- The output must include `module.ll`, `module.obj`, `module.manifest.json`, and `module.runtime-metadata.bin`.
- The emitted IR must show:
  - `ReleaseTemp` and `CloseFd` declarations
  - emitted block copy/dispose helpers
  - cleanup-hook calls inside the dispose helper
  - resource-close calls on the lexical cleanup path

## Negative Proof

- A negative fixture that forces block-handle promotion of a move-based cleanup/resource capture must fail with the existing `O3L300` ownership-transfer diagnostic.

## Truth Constraints

- No second Part 8 lowering packet.
- No claim of escaping move-capture ownership transfer support.
- No claim of Part 8 runtime closeout work that belongs to later lane-D issues.
