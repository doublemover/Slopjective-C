# M244 Interop Lowering and ABI Conformance Conformance Corpus Expansion Expectations (C010)

Contract ID: `objc3c-interop-lowering-and-abi-conformance-conformance-corpus-expansion/m244-c010-v1`
Status: Accepted
Dependencies: `M244-C009`
Scope: lane-C interop lowering/ABI conformance corpus expansion governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-C conformance corpus expansion governance for interop lowering and ABI
conformance on top of C009 conformance matrix implementation assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6559` defines canonical lane-C conformance corpus expansion scope.
- `M244-C009` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_lowering_and_abi_conformance_conformance_matrix_implementation_c009_expectations.md`
  - `spec/planning/compiler/m244/m244_c009_interop_lowering_and_abi_conformance_conformance_matrix_implementation_packet.md`
  - `scripts/check_m244_c009_interop_lowering_and_abi_conformance_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m244_c009_interop_lowering_and_abi_conformance_conformance_matrix_implementation_contract.py`

## Deterministic Invariants

1. lane-C conformance corpus expansion dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-C009` before `M244-C010`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-c010-interop-lowering-abi-conformance-conformance-corpus-expansion-contract`.
- `package.json` includes
  `test:tooling:m244-c010-interop-lowering-abi-conformance-conformance-corpus-expansion-contract`.
- `package.json` includes `check:objc3c:m244-c010-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-c009-lane-c-readiness`
  - `check:objc3c:m244-c010-lane-c-readiness`

## Validation

- `python scripts/check_m244_c010_interop_lowering_and_abi_conformance_conformance_corpus_expansion_contract.py`
- `python scripts/check_m244_c010_interop_lowering_and_abi_conformance_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c010_interop_lowering_and_abi_conformance_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m244-c010-lane-c-readiness`

## Evidence Path

- `tmp/reports/m244/M244-C010/interop_lowering_and_abi_conformance_conformance_corpus_expansion_contract_summary.json`

