# M262-A002 ARC Mode Handling For Methods, Properties, Returns, And Block Captures Core Feature Implementation Packet

Packet: `M262-A002`

Issue: `#7195`

## Objective

Turn the frozen ARC-adjacent mode boundary into a real explicit ARC-mode
surface for methods, properties, returns, and block captures.

## Dependencies

- `M262-A001`

## Contract

- contract id
  `objc3c-arc-mode-handling/m262-a002-v1`
- source model
  `ownership-qualified-method-property-return-and-block-capture-surfaces-are-runnable-under-explicit-arc-mode`
- mode model
  `driver-admits-fobjc-arc-and-fno-objc-arc-and-threads-arc-mode-through-frontend-sema-and-ir`
- failure model
  `non-arc-mode-still-rejects-executable-ownership-qualified-method-and-function-signatures`
- non-goal model
  `no-generalized-arc-cleanup-synthesis-no-implicit-nonarc-promotion-no-full-arc-automation-yet`

## Required anchors

- `docs/contracts/m262_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation_a002_expectations.md`
- `scripts/check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py`
- `tests/tooling/test_check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py`
- `scripts/run_m262_a002_lane_a_readiness.py`
- `check:objc3c:m262-a002-arc-mode-handling-methods-properties-returns-block-captures-contract`
- `check:objc3c:m262-a002-lane-a-readiness`

## Canonical live proof surface

- `-fobjc-arc` is accepted and makes ownership-qualified executable methods and
  functions runnable
- ownership-qualified property surfaces compile under explicit ARC mode
- block captures using ownership-qualified values compile under explicit ARC mode
- `-fno-objc-arc` is accepted as an explicit non-ARC mode
- non-ARC mode still rejects executable ownership-qualified method/function
  signatures with `O3S221`
- `Objc3ArcModeHandlingSummary(...)`
- `!objc3.objc_arc_mode_handling`

## Live proof fixtures

- `tests/tooling/fixtures/native/m262_arc_mode_handling_positive.objc3`
- `tests/tooling/fixtures/native/hello.objc3`

## Handoff

`M262-B001` is the explicit next handoff after this implementation lands.
