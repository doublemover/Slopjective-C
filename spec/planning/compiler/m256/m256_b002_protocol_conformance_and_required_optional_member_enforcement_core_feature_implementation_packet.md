# M256-B002 Protocol Conformance and Required/Optional Member Enforcement Core Feature Implementation Packet

Packet: `M256-B002`
Milestone: `M256`
Lane: `B`
Issue: `#7133`
Contract ID: `objc3c-protocol-conformance-required-optional-enforcement/m256-b002-v1`
Dependencies: `M256-B001`, `M256-A003`
Next issue: `M256-B003`

## Summary

Implement live lane-B protocol conformance enforcement for required methods and required properties while preserving parser-owned required/optional partitioning and keeping optional members non-blocking.

## Acceptance criteria

- `@required` and `@optional` partitioning is consumed from parser-owned AST state.
- Required methods are enforced across protocol inheritance closures.
- Required properties are enforced across protocol inheritance closures.
- Category interfaces may satisfy adopted protocol members through category declarations plus base-class declarations.
- Missing and incompatible required members fail closed with `O3S218`.
- Positive proof compiles through `artifacts/bin/objc3c-native.exe` on the `llvm-direct` backend.
- Evidence lands at `tmp/reports/m256/M256-B002/protocol_conformance_required_optional_member_enforcement_summary.json`.

## Ownership boundary

- Parser owns required/optional source partitioning.
- Sema owns required-member closure construction and conformance enforcement.
- IR owns proof publication only and must not reinterpret conformance legality.

## Required anchors

- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Evidence

- Canonical summary path:
  `tmp/reports/m256/M256-B002/protocol_conformance_required_optional_member_enforcement_summary.json`

## Validation

- `python scripts/check_m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation.py -q`
- `npm run check:objc3c:m256-b002-lane-b-readiness`
