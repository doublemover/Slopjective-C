# Extension Test Catalog Skeleton v0.11 (`C-09` Companion) {#extension-test-catalog-skeleton-v011}

_Finalized v0.11 - 2026-02-23_

Status: finalized companion artifact for issue `#175` (`C-09`) Section 9
requirements.

## 1. Purpose and Scope

This catalog skeleton materializes the required row schema defined by
`spec/planning/issue_175_extension_test_obligations_package.md` Section 9 and
provides canonical starter rows for `F-01` through `F-10`.

The artifact is intentionally deterministic:

- every required field is present in every row,
- conditional families include explicit predicates,
- gate bindings collectively cover `TG-01` through `TG-07`,
- non-waiverable family alignment with Section 6.2 is explicit.

## 2. Required Row Schema

| Field | Allowed/expected value shape | Validation rule |
| --- | --- | --- |
| `test_id` | Stable ID (`TST-Fxx-...`) | Unique per row; referenced by evidence and claim mappings. |
| `family_id` | `F-01`..`F-10` | Must map to one canonical family in issue `#175` Section 4.1. |
| `lifecycle_min_state` | `LS-1`..`LS-5` | Must reflect earliest lifecycle state where row can activate. |
| `condition_expression` | `always` or explicit predicate | Conditional families must not use `always`. |
| `severity_if_fail` | `P0`..`P3` | Must align with Section 3.3 severity semantics. |
| `required_profiles` | Comma-separated profile IDs | Must include all profiles where claim scope applies. |
| `interop_dimensions` | `none` or explicit matrix dimensions | Interop families must declare producer/consumer/platform axes. |
| `evidence_artifact_type` | Semicolon-separated evidence classes | Must map to Section 7.1 evidence bundle classes. |
| `waiver_eligibility` | `none` or waiver classes (`W-*`) | Non-waiverable families must be `none`. |
| `gate_bindings` | Comma-separated `TG-*` IDs | At least one gate per row. |

## 3. Canonical Skeleton Rows (`F-01`..`F-10`)

| test_id | family_id | lifecycle_min_state | condition_expression | severity_if_fail | required_profiles | interop_dimensions | evidence_artifact_type | waiver_eligibility | gate_bindings |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `TST-F01-SYNTAX-CORE-001` | `F-01` | `LS-1` | `always` | `P1` | `core,strict` | `none` | `test_result_ledger;environment_fingerprint;raw_logs_and_summary` | `W-01,W-04` | `TG-01,TG-02,TG-04` |
| `TST-F02-SEMA-SOUND-001` | `F-02` | `LS-1` | `always` | `P0` | `core,strict,strict-concurrency` | `none` | `test_result_ledger;raw_logs_and_summary;provenance_and_replay_metadata` | `none` | `TG-01,TG-02,TG-04,TG-05` |
| `TST-F03-DIAG-FIXIT-001` | `F-03` | `LS-1` | `always` | `P2` | `core,strict` | `none` | `test_result_ledger;raw_logs_and_summary` | `W-01,W-04` | `TG-01,TG-02,TG-04` |
| `TST-F04-ABI-INTEROP-001` | `F-04` | `LS-1` | `output.crosses_boundary in {binary,metadata}` | `P0` | `core,strict` | `producer_impl x consumer_impl x os_arch` | `interop_matrix_record;test_result_ledger;environment_fingerprint` | `none` | `TG-01,TG-02,TG-03,TG-04,TG-05` |
| `TST-F05-RUNTIME-INTEROP-001` | `F-05` | `LS-1` | `runtime.observable_by_external_impl = true` | `P0` | `core,strict` | `producer_impl x consumer_impl x runtime` | `interop_matrix_record;test_result_ledger;raw_logs_and_summary` | `none` | `TG-01,TG-02,TG-03,TG-04,TG-05` |
| `TST-F06-MODULE-BOUNDARY-001` | `F-06` | `LS-1` | `scope.crosses_module_or_package_boundary = true` | `P1` | `core,strict,strict-concurrency` | `producer_module x consumer_module x package_graph` | `interop_matrix_record;test_result_ledger;provenance_and_replay_metadata` | `W-02,W-03` | `TG-01,TG-02,TG-03,TG-04` |
| `TST-F07-SECURITY-ISOLATION-001` | `F-07` | `LS-1` | `always` | `P0` | `core,strict,strict-concurrency` | `sandbox_policy x platform_tuple` | `test_result_ledger;environment_fingerprint;provenance_and_replay_metadata` | `none` | `TG-01,TG-02,TG-04,TG-05` |
| `TST-F08-REPRO-REPLAY-001` | `F-08` | `LS-1` | `always` | `P1` | `core,strict,strict-concurrency` | `run_1 x run_2 x run_3` | `test_result_ledger;provenance_and_replay_metadata;environment_fingerprint` | `W-01,W-04` | `TG-01,TG-02,TG-04` |
| `TST-F09-MIGRATION-COMPAT-001` | `F-09` | `LS-2` | `proposal.includes_migration_or_fallback_claim = true` | `P1` | `core,strict` | `producer_version x consumer_version x deprecation_mode` | `interop_matrix_record;test_result_ledger;claim_linkage_manifest` | `W-02,W-03` | `TG-01,TG-02,TG-03,TG-04,TG-05` |
| `TST-F10-CLAIM-TRACE-001` | `F-10` | `LS-1` | `always` | `P1` | `core,strict,strict-concurrency` | `claim_id x test_id x profile_id` | `claim_linkage_manifest;test_result_ledger` | `none` | `TG-01,TG-06,TG-07` |

## 4. Section 9 Completeness Checks

| Check ID | Deterministic check | Result | Evidence note |
| --- | --- | --- | --- |
| `CAT-175-01` | Every row includes all ten required Section 9 fields. | `pass` | Section `3` table has no missing columns or empty field cells. |
| `CAT-175-02` | Family coverage includes `F-01` through `F-10`. | `pass` | Exactly one canonical row is present for each family ID. |
| `CAT-175-03` | Conditional families declare explicit predicates. | `pass` | `F-04`, `F-05`, `F-06`, and `F-09` use non-`always` expressions. |
| `CAT-175-04` | Each row maps to one or more gates and aggregate gate coverage is complete. | `pass` | Row-level bindings are non-empty; aggregate includes `TG-01`..`TG-07`. |
| `CAT-175-05` | Non-waiverable family alignment with Section 6.2 is preserved. | `pass` | `F-02`, `F-04`, `F-05`, and `F-07` use `waiver_eligibility = none`. |

## 5. Aggregate Gate Coverage (`TG-01`..`TG-07`)

| Gate ID | Bound test rows |
| --- | --- |
| `TG-01` | `TST-F01-SYNTAX-CORE-001`, `TST-F02-SEMA-SOUND-001`, `TST-F03-DIAG-FIXIT-001`, `TST-F04-ABI-INTEROP-001`, `TST-F05-RUNTIME-INTEROP-001`, `TST-F06-MODULE-BOUNDARY-001`, `TST-F07-SECURITY-ISOLATION-001`, `TST-F08-REPRO-REPLAY-001`, `TST-F09-MIGRATION-COMPAT-001`, `TST-F10-CLAIM-TRACE-001` |
| `TG-02` | `TST-F01-SYNTAX-CORE-001`, `TST-F02-SEMA-SOUND-001`, `TST-F03-DIAG-FIXIT-001`, `TST-F04-ABI-INTEROP-001`, `TST-F05-RUNTIME-INTEROP-001`, `TST-F06-MODULE-BOUNDARY-001`, `TST-F07-SECURITY-ISOLATION-001`, `TST-F08-REPRO-REPLAY-001`, `TST-F09-MIGRATION-COMPAT-001` |
| `TG-03` | `TST-F04-ABI-INTEROP-001`, `TST-F05-RUNTIME-INTEROP-001`, `TST-F06-MODULE-BOUNDARY-001`, `TST-F09-MIGRATION-COMPAT-001` |
| `TG-04` | `TST-F01-SYNTAX-CORE-001`, `TST-F02-SEMA-SOUND-001`, `TST-F03-DIAG-FIXIT-001`, `TST-F04-ABI-INTEROP-001`, `TST-F05-RUNTIME-INTEROP-001`, `TST-F06-MODULE-BOUNDARY-001`, `TST-F07-SECURITY-ISOLATION-001`, `TST-F08-REPRO-REPLAY-001`, `TST-F09-MIGRATION-COMPAT-001` |
| `TG-05` | `TST-F02-SEMA-SOUND-001`, `TST-F04-ABI-INTEROP-001`, `TST-F05-RUNTIME-INTEROP-001`, `TST-F07-SECURITY-ISOLATION-001`, `TST-F09-MIGRATION-COMPAT-001` |
| `TG-06` | `TST-F10-CLAIM-TRACE-001` |
| `TG-07` | `TST-F10-CLAIM-TRACE-001` |

## 6. Claim Traceability Starter Mapping

| claim_id | required_test_ids | profile_scope | note |
| --- | --- | --- | --- |
| `CLM-C08-SYNTAX` | `TST-F01-SYNTAX-CORE-001` | `core,strict` | Syntax surface and parser behavior claims. |
| `CLM-C08-SEMA` | `TST-F02-SEMA-SOUND-001` | `core,strict,strict-concurrency` | Static semantics and soundness claims. |
| `CLM-C08-DIAG` | `TST-F03-DIAG-FIXIT-001` | `core,strict` | Diagnostic and fix-it determinism claims. |
| `CLM-C08-ABI` | `TST-F04-ABI-INTEROP-001` | `core,strict` | ABI and metadata interoperability claims. |
| `CLM-C08-RUNTIME` | `TST-F05-RUNTIME-INTEROP-001` | `core,strict` | Runtime interoperability claims. |
| `CLM-C08-MODULE` | `TST-F06-MODULE-BOUNDARY-001` | `core,strict,strict-concurrency` | Cross-module/package claims. |
| `CLM-C08-SECURITY` | `TST-F07-SECURITY-ISOLATION-001` | `core,strict,strict-concurrency` | Security and isolation claims. |
| `CLM-C08-REPRO` | `TST-F08-REPRO-REPLAY-001` | `core,strict,strict-concurrency` | Reproducibility and replay claims. |
| `CLM-C08-MIGRATION` | `TST-F09-MIGRATION-COMPAT-001` | `core,strict` | Migration/deprecation compatibility claims. |
| `CLM-C08-TRACE` | `TST-F10-CLAIM-TRACE-001` | `core,strict,strict-concurrency` | Claim-to-test traceability completeness claims. |
