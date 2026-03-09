# M256 Protocol Conformance and Required/Optional Member Enforcement Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-protocol-conformance-required-optional-enforcement/m256-b002-v1`
Status: Accepted
Issue: `#7133`
Scope: `M256` lane-B implementation of executable protocol conformance for required and optional methods and properties.

## Objective

Implement live sema enforcement for declared protocol conformance so required protocol members must be present and signature-compatible while optional members remain non-blocking.

## Required implementation

1. Parser support remains authoritative for `@required` and `@optional` partitioning inside `@protocol` containers.
2. `sema/objc3_semantic_passes.cpp` must:
   - build a required-only protocol requirement closure across inherited protocols
   - fail closed when inherited required members are incompatible
   - enforce required methods and required properties for interfaces and category interfaces
   - treat optional methods and optional properties as non-blocking
   - emit deterministic `O3S218` diagnostics for missing or incompatible required members
3. `ir/objc3_ir_emitter.cpp` remains downstream proof/consumer only for this issue and must not reinterpret required-vs-optional legality.
4. Canonical proof fixtures must include:
   - `tests/tooling/fixtures/native/m256_protocol_conformance_positive.objc3`
   - `tests/tooling/fixtures/native/m256_protocol_conformance_missing_required.objc3`
   - `tests/tooling/fixtures/native/m256_protocol_conformance_incompatible_signature.objc3`
   - `tests/tooling/fixtures/native/m256_protocol_conformance_missing_required_property.objc3`
   - `tests/tooling/fixtures/native/m256_protocol_conformance_incompatible_property.objc3`
5. `package.json` must wire:
   - `check:objc3c:m256-b002-protocol-conformance-and-required-optional-member-enforcement`
   - `test:tooling:m256-b002-protocol-conformance-and-required-optional-member-enforcement`
   - `check:objc3c:m256-b002-lane-b-readiness`
6. Validation evidence lands at `tmp/reports/m256/M256-B002/protocol_conformance_required_optional_member_enforcement_summary.json`.

## Required diagnostics

- Missing required method => `O3S218`
- Incompatible required method => `O3S218`
- Missing required property => `O3S218`
- Incompatible required property => `O3S218`

## Non-goals

- No runtime-side protocol realization yet.
- No property synthesis or accessor generation.
- No category merge execution beyond semantic conformance lookup.
- No new lowering ABI.

## Validation

- `python scripts/check_m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation.py -q`
- `npm run check:objc3c:m256-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m256/M256-B002/protocol_conformance_required_optional_member_enforcement_summary.json`
