# M259-C002 Object And IR Replay-Proof Plus Metadata Inspection Evidence Packet

Packet: `M259-C002`

Issue: `#7213`

Milestone: `M259`

Lane: `C`

## Objective

Implement the full replay-proof and binary-inspection evidence path for the
canonical runnable object-model sample.

## Dependencies

- `M259-C001`
- `M258-E002`

## Acceptance

- Implement object and IR replay-proof plus metadata inspection evidence as a
  real compiler/runtime capability rather than a manifest-only summary.
- Add deterministic checker coverage over the live happy path.
- Keep code/spec anchors explicit and deterministic.
- Publish deterministic evidence at
  `tmp/reports/m259/M259-C002/object_and_ir_replay_proof_plus_metadata_inspection_summary.json`.
- Next issue: `M259-D001`.

## Truthful boundary

- This issue widens the frozen C001 boundary into a live proof path.
- The canonical runnable sample remains
  `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`.
- The proof compares replay-stable hashes for `module.ll`, `module.obj`, and
  `llvm-readobj --sections` output across two runs.
- Archive/static-link, multi-module, multi-image, and broader binary-inspection
  expansion remain later work.
