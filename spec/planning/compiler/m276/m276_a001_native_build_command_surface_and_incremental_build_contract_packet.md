# M276-A001 Native Build Command Surface And Incremental Build Contract Packet

Packet: `M276-A001`

Issue: `#7386`

## Objective

Freeze the future native-build command taxonomy and policy boundary before any
incremental backend becomes the default local build path.

## Dependencies

- None

## Contract

- contract id
  `objc3c-native-build-command-surface/m276-a001-v1`
- contract model
  `monolithic-build-remains-authoritative-until-parity-proven-incremental-command-surface-replaces-it`
- artifact model
  `binary-publication-under-artifacts-packet-generation-under-tmp-with-explicit-source-binary-closeout-classes`
- persistence model
  `persistent-local-build-tree-under-tmp-with-ephemeral-ci-semantics-and-no-delete-based-healing`
- validation model
  `fast-local-issue-work-full-closeout-and-ci-with-default-flip-deferred-until-parity-proof`

## Required anchors

- `docs/contracts/m276_native_build_command_surface_and_incremental_build_contract_a001_expectations.md`
- `scripts/check_m276_a001_native_build_command_surface_and_incremental_build_contract.py`
- `tests/tooling/test_check_m276_a001_native_build_command_surface_and_incremental_build_contract.py`
- `scripts/run_m276_a001_lane_a_readiness.py`
- `check:objc3c:m276-a001-native-build-command-surface-contract`
- `check:objc3c:m276-a001-lane-a-readiness`

## Frozen truthful current state

- `package.json` routes `build:objc3c-native` to `scripts/build_objc3c_native.ps1`
- the authoritative current path still:
  - builds `objc3c-native`
  - builds `objc3c-frontend-c-api-runner`
  - archives `artifacts/lib/objc3_runtime.lib`
  - regenerates the current frontend packet family in the same invocation

## Frozen future command taxonomy

- `build:objc3c-native`
  - eventual fast local binary-build default after parity proof
- `build:objc3c-native:contracts`
  - reserved contracts and packet-generation path
- `build:objc3c-native:full`
  - reserved closeout and CI full-build path
- `build:objc3c-native:reconfigure`
  - reserved fingerprint refresh / self-healing configure path

## Handoff

`M276-A002` is the explicit next handoff after this freeze closes.
