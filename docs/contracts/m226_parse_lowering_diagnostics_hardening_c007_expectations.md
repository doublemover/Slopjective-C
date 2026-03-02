# M226 Parse-Lowering Diagnostics Hardening Expectations (C007)

Contract ID: `objc3c-parse-lowering-diagnostics-hardening-contract/m226-c007-v1`
Status: Accepted
Scope: Parse/lowering diagnostics-hardening coverage in `native/objc3c/src/pipeline/*`.

## Objective

Harden parse-to-lowering readiness by requiring deterministic parser diagnostic
surface accounting (including unique diagnostic-code coverage and fingerprint)
before lower-stage readiness is considered stable.

## Required Invariants

1. Readiness surface tracks diagnostics-hardening evidence:
   - `parser_diagnostic_surface_consistent`
   - `parser_diagnostic_code_surface_deterministic`
   - `parse_artifact_diagnostics_hardening_consistent`
   - `parser_diagnostic_code_count`
   - `parser_diagnostic_code_fingerprint`
   - `parse_artifact_diagnostics_hardening_key`
2. Readiness builder computes parser diagnostic code coverage using explicit
   helper surfaces:
   - `TryExtractObjc3ParseLoweringDiagnosticCode(...)`
   - `BuildObjc3ParseLoweringDiagnosticCodeCoverage(...)`
   - `BuildObjc3ParseArtifactDiagnosticsHardeningKey(...)`
3. Readiness fails closed when diagnostics-hardening invariants drift:
   - parser diagnostics surface inconsistency
   - non-deterministic parser diagnostic code surface
   - diagnostics hardening inconsistency
4. Parse snapshot replay readiness requires diagnostics-hardening consistency.
5. Manifest projection includes diagnostics-hardening fields and keys under
   `parse_lowering_readiness`.
6. Validation entrypoints are pinned:
   - `python scripts/check_m226_c007_parse_lowering_diagnostics_hardening_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_c007_parse_lowering_diagnostics_hardening_contract.py -q`

## Validation

- `python scripts/check_m226_c007_parse_lowering_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c007_parse_lowering_diagnostics_hardening_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_c007_parse_lowering_diagnostics_hardening_contract_summary.json`
