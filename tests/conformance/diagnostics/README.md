# Diagnostics Bucket

Minimum scope:

- required diagnostic groups from Part 12,
- strict and strict-system escalation behavior,
- stable diagnostic-code assertions,
- required fix-it coverage for mechanical migrations.

## Wave 0 fixture set (strictness/sub-mode matrix)

These fixtures cover strictness and concurrency sub-mode selection requirements
tracked by issues `#51` and `#52`.

- `SCM-01.json`: strict + concurrency off matrix expectations.
- `SCM-02.json`: strict + concurrency strict matrix expectations.
- `SCM-03.json`: strict-system + concurrency off matrix expectations.
- `SCM-04.json`: strict-system + concurrency strict matrix expectations.
- `SCM-05.json`: mode-selection macro consistency across matrix points.
- `SCM-06.json`: conformance-report mode/profile matrix consistency.

See `tests/conformance/diagnostics/manifest.json` for machine-readable
indexing.

## Strict nullability diagnostics fixture set

These fixtures cover strict missing-nullability diagnostics for issue `#60`:

- `NUL-60-NEG-01.json`: required strict diagnostic for missing nullability.
- `NUL-60-FIX-01.json`: required machine-applicable fix-it coverage.

## Strict lifetime/escape diagnostics fixture set (issue #69)

These fixtures cover strict lifetime-extension and unsafe-escape diagnostics:

- `LFT-69-NEG-01.json`: suspicious lifetime extension, stack-bound escape, and
  unsafe bridge diagnostics.
- `LFT-69-FIX-01.json`: machine-applicable explicit-bridge fix-it coverage.

## Strict control-flow diagnostics fixture set (issue #74)

These fixtures cover strict defer non-local-exit diagnostics:

- `NLE-74-NEG-01.json`: rejects `return` from inside `defer`.
- `NLE-74-NEG-02.json`: rejects `goto`/loop control transfers from `defer`.

## Strict throws-misuse diagnostics fixture set (issue #79)

These fixtures cover strict diagnostics for error-handling misuse:

- `ERR-79-NEG-01.json`: missing `try`, ignored throwing value, invalid
  bridge-error storage.
- `ERR-79-FIX-01.json`: machine-applicable `try` insertion fix-it.

## Async/await diagnostics fixture set (issue #80)

These fixtures cover required diagnostics and fix-its for await misuse:

- `ASY-05.json`, `ASY-06.json`: missing-await and invalid-await diagnostics
  with portable assertion metadata.

## Isolation/executor diagnostics fixture set (issue #85)

These fixtures cover strict-concurrency isolation and executor misuse:

- `CONC-DIAG-01.json`..`CONC-DIAG-06.json`: missing-await fix-its, invalid
  executor annotation/target diagnostics, suspicious cross-actor call warning,
  nonisolated actor-state access errors, and positive controls.

## Sendable-boundary diagnostics fixture set (issue #86)

These fixtures cover sendability-boundary diagnostics:

- `SND-07.json`, `SND-08.json`: non-Sendable boundary errors and
  `objc_unsafe_sendable` unchecked-transfer warnings.

## D-011 await diagnostics fixture set (issue #87)

These fixtures expand missing-await diagnostics/fix-its to all suspension
categories:

- `AWT-05.json`, `AWT-06.json`: async-call, executor-hop, and actor-isolated
  cross-domain await diagnostics with portable fix-it assertions.

## Resource cleanup diagnostics fixture set (issue #89)

These fixtures cover strict-system resource misuse diagnostics:

- `RES-06.json`: required strict-system diagnostics for resource misuse.
- `AGR-NEG-01.json`: aggregate resource annotation rejection diagnostics.

## Borrowed-pointer diagnostics fixture set (issue #91)

These fixtures cover strict-system borrowed-pointer escape diagnostics:

- `BRW-NEG-03.json`..`BRW-NEG-05.json`: required escape and invalid-borrow
  diagnostics with portable metadata.

## Capture-list diagnostics fixture set (issue #93)

These fixtures cover strict-system capture-list hazard diagnostics:

- `CAP-06.json`..`CAP-08.json`: move-state, `unowned` safety, and suspicious
  capture diagnostics.

## System hazard diagnostics fixture set (issue #94)

These fixtures expand strict-system borrowed/resource/capture diagnostics:

- `SYS-DIAG-01.json`..`SYS-DIAG-08.json`: borrowed escapes, borrowed return
  owner-index validation, resource move-state hazards, and dangerous capture
  pattern diagnostics.

## Performance/dynamism diagnostics fixture set (issue #97)

These fixtures cover strict and strict-performance misuse diagnostics:

- `PERF-DYN-01.json`..`PERF-DYN-04.json`: dynamic-dispatch misuse diagnostics
  for `objc_direct` methods and valid `objc_dynamic` opt-outs.
- `PERF-DIAG-01.json`..`PERF-DIAG-04.json`: required final/sealed/category
  collision diagnostics with stable assertion metadata.

## Macro sandbox diagnostics fixture set (issue #99)

These fixtures cover macro package/macro-expansion safety diagnostics:

- `META-MAC-01.json`..`META-MAC-04.json`: sandbox policy, determinism,
  capability-gating, and lockfile mismatch diagnostics.

## Diagnostic-group matrix fixture set (issue #104)

These fixtures cover minimum Part 12 diagnostic groups and profile severities:

- `DIAG-GRP-01.json`..`DIAG-GRP-10.json`: nullability, throws, concurrency,
  modules/interfaces, and performance-control diagnostic-group coverage.

## Migrator and fix-it fixture set (issue #105)

These fixtures cover deterministic migrator transformations:

- `MIG-01.json`..`MIG-08.json`: nullability insertion, optional-send rewrites,
  NSError-to-throws wrapper migration, and executor/actor hop fix-it coverage.

## Conformance-report schema fixture set (issue #106)

These fixtures cover report emission/validation requirements:

- `CRPT-01.json`..`CRPT-06.json`: schema-id emission, required-key checks,
  enum validity, known-deviation shape checks, and mode/profile consistency.

## M02 lane D diagnostic mapping fixture set (issues #1241-#1248)

These fixtures cover deterministic diagnostics/conformance payload-code mapping
for M02 lane D scope freeze:

- `M02-D001.json`: contract definition diagnostic payload shape baseline.
- `M02-D002.json`: driver profile/code mapping baseline.
- `M02-D003.json`: negative-path oracle expansion with deterministic payloads.
- `M02-D004.json`: mismatch/drift fail-closed mapping assertions.
- `M02-D005.json`: deterministic diagnostic rendering-order assertions.
- `M02-D006.json`: strict/strict-system failure-path payload assertions.
- `M02-D007.json`: evidence and traceability mapping stabilization checks.
- `M02-D008.json`: conformance expectation-set mapping/documentation checks.
