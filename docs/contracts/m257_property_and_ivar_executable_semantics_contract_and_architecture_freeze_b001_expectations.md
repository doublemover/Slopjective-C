# M257 Property And Ivar Executable Semantics Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-executable-property-ivar-semantics/m257-b001-v1`

## Objective

Freeze the runtime-meaningful semantic rules for property synthesis, ivar
binding, accessor resolution, attribute interactions, and storage-compatible
layout publication before live accessor bodies or runtime allocation land.

## Required implementation

1. Add a canonical expectations document for the executable property/ivar
   semantic boundary.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-B
   readiness runner:
   - `scripts/check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py`
   - `scripts/run_m257_b001_lane_b_readiness.py`
3. Add `M257-B001` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. Freeze the semantic boundary around:
   - deterministic implicit ivar binding for non-category class-interface properties
   - deterministic effective getter/setter selector resolution from property attributes
   - deterministic attribute/ownership profiles as declaration-level compatibility data
   - deterministic layout slots/sizes/alignment as storage-local data that must not participate in protocol compatibility
   - fail-closed drift rules that block later accessor-body/storage realization from silently changing this boundary
5. The checker must prove the boundary with two live compiles:
   - `tests/tooling/fixtures/native/m257_property_ivar_source_model_completion_positive.objc3`
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   The second probe must stay protocol-compatible after the `M257-A002`
   storage/layout expansion, proving declaration compatibility excludes
   storage-local layout identities.
6. `package.json` must wire:
   - `check:objc3c:m257-b001-property-and-ivar-executable-semantics`
   - `test:tooling:m257-b001-property-and-ivar-executable-semantics`
   - `check:objc3c:m257-b001-lane-b-readiness`
7. The contract must explicitly hand off to `M257-B002`.

## Canonical models

- Synthesis semantics model:
  `non-category-class-interface-properties-own-deterministic-implicit-ivar-and-synthesized-binding-identities-until-explicit-synthesize-lands`
- Accessor semantics model:
  `readonly-and-attribute-driven-accessor-selectors-resolve-to-one-declaration-level-profile-before-body-emission`
- Storage semantics model:
  `interface-owned-property-layout-slots-sizes-and-alignment-remain-deterministic-before-runtime-allocation`
- Compatibility semantics model:
  `protocol-and-inheritance-compatibility-compare-declaration-level-attribute-accessor-ownership-profiles-not-storage-local-layout-symbols`
- Failure model:
  `fail-closed-on-property-runtime-semantic-boundary-drift-before-accessor-body-or-storage-realization`

## Non-goals

- No explicit `@synthesize` / `@dynamic` implementation yet.
- No synthesized accessor body emission yet.
- No runtime allocation or instance layout realization yet.
- No runtime property getter/setter execution semantics yet.

## Evidence

- `tmp/reports/m257/M257-B001/property_ivar_executable_semantics_contract_summary.json`
