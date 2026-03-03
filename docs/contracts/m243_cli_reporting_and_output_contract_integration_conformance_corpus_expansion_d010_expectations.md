# M243 CLI/Reporting and Output Contract Integration Conformance Corpus Expansion Expectations (D010)

Contract ID: `objc3c-cli-reporting-output-contract-integration-conformance-corpus-expansion/m243-d010-v1`
Status: Accepted
Scope: lane-D conformance corpus expansion continuity for CLI/reporting output contract integration and deterministic fail-closed corpus key closure.

## Objective

Expand lane-D conformance closure on top of D009 so CLI/reporting summary and
diagnostics output contracts preserve deterministic conformance-corpus
consistency/readiness and conformance-corpus-key continuity in addition to
conformance-matrix closure.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-D009`
- M243-D009 conformance matrix implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m243_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_d009_expectations.md`
  - `spec/planning/compiler/m243/m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_packet.md`
  - `scripts/check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py`
- Packet/checker/test assets for D010 remain mandatory:
  - `spec/planning/compiler/m243/m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_packet.md`
  - `scripts/check_m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract.py`
  - `tests/tooling/test_check_m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract.py`
- Conformance corpus expansion fixtures remain mandatory:
  - `tests/conformance/diagnostics/M243-D010-C001.json`
  - `tests/conformance/diagnostics/M243-D010-R001.json`
  - `tests/conformance/diagnostics/manifest.json`

## Deterministic Invariants

1. `Objc3CliReportingOutputContractConformanceCorpusExpansionSurface`
   remains the canonical lane-D D010 conformance corpus expansion surface for
   CLI/reporting output contract integration.
2. `BuildObjc3CliReportingOutputContractConformanceCorpusExpansionKey(...)`
   and
   `BuildObjc3CliReportingOutputContractConformanceCorpusExpansionSurface(...)`
   remain the canonical D010 conformance-corpus builders for:
   - D009 conformance-matrix continuity
   - deterministic conformance-corpus case-accounting continuity
   - deterministic conformance-corpus key synthesis
3. `objc3c_frontend_c_api_runner.cpp` wires
   `BuildObjc3CliReportingOutputContractConformanceCorpusExpansionSurface(...)`
   and
   `IsObjc3CliReportingOutputContractConformanceCorpusExpansionSurfaceReady(...)`
   and fail-closes before writing summary output.
4. Runner summary `output_contract` payload remains deterministic and includes:
   - `conformance_corpus_key`
   - `conformance_corpus_consistent`
   - `conformance_corpus_ready`
   - `conformance_corpus_key_ready`
   - `conformance_corpus_case_count`
   - `conformance_corpus_accept_case_count`
   - `conformance_corpus_reject_case_count`
5. D010 conformance corpus fixture expansion remains anchored by issue `#6474`
   in `tests/conformance/diagnostics/manifest.json` with explicit
   `M243-D010-C001` and `M243-D010-R001` fixture references.
6. Shared architecture/spec anchors explicitly include M243 lane-D D010:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Code and Spec Anchors

- `native/objc3c/src/io/objc3_cli_reporting_output_contract_conformance_corpus_expansion_surface.h`
- `native/objc3c/src/io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h`
- `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
- `tests/conformance/diagnostics/M243-D010-C001.json`
- `tests/conformance/diagnostics/M243-D010-R001.json`
- `tests/conformance/diagnostics/manifest.json`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-d010-cli-reporting-output-contract-integration-conformance-corpus-expansion-contract`.
- `package.json` includes
  `test:tooling:m243-d010-cli-reporting-output-contract-integration-conformance-corpus-expansion-contract`.
- `package.json` includes `check:objc3c:m243-d010-lane-d-readiness`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py`
- `python scripts/check_m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m243-d010-lane-d-readiness`

## Evidence Path

- `tmp/reports/m243/M243-D010/cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract_summary.json`

