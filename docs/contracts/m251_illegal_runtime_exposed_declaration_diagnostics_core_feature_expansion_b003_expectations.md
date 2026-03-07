# M251 Illegal Runtime-Exposed Declaration Diagnostics Expectations (B003)

Contract ID: `objc3c-runtime-export-diagnostics/m251-b003-v1`
Status: Accepted
Scope: M251 lane-B diagnostic expansion for declarations that parse but still cannot participate in runtime export.

## Objective

Preserve the B002 fail-closed runtime export barrier while making incomplete
runtime-export diagnostics precise, deterministic, and source-anchored for the
class/category declaration inventory that currently falls through to a broad
blocker.

## Required Invariants

1. `pipeline/objc3_frontend_pipeline.cpp` synthesizes deterministic runtime
   export blocker diagnostics from runtime metadata source records.
2. Interface-only runtime export units report a precise reason naming the class
   interface that is missing a matching `@implementation`.
3. Category-interface-only runtime export units report a precise reason naming
   the category that is missing a matching `@implementation`.
4. Existing fail-closed blocker text remains stable enough for the B002
   readiness chain to keep passing.
5. The happy-path manifest-only runtime metadata fixture remains export-ready.
6. `driver/objc3_objc3_path.cpp` continues to preserve diagnostics and manifest
   emission behavior before later runtime registration work lands.
7. `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains explicitly test-only
   evidence and not the native runtime implementation.

## Non-Goals

- `M251-B003` does not refactor category semantic ownership into a dedicated
  semantic surface.
- `M251-B003` does not change the runtime export metadata packet shape.
- `M251-B003` does not replace pre-existing semantic diagnostics that are
  already precise enough for duplicate or incompatible redeclaration cases.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m251-b003-illegal-runtime-exposed-declaration-diagnostics`.
- `package.json` includes
  `test:tooling:m251-b003-illegal-runtime-exposed-declaration-diagnostics`.
- `package.json` includes `check:objc3c:m251-b003-lane-b-readiness`.

## Validation

- `python scripts/check_m251_b003_illegal_runtime_exposed_declaration_diagnostics.py`
- `python -m pytest tests/tooling/test_check_m251_b003_illegal_runtime_exposed_declaration_diagnostics.py -q`
- `npm run check:objc3c:m251-b003-lane-b-readiness`

## Evidence Path

- `tmp/reports/m251/M251-B003/illegal_runtime_exposed_declaration_diagnostics_summary.json`
