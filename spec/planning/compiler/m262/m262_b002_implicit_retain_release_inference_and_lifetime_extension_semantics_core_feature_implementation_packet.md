# M262-B002 Implicit Retain-Release Inference And Lifetime-Extension Semantics Core Feature Implementation Packet

Packet: `M262-B002`

Issue: `#7197`

## Objective

Implement one canonical ARC inference and lifetime boundary for the supported
runnable slice so unqualified object signatures no longer depend on explicit
ownership spelling alone under `-fobjc-arc`.

## Dependencies

- `M262-A002`
- `M262-B001`

## Contract

- contract id
  `objc3c-arc-inference-lifetime/m262-b002-v1`
- source model
  `explicit-arc-mode-now-infers-strong-owned-executable-object-signatures-for-the-supported-runnable-slice`
- semantic model
  `arc-enabled-unqualified-object-signatures-now-produce-canonical-retain-release-lifetime-accounting-while-nonarc-remains-zero-inference`
- failure model
  `non-arc-mode-keeps-unqualified-object-signatures-non-inferred-and-zero-retain-release-lifetime-accounting`
- non-goal model
  `no-full-arc-cleanup-synthesis-no-weak-autorelease-return-property-synthesis-or-block-interaction-arc-semantics-yet`

## Required anchors

- `docs/contracts/m262_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation_b002_expectations.md`
- `scripts/check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py`
- `tests/tooling/test_check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py`
- `scripts/run_m262_b002_lane_b_readiness.py`
- `check:objc3c:m262-b002-implicit-retain-release-inference-and-lifetime-extension-semantics-contract`
- `check:objc3c:m262-b002-lane-b-readiness`

## Canonical implementation surface

- explicit ARC mode now infers strong-owned object parameters and returns for
  the supported runnable slice
- explicit ARC mode now infers a strong-owned property lifetime profile for the
  supported object-property slice
- non-ARC remains a zero-inference baseline for the same source
- the retain/release lowering replay profile is the canonical live proof
- `Objc3ArcInferenceLifetimeSummary()`

## Live proof fixture

- `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3`

## Handoff

`M262-B003` is the explicit next handoff after this implementation lands.
