# M259-A002 Canonical Runnable Sample Set Packet

Packet: `M259-A002`

Issue: `#7209`

Milestone: `M259`

Lane: `A`

## Objective

Create the first integrated canonical runnable sample that exercises the current
truthful object-model core on one live runtime path.

## Dependencies

- `M259-A001`

## Acceptance

- Add one canonical runnable sample fixture and one runtime probe.
- The probe must prove alloc/init, superclass dispatch, category dispatch,
  protocol conformance, and property access on the same emitted module.
- Keep docs/spec/package plus smoke/replay anchors explicit and deterministic.
- Publish deterministic evidence at
  `tmp/reports/m259/M259-A002/canonical_runnable_sample_set_summary.json`.
- Next issue: `M259-B001`.

## Canonical proof assets

- `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
- `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
- `tmp/reports/m259/M259-A001/runnable_sample_surface_contract_summary.json`

## Truthful boundary

- The canonical integrated sample is single-module and probe-backed.
- It proves runtime-backed alloc/init, protocol/category behavior, and property
  dispatch together without claiming blocks, ARC, or cross-module method-body
  completion.
- Execution smoke and replay scripts remain scalar/core corpus gates; they are
  only annotated here so the canonical sample set stays documented and
  discoverable.
