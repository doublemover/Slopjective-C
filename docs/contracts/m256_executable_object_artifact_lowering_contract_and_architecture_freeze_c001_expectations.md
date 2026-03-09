# M256 Executable Object Artifact Lowering Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-executable-object-artifact-lowering/m256-c001-v1`
Status: Accepted
Issue: `#7136`
Scope: M256 lane-C freeze of the current object-artifact lowering boundary that binds implementation method bodies and realized class/category records into emitted metadata artifacts without widening the runtime surface.

## Objective

Freeze the executable object artifact lowering boundary that already exists in the native IR/object path. The current compiler emits method bodies, owner-scoped method-list payloads, and realized class/category descriptor bundles. This issue makes that boundary explicit so later implementation can extend it without moving parser/sema/runtime responsibilities.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-executable-object-artifact-lowering/m256-c001-v1`
   - method-body binding model `implementation-owner-identity-to-llvm-definition-symbol`
   - realization-record model `class-metaclass-and-category-descriptor-bundles-point-to-owner-scoped-method-list-ref-records`
   - method-entry payload model `selector-owner-return-arity-implementation-symbol-has-body`
   - scope model `parser-source-identities-sema-realization-closure-ir-object-binding`
   - fail-closed model `no-synthetic-implementation-symbols-no-rebound-legality-no-new-section-families`.
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic boundary summary for the current executable object artifact surface.
3. `parse/objc3_parser.cpp` remains explicit that parser-owned implementation method bodies, selectors, and owner identities are source inputs only and do not decide emitted artifact binding.
4. `sema/objc3_semantic_passes.cpp` remains explicit that sema owns realized-object legality and canonical owner identities but does not synthesize object-artifact slots or body-symbol attachments.
5. `ir/objc3_ir_emitter.cpp` publishes the boundary directly into emitted IR through `; executable_object_artifact_lowering = ...` and keeps the implementation-owner-identity binding path explicit.
6. Happy-path native emission must keep producing real object artifacts where implementation-owned method-list entries can point at concrete `@objc3_method_*` LLVM definitions and class/category realization records still consume owner-scoped method-list refs.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m256_inheritance_override_realization_positive.objc3` proves the emitted IR carries the new executable-object boundary line, emits concrete `@objc3_method_*` definitions, and binds realized class/method-list payloads into a non-empty `module.obj`.
2. Native compile probe over `tests/tooling/fixtures/native/m256_category_merge_positive.objc3` proves the same boundary holds for category-owned executable method entries and category realization records.

## Non-Goals and Fail-Closed Rules

- `M256-C001` does not introduce new metadata section families.
- `M256-C001` does not reinterpret parser/sema legality inside IR/object emission.
- `M256-C001` does not add bootstrap/runtime-registration rebinding.
- `M256-C001` does not make protocol records executable realization records.
- If the executable object artifact boundary drifts, later lane-C implementation must fail closed rather than silently widening the binding surface.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m256-c001-executable-object-artifact-lowering-contract`.
- `package.json` includes `test:tooling:m256-c001-executable-object-artifact-lowering-contract`.
- `package.json` includes `check:objc3c:m256-c001-lane-c-readiness`.

## Validation

- `python scripts/check_m256_c001_executable_object_artifact_lowering_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m256_c001_executable_object_artifact_lowering_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m256-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m256/M256-C001/executable_object_artifact_lowering_contract_summary.json`
