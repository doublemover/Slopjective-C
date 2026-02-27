# v0.11 Spec Open Issues Seed Summary (`EV-04`) {#v011-spec-open-issues-summary}

Generated on `2026-02-23` for `D-05` quality-gate consumption.

## 1. Source Artifacts

| Artifact | Path |
| --- | --- |
| Seed traceability matrix | `tests/conformance/spec_open_issues/P0-P3-seed_traceability.csv` |
| Seed fixture manifest | `tests/conformance/spec_open_issues/manifest.json` |
| Dashboard context | `reports/conformance/dashboard_v011.md` |
| Gate decision context | `reports/releases/v011_quality_gate_decision.md` |

## 2. Aggregate Pass/Fail Totals

| Metric | Value |
| --- | --- |
| `release_label` | `v0.11` |
| `seed_alias_count` | `5` |
| `test_row_count` | `10` |
| `required_pass_rows` | `5` |
| `required_fail_rows` | `5` |
| `coverage_rule` | `Each seed alias has >=1 required-pass and >=1 required-fail row` |
| `coverage_result` | `pass` |

## 3. Seed Coverage Matrix

| seed_alias | required_pass_rows | required_fail_rows | status |
| --- | ---: | ---: | --- |
| `S1-P0-AMSYNC` | `1` | `1` | `pass` |
| `S1-P0-MANIFEST` | `1` | `1` | `pass` |
| `S1-P3-MANGLE` | `1` | `1` | `pass` |
| `S1-P3-OPT` | `1` | `1` | `pass` |
| `S1-P3-REIFY` | `1` | `1` | `pass` |

## 4. Stable Failure-Signal Report

| test_id | expected_signal | profile_scope | fixture_ref |
| --- | --- | --- | --- |
| `P0-P3-AMSYNC-FAIL-01` | `AMSYNC-DRIFT-DETECTED` | `all` | `tests/conformance/spec_open_issues/P0-P3-AMSYNC-FAIL-01.json` |
| `P0-P3-MANIFEST-FAIL-01` | `MANIFEST-VALIDATION-SCHEMA-MISMATCH` | `all` | `tests/conformance/spec_open_issues/P0-P3-MANIFEST-FAIL-01.json` |
| `P0-P3-MANGLE-FAIL-01` | `DEMANGLE-GFN-UNSUPPORTED-SHAPE` | `all` | `tests/conformance/spec_open_issues/P0-P3-MANGLE-FAIL-01.json` |
| `P0-P3-OPT-FAIL-01` | `OPT-SPELL-RESERVED-ALIAS` | `core|strict|strict-concurrency|strict-system` | `tests/conformance/spec_open_issues/P0-P3-OPT-FAIL-01.json` |
| `P0-P3-REIFY-FAIL-01` | `CAPREP-UNKNOWN-CAPABILITY` | `core|strict|strict-concurrency|strict-system` | `tests/conformance/spec_open_issues/P0-P3-REIFY-FAIL-01.json` |

## 5. Deterministic Validation Snapshot

| Check ID | Command summary | Result |
| --- | --- | --- |
| `V011-SUM-01` | Count required-pass and required-fail rows from CSV | `pass` (`5/5`) |
| `V011-SUM-02` | Verify per-seed dual coverage (`required-pass` + `required-fail`) | `pass` (`5/5 seeds`) |
| `V011-SUM-03` | Enumerate expected stable failure signals | `pass` (`5 signals`) |

## 6. Consumption Notes

- This artifact satisfies issue `#185` `EV-04` path requirement: `reports/conformance/spec_open_issues_v011_summary.md`.
- `EV-05` reproducibility evidence remains at `reports/conformance/reproducibility/v011_rerun_digest_report.md`.
