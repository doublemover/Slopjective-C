# Manifest Publish Verification Bundle (2026-02-23)

Policy: `MANPUB-POLICY-v0.11-B04`

## 1. Artifact Registry

- Workflow: `.github/workflows/objc3-manifest-publish.yml`
- Policy note: `spec/conformance/objc3_manifest_ci_publication_policy_v0.11.md`
- Index contract: `spec/conformance/objc3_manifest_publish_index_contract_v0.11.md`
- Publish record: `reports/conformance/manifests/publish/manifest_publish_record_v0.11.json`
- Index current pointer: `reports/conformance/manifests/publish/index/current.json`
- Index snapshot: `reports/conformance/manifests/publish/index/snapshots/main_20260223_7001.json`
- Retention dry run: `reports/conformance/manifests/publish/verification/retention_sweep_dry_run_2026-02-23.json`

## 2. Verification Disposition (`B04-VER-01`..`B04-VER-12`)

| Verification ID | Run ID | Result | Evidence log |
| --- | --- | --- | --- |
| `B04-VER-01` | `B04-RUN-20260223-7001` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-01_success.log` |
| `B04-VER-02` | `B04-RUN-20260223-7001` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-02_path_digest.log` |
| `B04-VER-03` | `B04-RUN-20260223-7002` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-03_idempotent.log` |
| `B04-VER-04` | `B04-RUN-20260223-7003` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-04_collision.log` |
| `B04-VER-05` | `B04-RUN-20260223-7004` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-05_signature.log` |
| `B04-VER-06` | `B04-RUN-20260223-7005` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-06_provenance.log` |
| `B04-VER-07` | `B04-RUN-20260223-7001` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-07_retention_metadata.log` |
| `B04-VER-08` | `B04-RUN-20260223-7006` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-08_unauthorized.log` |
| `B04-VER-09` | `B04-RUN-20260223-7007` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-09_ordering.log` |
| `B04-VER-10` | `B04-RUN-20260223-7008` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-10_post_publish.log` |
| `B04-VER-11` | `B04-RUN-20260223-7009` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-11_gate_failures.log` |
| `B04-VER-12` | `B04-RUN-20260223-7010` | `PASS` | `reports/conformance/manifests/publish/verification/run_B04-VER-12_retention_sweep.log` |

## 3. Deterministic `MANPUB-001`..`MANPUB-012` Emission Mapping

| Code | Verification linkage | Run ID | Evidence log |
| --- | --- | --- | --- |
| `MANPUB-001` | `B04-VER-11` | `B04-RUN-20260223-7009` | `reports/conformance/manifests/publish/verification/run_B04-VER-11_gate_failures.log` |
| `MANPUB-002` | `B04-VER-11` | `B04-RUN-20260223-7009` | `reports/conformance/manifests/publish/verification/run_B04-VER-11_gate_failures.log` |
| `MANPUB-003` | `B04-VER-11` | `B04-RUN-20260223-7009` | `reports/conformance/manifests/publish/verification/run_B04-VER-11_gate_failures.log` |
| `MANPUB-004` | `B04-VER-04` | `B04-RUN-20260223-7003` | `reports/conformance/manifests/publish/verification/run_B04-VER-04_collision.log` |
| `MANPUB-005` | `B04-VER-05` | `B04-RUN-20260223-7004` | `reports/conformance/manifests/publish/verification/run_B04-VER-05_signature.log` |
| `MANPUB-006` | `B04-VER-06` | `B04-RUN-20260223-7005` | `reports/conformance/manifests/publish/verification/run_B04-VER-06_provenance.log` |
| `MANPUB-007` | `B04-VER-10` | `B04-RUN-20260223-7011` | `reports/conformance/manifests/publish/verification/run_MANPUB-007_atomic_publish_failure.log` |
| `MANPUB-008` | `B04-VER-10` | `B04-RUN-20260223-7012` | `reports/conformance/manifests/publish/verification/run_MANPUB-008_index_update_failure.log` |
| `MANPUB-009` | `B04-VER-07` | `B04-RUN-20260223-7013` | `reports/conformance/manifests/publish/verification/run_MANPUB-009_retention_assignment_failure.log` |
| `MANPUB-010` | `B04-VER-08` | `B04-RUN-20260223-7006` | `reports/conformance/manifests/publish/verification/run_B04-VER-08_unauthorized.log` |
| `MANPUB-011` | `B04-VER-09` | `B04-RUN-20260223-7014` | `reports/conformance/manifests/publish/verification/run_MANPUB-011_ordering_drift_failure.log` |
| `MANPUB-012` | `B04-VER-10` | `B04-RUN-20260223-7008` | `reports/conformance/manifests/publish/verification/run_B04-VER-10_post_publish.log` |

## 4. Dependency Closeout Links (`#150`, `#151`, `#155`)

| Upstream issue | Owner | Revision/SHA | Decision date (UTC) | Revision-pinned closeout reference |
| --- | --- | --- | --- | --- |
| `#150` | Lane B generator owner | `4fb99be2a16ac2b9f05ad2b4f52084b8017dcb3e` | `2026-02-22` | `spec/planning/issue_150_manifest_generation_pipeline_plan.md` |
| `#151` | Lane B validation owner | `0f87108f1cde3fc8f4b6f0f12fcd9ebafe8627ab` | `2026-02-22` | `spec/planning/issue_151_manifest_validation_ci_plan.md` |
| `#155` | Lane B reproducibility owner | `321ac6d61f9a2b0d730183c1700145c9b5bd1f5e` | `2026-02-22` | `spec/planning/issue_155_manifest_reproducibility_checks_plan.md` |

All referenced logs in this bundle are in-repo and replayable from deterministic
run IDs listed above.
