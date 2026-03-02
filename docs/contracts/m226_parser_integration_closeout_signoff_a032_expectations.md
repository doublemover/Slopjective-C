# M226 Parser Integration Closeout and Gate Sign-off Expectations (A032)

Contract ID: `objc3c-parser-integration-closeout-signoff-contract/m226-a032-v1`
Status: Accepted
Scope: Parser lane-A integration closeout and gate sign-off for native frontend decomposition and parser completeness.

## Objective

Close parser lane-A by proving shard-2 and shard-3 integration summaries are
present, deterministic, and explicitly marked as gate-ready.

## Required Invariants

1. Closeout runner exists:
   - `scripts/run_m226_a032_parser_integration_closeout_signoff.ps1`
2. Runner consumes upstream integration summaries:
   - `tmp/reports/m226/M226-A025/parser_integration_shard2_summary.json`
   - `tmp/reports/m226/M226-A031/parser_integration_shard3_summary.json`
3. Runner fails closed when either upstream summary is missing or has
   `integrated_ok != true`.
4. Runner emits closeout summary with explicit sign-off fields:
   - `gate_signoff_ready`
   - `closeout_ready`
5. Runner writes summary evidence under:
   - `tmp/reports/m226/M226-A032/parser_integration_closeout_signoff_summary.json`
6. Validation entrypoints are pinned:
   - `python scripts/check_m226_a032_parser_integration_closeout_signoff_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a032_parser_integration_closeout_signoff_contract.py -q`

## Validation

- `python scripts/check_m226_a032_parser_integration_closeout_signoff_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a032_parser_integration_closeout_signoff_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m226_a032_parser_integration_closeout_signoff.ps1`

## Evidence Path

- `tmp/reports/m226/M226-A032/parser_integration_closeout_signoff_summary.json`
