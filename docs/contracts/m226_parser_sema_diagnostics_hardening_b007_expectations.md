# M226 Parser-to-Sema Diagnostics Hardening Expectations (B007)

Contract ID: `objc3c-parser-sema-diagnostics-hardening-contract/m226-b007-v1`
Status: Accepted
Scope: Parser->sema diagnostics accounting and fail-closed publish consistency.

## Objective

Harden sema diagnostics flow so parser->sema execution fails closed when
diagnostic accounting or diagnostics-bus publication drift from deterministic
per-pass expectations.

## Required Invariants

1. Per-pass diagnostics accounting tracks expected aggregate size:
   - `expected_diagnostics_size += pass_diagnostics.size()`
   - `result.diagnostics.size() == expected_diagnostics_size`
2. Diagnostics bus publication is checked when a diagnostics sink is wired:
   - `diagnostics_bus_count_after_publish == diagnostics_bus_count_before_publish + pass_diagnostics.size()`
3. Final deterministic diagnostics gate includes:
   - diagnostics accounting consistency
   - diagnostics bus publish consistency
   - emitted-total parity (`sum(diagnostics_emitted_by_pass) == diagnostics.size()`)
   - monotonic diagnostics-after-pass counts
4. Sema pass manager fails closed on diagnostics determinism drift before type
   metadata handoff work continues.
5. Checker and tests are fail-closed and write summary output under
   `tmp/reports/m226/`.

## Validation

- `python scripts/check_m226_b007_parser_sema_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b007_parser_sema_diagnostics_hardening_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B007/parser_sema_diagnostics_hardening_summary.json`

