# M233 Runtime Metadata and Lookup Plumbing Conformance Corpus Expansion Expectations (D010)

Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-conformance-corpus-expansion/m233-d010-v1`
Status: Accepted
Scope: M233 lane-D runtime metadata and lookup plumbing conformance corpus expansion continuity for deterministic readiness-chain and lookup-plumbing governance.

## Objective

Fail closed unless M233 lane-D runtime metadata and lookup plumbing
conformance corpus expansion anchors remain explicit, deterministic,
and traceable across dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M233-D009`
- Prerequisite conformance matrix implementation assets from `M233-D009` remain mandatory:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_conformance_matrix_implementation_d009_expectations.md`
  - `spec/planning/compiler/m233/m233_d009_runtime_metadata_and_lookup_plumbing_conformance_matrix_implementation_packet.md`
  - `scripts/check_m233_d009_runtime_metadata_and_lookup_plumbing_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m233_d009_runtime_metadata_and_lookup_plumbing_conformance_matrix_implementation_contract.py`
  - `scripts/run_m233_d009_lane_d_readiness.py`
- Packet/checker/test/readiness assets for `M233-D010` remain mandatory:
  - `spec/planning/compiler/m233/m233_d010_runtime_metadata_and_lookup_plumbing_conformance_corpus_expansion_packet.md`
  - `scripts/check_m233_d010_runtime_metadata_and_lookup_plumbing_conformance_corpus_expansion_contract.py`
  - `tests/tooling/test_check_m233_d010_runtime_metadata_and_lookup_plumbing_conformance_corpus_expansion_contract.py`
  - `scripts/run_m233_d010_lane_d_readiness.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-D `M233-D004`
  runtime metadata and lookup core feature expansion anchors inherited by D005 through
  D010 readiness-chain conformance corpus closure.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D runtime metadata and lookup
  conformance corpus expansion fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  runtime metadata and lookup conformance corpus expansion metadata wording for
  dependency continuity.

## Build and Readiness Integration

- `scripts/run_m233_d010_lane_d_readiness.py` enforces predecessor chaining
  through `python scripts/run_m233_d009_lane_d_readiness.py` before D010 checks execute.
- `package.json` continues to expose:
  - `check:objc3c:m233-d004-lane-d-readiness`
  - `compile:objc3c`
  - `proof:objc3c`
  - `test:objc3c:execution-replay-proof`
  - `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m233_d010_runtime_metadata_and_lookup_plumbing_conformance_corpus_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d010_runtime_metadata_and_lookup_plumbing_conformance_corpus_expansion_contract.py -q`
- `python scripts/run_m233_d010_lane_d_readiness.py`

## Evidence Path

- `tmp/reports/m233/M233-D010/runtime_metadata_and_lookup_plumbing_conformance_corpus_expansion_contract_summary.json`
