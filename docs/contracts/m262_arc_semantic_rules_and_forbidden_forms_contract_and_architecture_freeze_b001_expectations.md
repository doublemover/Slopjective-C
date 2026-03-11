# M262 ARC Semantic Rules And Forbidden Forms Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-arc-semantic-rules/m262-b001-v1`

Issue: `#7196`

## Objective

Freeze the current ARC semantic-rule boundary after explicit ARC mode admission
is live, including the forbidden forms and non-goals that still remain
fail-closed.

## Required implementation

1. Add a canonical expectations document for ARC semantic rules and forbidden
   forms.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-B
   readiness runner:
   - `scripts/check_m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze.py`
   - `scripts/run_m262_b001_lane_b_readiness.py`
3. Add `M262-B001` anchor text to:
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
4. Freeze the current semantic boundary around:
   - explicit ARC mode admitting only explicit ownership-qualified executable
     surfaces
   - conflicting `@property` ownership forms still failing deterministically
   - atomic ownership-aware properties still failing deterministically
   - no broad ARC inference, lifetime extension, or method-family ARC semantics
     being claimed yet
   - `Objc3ArcSemanticRulesSummary()`
5. The checker must prove the boundary with focused live probes:
   - `tests/tooling/fixtures/native/m262_arc_mode_handling_positive.objc3`
     compiles cleanly under `-fobjc-arc`
   - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_weak_mismatch_negative.objc3`
     fails with deterministic ownership-conflict diagnostics
   - `tests/tooling/fixtures/native/m257_property_atomic_ownership_negative.objc3`
     fails with deterministic atomic ownership diagnostics
   - emitted IR carries:
     - `; arc_semantic_rules = ...`
     - `!objc3.objc_arc_semantic_rules`
6. `package.json` must wire:
   - `check:objc3c:m262-b001-arc-semantic-rules-and-forbidden-forms-contract`
   - `test:tooling:m262-b001-arc-semantic-rules-and-forbidden-forms-contract`
   - `check:objc3c:m262-b001-lane-b-readiness`
7. The contract must explicitly hand off to `M262-B002`.

## Canonical models

- Source model:
  `explicit-arc-mode-admits-only-explicit-ownership-surfaces-while-forbidden-property-forms-and-broad-inference-remain-fail-closed`
- Semantic model:
  `conflicting-property-ownership-forms-and-atomic-ownership-aware-storage-still-fail-closed-while-general-arc-inference-remains-deferred`
- Failure model:
  `forbidden-arc-property-forms-and-non-inferred-lifetime-semantics-terminate-deterministically`
- Non-goal model:
  `no-implicit-retain-release-inference-no-lifetime-extension-no-method-family-based-arc-semantics-yet`

## Non-goals

- No implicit retain/release inference yet.
- No ARC lifetime extension semantics yet.
- No method-family ARC inference yet.
- No claim that later ARC cleanup or destruction behavior is implemented yet.

## Evidence

- `tmp/reports/m262/M262-B001/arc_semantic_rules_summary.json`
