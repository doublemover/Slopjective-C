# M276 Native Build Command Surface And Incremental Build Contract Expectations (A001)

Contract ID: `objc3c-native-build-command-surface/m276-a001-v1`

## Objective

Freeze the future native-build command taxonomy, artifact-class model,
persistent-build-tree policy, and fast-vs-full validation boundary before the
repo flips away from the current monolithic build path.

## Required implementation

1. Add a canonical expectations document for the M276-A001 build-surface
   freeze.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-A
   readiness runner:
   - `scripts/check_m276_a001_native_build_command_surface_and_incremental_build_contract.py`
   - `tests/tooling/test_check_m276_a001_native_build_command_surface_and_incremental_build_contract.py`
   - `scripts/run_m276_a001_lane_a_readiness.py`
3. Add `M276-A001` anchor text to:
   - `docs/objc3c-native/src/50-artifacts.md`
   - `docs/objc3c-native.md`
   - `README.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `scripts/build_objc3c_native.ps1`
   - `native/objc3c/CMakeLists.txt`
4. Freeze the current truthful build state:
   - `npm run build:objc3c-native` still routes to `scripts/build_objc3c_native.ps1`
   - that path still builds native binaries, archives `objc3_runtime.lib`, and
     regenerates the current frontend packet family in one invocation
5. Freeze the future command taxonomy without pretending it is implemented yet:
   - `build:objc3c-native`
   - `build:objc3c-native:contracts`
   - `build:objc3c-native:full`
   - `build:objc3c-native:reconfigure`
6. Freeze the artifact-class model:
   - native binaries/libraries under `artifacts/`
   - source-derived packets under `tmp/`
   - binary-derived packets under `tmp/`
   - closeout/integration packets under `tmp/`
7. Freeze the persistence and validation boundary:
   - later M276 issues place the persistent build tree under `tmp/`
   - CI remains semantically ephemeral even when it consumes the same command taxonomy
   - local issue work eventually defaults to the fast binary-build path only after parity proof
8. `package.json` must wire:
   - `check:objc3c:m276-a001-native-build-command-surface-contract`
   - `test:tooling:m276-a001-native-build-command-surface-contract`
   - `check:objc3c:m276-a001-lane-a-readiness`
9. The contract must explicitly hand off to `M276-A002`.

## Canonical models

- Contract model:
  `monolithic-build-remains-authoritative-until-parity-proven-incremental-command-surface-replaces-it`
- Artifact model:
  `binary-publication-under-artifacts-packet-generation-under-tmp-with-explicit-source-binary-closeout-classes`
- Persistence model:
  `persistent-local-build-tree-under-tmp-with-ephemeral-ci-semantics-and-no-delete-based-healing`
- Validation model:
  `fast-local-issue-work-full-closeout-and-ci-with-default-flip-deferred-until-parity-proof`

## Non-goals

- No incremental backend implementation yet.
- No command-surface flip yet.
- No packet-family migration yet.
- No readiness-runner migration yet.

## Evidence

- `tmp/reports/m276/M276-A001/native_build_command_surface_contract_summary.json`
