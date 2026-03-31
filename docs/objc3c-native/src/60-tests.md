<!-- markdownlint-disable-file MD041 -->

## Live Validation Commands

From repo root:

```powershell
npm run test:fast
npm run test:objc3c
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:runtime-acceptance
npm run test:objc3c:full
npm run test:objc3c:nightly
npm run test:ci
npm run test:objc3c:runtime-architecture
npm run proof:objc3c
npm run check:task-hygiene
npm run check:objc3c:boundaries
```

Targeted entrypoints accept bounded selectors when you need signal without the full corpus:

```powershell
npm run test:objc3c:execution-smoke -- -Limit 12
npm run test:objc3c -- -Limit 24
npm run test:objc3c:fixture-matrix -- -ShardIndex 0 -ShardCount 4
npm run test:objc3c:negative-expectations -- -FixtureGlob "tests/tooling/fixtures/native/recovery/negative/negative_assignment_*"
npm run test:objc3c:execution-replay-proof -- -CaseId synthesized-accessor
```

Composite runner entrypoints also write one integrated report to `tmp/reports/objc3c-public-workflow/<action>.json`, with the exact child-suite summary paths captured from the live smoke, runtime-acceptance, replay, recovery, and matrix scripts.

## What The Live Test Surface Covers

- `test-fast`: bounded execution-smoke slice, runtime acceptance, and canonical replay/native-truth proof
- `test-smoke`: full runnable execution smoke corpus
- `test-recovery`: recovery compile success and deterministic diagnostics replay as a non-default heavy path
- `test-full`: smoke, runtime acceptance, and replay/native-truth proof without the recovery fan-out
- `test-nightly`: full validation plus recovery, positive fixture-matrix, and static negative-expectation sweeps
- authoritative guarantee owners:
  - `test-execution-smoke`: compile/link/run execution behavior
  - `test-recovery`: recovery compile success and deterministic diagnostics replay
  - `test-execution-replay`: replay and native-output truth
  - `test-runtime-acceptance`: runtime acceptance and ABI/accessor proof
  - `test-negative-expectations`: negative expectation header and token enforcement
  - `test-fixture-matrix`: broad positive dispatch and artifact sanity
- `test:objc3c:runtime-architecture`: full public workflow plus runtime architecture proof-packet alignment
- dependency-boundary enforcement
- compact task-hygiene enforcement
- runtime dispatch over realized classes/categories/protocols
- synthesized property accessor execution over realized instance storage
- runtime-backed storage ownership reflection over emitted property descriptors
- native-output provenance through real compile and probe paths

## Current Corrective Gaps Under Test

- unresolved dispatch still has one deterministic fallback path after slow-path miss
- synthesized accessor IR still carries transitional lowering residue even though live getter/setter execution is already runtime-backed
- native-output truth requires the emitted object and linked probe to stay coupled end to end
