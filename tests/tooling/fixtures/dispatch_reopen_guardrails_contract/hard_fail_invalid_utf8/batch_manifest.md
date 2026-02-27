# Fixture: Dispatch Reopen Guardrails W1 Batch Manifest

- `batch_id`: `BATCH-20260224-M17`
- Scope class: dispatch-reopen baseline contracts + deterministic tooling/tests + CI/operator parity + governance/runbook controls
- Source-of-truth inputs:
  - `spec/planning/v013_activation_reopen_playbook_20260223.md`
  - `spec/planning/v013_activation_preflight_runner_package_20260223.md`
  - `spec/planning/v013_next_dispatch_candidate_batch_20260223.md`
  - `spec/planning/v014_dispatch_reopen_guardrails_w1_dispatch_review_20260224.md`

## 3. Lane Assignment Matrix

| Lane | Issue | Agent ID | Owned path(s) |
| --- | --- | --- | --- |
| `A` | `#904` | `019c90a2-bf71-70f0-9915-0850bedee4b8` | Lane A baseline docs |
| `B` | `#905` | `019c90a2-bfc8-7801-ac60-2137d732fd46` | dispatch-reopen tooling/tests/fixtures + lane-B evidence |
| `C` | `#906` | `019c90a2-bfd9-7df1-9eac-8c06ceb7bd05` | Lane C CI/operator entrypoints |
| `D` | `#907` | `019c90a2-c004-7b71-acbe-74574227ca9a` | Lane D governance controls |
| `INT` | `#908` | n/a | Integrator orchestration |

## 4. Dependency and Non-Overlap Rules

1. Lanes edit only owned paths from Section `3`.
2. Lane `A` defines deterministic dispatch-reopen baseline semantics consumed by `B/C/D`.
3. Lane `B` provides executable enforcement consumed by lane `C` entrypoints and integrator verification.

## 5. Merge and Commit Protocol

Deterministic intake order:

1. Lane `A` (`#904`)
2. Lane `B` (`#905`)
3. Lane `C` (`#906`)
4. Lane `D` (`#907`)
5. Integrator (`#908`)

## 6. Verification Contract

Lane minimum checks:

1. Lane `A`: `python scripts/spec_lint.py`
2. Lane `B`: `python -m pytest tests/tooling -q`; `python scripts/spec_lint.py`
3. Lane `C`: `python scripts/spec_lint.py`; `python scripts/check_issue_checkbox_drift.py`
4. Lane `D`: `python scripts/spec_lint.py`

Integrator final checks:

1. `python scripts/spec_lint.py`
2. `python scripts/check_issue_checkbox_drift.py`
3. `python -m pytest tests/tooling -q`
4. `npm run check:extension-registry-compatibility:w1:strict`
5. `npm run check:dispatch-reopen-guardrails:w1:strict`

## 7. Execution Status

- Status: `CLOSED`
- Lane `B` / issue `#905`: `373b4db`
