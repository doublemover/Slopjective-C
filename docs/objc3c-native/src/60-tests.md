<!-- markdownlint-disable-file MD041 -->

## Live Validation Commands

From repo root:

```powershell
npm run objc3c -- test-smoke
npm run objc3c -- test-default
npm run objc3c -- test-behavior-matrix
npm run objc3c -- test-recovery
npm run objc3c -- test-execution-smoke
npm run objc3c -- test-execution-replay
npm run objc3c -- test-runtime-acceptance
npm run objc3c -- test-full
npm run objc3c -- test-nightly
npm run objc3c -- test-ci
npm run objc3c -- validate-runtime-architecture
npm run objc3c -- proof-objc3c
npm run objc3c -- check-task-hygiene
npm run objc3c -- check-dependency-boundaries
```

Targeted entrypoints accept bounded selectors when you need signal without the full corpus:

```powershell
npm run objc3c -- test-execution-smoke -Limit 12
npm run objc3c -- test-execution-smoke -Limit 24
npm run objc3c -- test-fixture-matrix -ShardIndex 0 -ShardCount 4
npm run objc3c -- test-negative-expectations -FixtureGlob "tests/tooling/fixtures/native/recovery/negative/negative_assignment_*"
npm run objc3c -- test-execution-replay -CaseId synthesized-accessor
```

Composite runner entrypoints also write one integrated report to `tmp/reports/objc3c-public-workflow/<action>.json`, with the exact child-suite summary paths captured from the live smoke, runtime-acceptance, replay, recovery, and matrix scripts.

## Gate Shape

- default gate: `npm run objc3c -- test-default`
- smoke gate: `npm run objc3c -- test-smoke`
- full gate: `npm run objc3c -- test-full`
- release gate: `npm run objc3c -- test-nightly`

## What The Live Test Surface Covers

- `test-default`: public default entrypoint; runs `test-smoke`
- `test-smoke`: behavior-first parser/sema/lowering/IR/runtime/e2e matrix, runtime acceptance, and canonical replay/native-truth proof
- `test-behavior-matrix`: direct behavior fixture execution from `tests/native`
- `test-execution-smoke`: full runnable execution smoke corpus
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
- `validate-runtime-architecture`: full public workflow plus runtime architecture proof-packet alignment
- dependency-boundary enforcement
- compact task-hygiene enforcement
- runtime dispatch over realized classes/categories/protocols
- synthesized property accessor execution over realized instance storage
- runtime-backed storage ownership reflection over emitted property descriptors
- native-output provenance through real compile and probe paths

## Runtime Dispatch Acceptance Gates

- strict dispatch is claimable only when every admitted send returns typed
  success for nil receiver, resolved live method, resolved builtin, or resolved
  property accessor behavior through live status evidence
- runtime dispatch errors are claimable only when tests cover unknown selector,
  unknown receiver class, missing class graph, rejected return shape,
  rejected argument layout, malformed metadata, and category conflict cases
- linked strict dispatch status probes, including
  `tests/tooling/runtime/strict_dispatch_error_status_probe.cpp`, are required
  evidence for the dispatch gate; manifest or source inventory alone is not
  enough to claim strict dispatch completion
- synthesized getter/setter execution is runtime-backed on live paths
- native-output truth requires the emitted object and linked probe to stay coupled end to end
