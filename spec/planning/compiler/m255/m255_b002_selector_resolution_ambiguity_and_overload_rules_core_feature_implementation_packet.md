# M255-B002 Selector Resolution, Ambiguity, And Overload Rules Core Feature Implementation Packet

Packet: `M255-B002`
Milestone: `M255`
Lane: `B`
Dependencies: `M255-B001`, `M255-A002`
Contract ID: `objc3c-selector-resolution-ambiguity/m255-b002-v1`

## Summary

Implement live lane-B selector resolution for concrete `self`, `super`, and
known-class message sends. Missing concrete selectors must fail closed with
`O3S216`. Incompatible concrete declarations must fail closed with `O3S217`.
Non-concrete receivers remain on the runtime-dynamic path.

## Acceptance criteria

- Concrete `self`, `super`, and known-class message sends are resolved in sema.
- Non-concrete receivers continue to compile as dynamic runtime dispatch sites.
- The happy path compiles through `artifacts/bin/objc3c-native.exe`.
- Negative fixtures prove `O3S216` and `O3S217`.
- Code/spec anchors remain explicit and deterministic.
- Evidence lands at `tmp/reports/m255/M255-B002/selector_resolution_ambiguity_summary.json`.
