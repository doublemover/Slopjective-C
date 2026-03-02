# M226 Parser Advanced Conformance Workpack Expectations (A024)

Contract ID: `objc3c-parser-advanced-conformance-workpack-contract/m226-a024-v1`
Status: Accepted
Scope: Parser advanced conformance workpack shard 2 for native frontend decomposition and parser completeness.

## Objective

Execute deterministic two-pass parser conformance validation across expanded
native fixtures and fail closed on parser-stage or parse-lowering-readiness
recovery/determinism drift.

## Required Invariants

1. Shard-2 conformance runner exists:
   - `scripts/run_m226_a024_parser_conformance_workpack.ps1`
2. Runner compiles these fixtures using native wrapper:
   - `hello.objc3`
   - `return_paths_ok.objc3`
   - `typed_i32_bool.objc3`
   - `comparison_logic.objc3`
3. Runner executes two passes per fixture and enforces deterministic equality
   for parser-stage and parse-lowering-readiness projections.
4. Runner validates parser-stage key coverage per pass:
   - `diagnostic_code_count`
   - `diagnostic_code_fingerprint`
   - `diagnostic_code_surface_deterministic`
   - `interface_categories`
   - `implementation_categories`
   - `function_prototypes`
   - `function_pure`
   - `protocol_properties`
   - `protocol_methods`
   - `interface_properties`
   - `interface_methods`
   - `implementation_properties`
   - `implementation_methods`
5. Runner validates parse-lowering-readiness recovery/determinism keys:
   - `parse_artifact_replay_key_deterministic`
   - `parse_artifact_diagnostics_hardening_consistent`
   - `parse_artifact_edge_case_robustness_consistent`
   - `parse_recovery_determinism_hardening_consistent`
   - `parse_recovery_determinism_hardening_key`
   - `ready_for_lowering`
6. Runner writes summary evidence under:
   - `tmp/reports/m226/M226-A024/parser_conformance_shard2_summary.json`
7. Validation entrypoints are pinned:
   - `python scripts/check_m226_a024_parser_advanced_conformance_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a024_parser_advanced_conformance_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a024_parser_advanced_conformance_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a024_parser_advanced_conformance_workpack_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m226_a024_parser_conformance_workpack.ps1`

## Evidence Path

- `tmp/reports/m226/M226-A024/parser_conformance_shard2_summary.json`
