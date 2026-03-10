# M259 A002 Canonical Runnable Sample Set Expectations

Contract ID: `objc3c-canonical-runnable-sample-set/m259-a002-v1`

Issue: `#7209`

## Objective

Implement the first integrated canonical runnable sample that exercises alloc/init,
protocol conformance, category extension, and property access on the live runtime
path.

## Required implementation

1. Add a canonical expectations document for the integrated runnable sample.
2. Add a deterministic checker, tooling tests, and a direct lane-A readiness runner:
   - `scripts/check_m259_a002_canonical_runnable_sample_set.py`
   - `tests/tooling/test_check_m259_a002_canonical_runnable_sample_set.py`
   - `scripts/run_m259_a002_lane_a_readiness.py`
3. Add the integrated proof assets:
   - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
   - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
4. The live proof must compile the sample, emit `llvm-direct` object output, link the
   dedicated probe, and prove all of the following:
   - `Widget` realizes successfully with attached category owner
     `category:Widget(Tracing)`
   - `alloc`/`init` produce a non-zero instance receiver
   - `tracedValue == 13`
   - `inheritedValue == 7`
   - `classValue == 11`
   - `shared == 19`
   - property dispatch reloads `count == 37`, `enabled == 1`,
     `currentValue == 55`, `tokenValue == 0`
   - `Widget -> Worker` conforms without an attachment owner match
   - `Widget -> Tracer` conforms through attachment owner
     `category:Widget(Tracing)`
   - property reflection reports stable slot/layout/accessor facts for `count`,
     `value`, and `token`
5. Add `M259-A002` anchor text to:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `scripts/check_objc3c_native_execution_smoke.ps1`
   - `scripts/check_objc3c_execution_replay_proof.ps1`
   - `package.json`
6. `package.json` must wire:
   - `check:objc3c:m259-a002-canonical-runnable-sample-set`
   - `test:tooling:m259-a002-canonical-runnable-sample-set`
   - `check:objc3c:m259-a002-lane-a-readiness`
7. The implementation must explicitly hand off to `M259-B001`.

## Canonical models

- Evidence model:
  `a001-freeze-plus-live-integrated-runnable-object-property-category-protocol-sample`
- Sample set model:
  `integrated-runnable-sample-set-unifies-alloc-init-protocol-category-and-property-behavior`
- Failure model:
  `fail-closed-on-integrated-runnable-sample-drift-or-missing-live-proof`

## Non-goals

- No blocks, ARC, async, throws, actors, or import/module expansion here.
- No widening of execution smoke/replay into this sample set yet.
- No claim that cross-module source-bound method bodies are complete.

## Evidence

- `tmp/reports/m259/M259-A002/canonical_runnable_sample_set_summary.json`
