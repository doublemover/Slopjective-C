# M262-A001 ARC Source Surface And Mode Boundary Contract And Architecture Freeze Packet

Packet: `M262-A001`

Issue: `#7194`

## Objective

Freeze the truthful ARC-adjacent frontend and mode boundary before ARC
automation work begins.

## Dependencies

- None

## Contract

- contract id
  `objc3c-arc-source-mode-boundary-freeze/m262-a001-v1`
- source model
  `ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode`
- mode model
  `native-driver-rejects-fobjc-arc-while-executable-ownership-qualified-functions-and-methods-stay-fail-closed`
- failure model
  `fail-closed-on-arc-source-mode-boundary-drift-before-arc-automation`
- non-goal model
  `no-fobjc-arc-cli-mode-no-fno-objc-arc-cli-mode-no-automatic-arc-cleanup-insertion-no-user-visible-arc-runtime-mode-split`

## Required anchors

- `docs/contracts/m262_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze_a001_expectations.md`
- `scripts/check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py`
- `tests/tooling/test_check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py`
- `scripts/run_m262_a001_lane_a_readiness.py`
- `check:objc3c:m262-a001-arc-source-surface-mode-boundary-contract`
- `check:objc3c:m262-a001-lane-a-readiness`

## Canonical freeze surface

- ownership qualifiers remain parser/sema-visible
- weak/unowned summaries remain live
- `@autoreleasepool` remains parser/sema-profiled
- ARC fix-it summaries remain live
- executable function/method ownership qualifiers still fail with `O3S221`
- the native driver still rejects `-fobjc-arc`
- `Objc3ArcSourceModeBoundarySummary()`

## Live proof fixtures

- `tests/tooling/fixtures/native/hello.objc3`
- `tests/tooling/fixtures/native/m260_runtime_backed_object_ownership_attribute_surface_positive.objc3`
- `tests/tooling/fixtures/native/m259_b002_unsupported_feature_claim_arc_ownership_qualifier.objc3`

## Handoff

`M262-A002` is the explicit next handoff after this freeze closes.
