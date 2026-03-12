# M265-B003 - Generic erasure and key-path legality completion

## Goal

Close the remaining Part 3 lane-B legality gaps that still sat between the `M265-B001` semantic packet and the later lowering/runtime milestones.

## Required behavior

- keep pragmatic generic-erasure semantic counts visible and deterministic on the existing positive corpus
- diagnose reserved generic Objective-C method syntax `- <T> ...` in v1 mode
- admit class-root key paths such as `@keypath(Person, name)` when the component names a readable property on the root type
- reject class-root key paths when the component is missing from the root type
- reject typed key-path roots that are not `self`, a known class type, or an ObjC-reference-compatible identifier
- reject multi-component typed key-path member chains until later executable typed key-path lowering work lands

## Acceptance notes

- This issue is still lane-B semantic legality work. It does not claim executable typed key-path runtime behavior.
- The happy path stays source-only in this issue.
- Validation must land under `tmp/reports/m265/M265-B003/`.
