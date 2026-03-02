# Parser-to-Sema Modular Handoff Scaffolding Expectations (M226-B002)

Contract ID: `objc3c-parser-sema-modular-handoff-contract/m226-b002-v1`

## Scope

This contract defines Lane B modular split/scaffolding requirements for parser-to-sema handoff preflight in sema-owned code paths.

## Deterministic Invariants

| ID | Requirement |
| --- | --- |
| `M226-B002-INV-01` | `native/objc3c/src/sema/objc3_parser_sema_handoff_scaffold.h` exists and provides canonical scaffold helpers for parser snapshot resolution and consistency checks before sema passes run. |
| `M226-B002-INV-02` | `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h` exposes parser snapshot plumbing in sema input/output (`parser_contract_snapshot` pointer on input, resolved snapshot plus determinism bit on result). |
| `M226-B002-INV-03` | `native/objc3c/src/sema/objc3_sema_pass_manager.cpp` consumes `BuildObjc3ParserSemaHandoffScaffold(...)` and fail-closes (`if (!handoff.deterministic)`) before pass execution. |
| `M226-B002-INV-04` | Pass execution order and parser-owned wrapper boundary remain deterministic (`kObjc3SemaPassOrder` and `Objc3ParsedProgram` contract continue to gate sema entry). |
| `M226-B002-INV-05` | Contract validation remains fail-closed with deterministic JSON summary output under `tmp/reports/`. |

## Acceptance Checks

- `python scripts/check_m226_b002_parser_sema_modular_handoff_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b002_parser_sema_modular_handoff_contract.py -q`

Default checker output: `tmp/reports/m226_b002_parser_sema_modular_handoff_contract_summary.json`.
