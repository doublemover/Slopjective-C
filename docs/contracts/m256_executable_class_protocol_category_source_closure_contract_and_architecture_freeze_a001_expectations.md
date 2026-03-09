# M256 Executable Class Protocol Category Source Closure Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-executable-class-protocol-category-source-closure/m256-a001-v1`
Status: Accepted
Issue: `#7129`
Scope: `M256` lane-A contract and architecture freeze for the runnable
class/protocol/category source closure.

## Objective

Freeze one deterministic parser/sema/IR source-closure boundary for classes,
protocols, and categories before later `M256` issues make those declarations
runnable through live realization, category attachment, and protocol
conformance behavior.

## Required Invariants

1. `parse/objc3_parser.cpp` remains the canonical parser-owned source surface
   for:
   - interface superclass names
   - adopted protocol lists in lexicographic semantic-link order
   - canonical interface/implementation/category owner identities via
     `BuildObjcContainerScopeOwner(...)`
   - category attachment identities via
     `BuildObjcCategorySemanticLinkSymbol(...)`
2. `sema/objc3_semantic_passes.cpp` remains the canonical semantic closure
   owner for:
   - `interface_implementation_summary`
   - `protocol_category_composition_summary`
   - `class_protocol_category_linking_summary`
3. `ir/objc3_ir_emitter.cpp` continues to publish the same source-closure proof
   surface via:
   - `!objc3.objc_interface_implementation`
   - `!objc3.objc_protocol_category`
   - `!objc3.objc_class_protocol_category_linking`
4. The frozen boundary explicitly preserves later-implementation handoff for:
   - inheritance
   - metaclass derivation from interface identity
   - category attachment ownership
   - adopted protocol composition ordering
5. The issue remains freeze/evidence only. It does not claim runnable
   realization semantics yet.

## Non-Goals and Fail-Closed Rules

- `M256-A001` does not implement runtime class realization.
- `M256-A001` does not implement category merge behavior.
- `M256-A001` does not implement protocol conformance enforcement.
- `M256-A001` does not implement executable method binding or instance layout.
- `M256-A001` must fail closed on drift before `M256-A002` begins expanding the
  executable class/metaclass source surface.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m256-a001-executable-class-protocol-category-source-closure-contract`.
- `package.json` includes
  `test:tooling:m256-a001-executable-class-protocol-category-source-closure-contract`.
- `package.json` includes `check:objc3c:m256-a001-lane-a-readiness`.

## Validation

- `python scripts/check_m256_a001_executable_class_protocol_category_source_closure_contract.py`
- `python -m pytest tests/tooling/test_check_m256_a001_executable_class_protocol_category_source_closure_contract.py -q`
- `npm run check:objc3c:m256-a001-lane-a-readiness`

## Evidence Path

- `tmp/reports/m256/M256-A001/executable_class_protocol_category_source_closure_contract_summary.json`
