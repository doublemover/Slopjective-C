# M262 Implicit Retain-Release Inference And Lifetime-Extension Semantics Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-arc-inference-lifetime/m262-b002-v1`

Issue: `#7197`

## Objective

Make implicit ARC retain/release inference and the supported lifetime-extension
boundary a real compiler-visible semantic capability for unqualified object
signatures under explicit ARC mode.

## Required implementation

1. Add a canonical expectations document for ARC inference and lifetime
   semantics.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-B
   readiness runner:
   - `scripts/check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py`
   - `tests/tooling/test_check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py`
   - `scripts/run_m262_b002_lane_b_readiness.py`
3. Add `M262-B002` anchor text to:
   - `docs/objc3c-native/src/20-grammar.md`
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
   - `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. The implementation must make the following truthful:
   - unqualified object parameters infer strong-owned retain/release behavior under `-fobjc-arc` for the supported runnable slice
   - under `-fobjc-arc`, unqualified object returns infer strong-owned
     retain/release behavior for the supported runnable slice
   - under `-fobjc-arc`, unqualified object property surfaces infer a
     strong-owned lifetime profile in semantic integration
   - the same source remains a zero-inference baseline without ARC mode
   - emitted IR carries:
     - `; arc_inference_lifetime = ...`
     - `!objc3.objc_arc_inference_lifetime`
5. The checker must prove the boundary with focused live probes:
   - `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3`
     compiles cleanly under `-fobjc-arc`
   - the same fixture compiles cleanly without ARC mode
   - ARC-enabled manifests show nonzero:
     - `ownership_qualified_sites`
     - `retain_insertion_sites`
     - `release_insertion_sites`
   - non-ARC manifests preserve zero inferred retain/release accounting for the
     same source
   - the fixture still traverses the existing block escape profile so lifetime
     proof is not reduced to a signature-only probe
6. `package.json` must wire:
   - `check:objc3c:m262-b002-implicit-retain-release-inference-and-lifetime-extension-semantics-contract`
   - `test:tooling:m262-b002-implicit-retain-release-inference-and-lifetime-extension-semantics-contract`
   - `check:objc3c:m262-b002-lane-b-readiness`
7. The contract must explicitly hand off to `M262-B003`.

## Canonical models

- Source model:
  `explicit-arc-mode-now-infers-strong-owned-executable-object-signatures-for-the-supported-runnable-slice`
- Semantic model:
  `arc-enabled-unqualified-object-signatures-now-produce-canonical-retain-release-lifetime-accounting-while-nonarc-remains-zero-inference`
- Failure model:
  `non-arc-mode-keeps-unqualified-object-signatures-non-inferred-and-zero-retain-release-lifetime-accounting`
- Non-goal model:
  `no-full-arc-cleanup-synthesis-no-weak-autorelease-return-property-synthesis-or-block-interaction-arc-semantics-yet`

## Non-goals

- No full ARC cleanup synthesis yet.
- No weak/autorelease-return ARC semantics yet.
- No property-synthesis ARC automation yet.
- No broader block-interaction ARC semantics yet.

## Evidence

- `tmp/reports/m262/M262-B002/arc_inference_lifetime_summary.json`
