# Runtime-Facing Type Metadata Conformance Corpus Expansion Expectations (M227-D010)

Contract ID: `objc3c-runtime-facing-type-metadata-conformance-corpus-expansion/m227-d010-v1`
Status: Accepted
Dependencies: `M227-D009`
Scope: Lane-D runtime-facing type metadata conformance corpus expansion dependency continuity for deterministic fail-closed readiness integration.

## Objective

Execute issue `#5156` by enforcing lane-D runtime-facing type metadata
conformance corpus expansion governance on top of D009 conformance matrix
implementation assets so dependency continuity and readiness evidence remain
deterministic and fail closed.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#5156` defines canonical lane-D conformance corpus expansion scope.
- `M227-D009` assets remain mandatory prerequisites:
  - `docs/contracts/m227_runtime_facing_type_metadata_conformance_matrix_implementation_d009_expectations.md`
  - `spec/planning/compiler/m227/m227_d009_runtime_facing_type_metadata_conformance_matrix_implementation_packet.md`
  - `scripts/check_m227_d009_runtime_facing_type_metadata_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m227_d009_runtime_facing_type_metadata_conformance_matrix_implementation_contract.py`

## Deterministic Invariants

1. Runtime-facing typed sema-to-lowering conformance corpus fields remain explicit and
   fail closed in typed sema and parse/lowering readiness surfaces.
2. Conformance corpus consistency/readiness/key continuity remains deterministic and
   fails closed when `M227-D009` dependency references drift.
3. Lane-D readiness command chaining runs direct `M227-D009` checker/test
   evidence before `M227-D010` checker/test evidence.
4. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
5. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-d010-runtime-facing-type-metadata-conformance-corpus-expansion-contract`
  - `test:tooling:m227-d010-runtime-facing-type-metadata-conformance-corpus-expansion-contract`
  - `check:objc3c:m227-d010-lane-d-readiness`
- lane-D readiness chaining remains deterministic and fail closed:
  - `npm run check:objc3c:m227-d009-lane-d-readiness`
  - `check:objc3c:m227-d010-lane-d-readiness`

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m227_d009_runtime_facing_type_metadata_conformance_matrix_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d009_runtime_facing_type_metadata_conformance_matrix_implementation_contract.py -q`
- `python scripts/check_m227_d010_runtime_facing_type_metadata_conformance_corpus_expansion_contract.py`
- `python scripts/check_m227_d010_runtime_facing_type_metadata_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m227_d010_runtime_facing_type_metadata_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m227-d010-lane-d-readiness`

## Evidence Path

- `tmp/reports/m227/M227-D010/runtime_facing_type_metadata_conformance_corpus_expansion_contract_summary.json`
