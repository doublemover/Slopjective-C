# Conformance Evidence Dashboard v0.11

Machine status feed: `reports/conformance/dashboard_v011.status.json`  
Status schema: `schemas/objc3-conformance-dashboard-status-v1.schema.json`

## DB-SEC-01 Release Header and Generation Metadata

| Field | Value |
| --- | --- |
| `release_label` | `v0.11` |
| `release_id` | `20260223-issue789-lanea-013` |
| `dashboard_version` | `0.11.0` |
| `generated_at` | `2026-02-23T20:05:00Z` |
| `source_revision` | `fc34c0a0c744053782d225a875a00d3f89ce41cb` |
| `wave` | `W1` |
| `batch_id` | `BATCH-20260223-11S` |
| `issue` | `#789` |
| `overall_status` | `fail` |
| `schema_id` | `objc3-conformance-dashboard-status/v1` |

## DB-SEC-02 Profile Verdict Matrix

| Profile | Verdict | B-04 | B-10 | B-11 | B-12 | Blockers |
| --- | --- | --- | --- | --- | --- | --- |
| `core` | `pass` | `pass` | `pass` | `pass` | `pass` | None |
| `strict` | `incomplete` | `pass` | `pass` | `pass` | `stale` | `BLK-B12-STALE-001` |
| `strict-concurrency` | `fail` | `pass` | `pass` | `fail` | `stale` | `BLK-B11-RESULT-FAIL-001`, `BLK-B12-STALE-001` |
| `strict-system` | `blocked` | `pass` | `blocked` | `pass` | `stale` | `BLK-B10-PROFILE-GAP-001`, `BLK-B12-STALE-001` |

Rollup precedence used: `fail` > `blocked` > `incomplete` > `pass`.

## DB-SEC-03 Dependency Health Board

| Dependency | Status | Refreshed At | Stale After (h) | Failure Codes | Artifact Refs |
| --- | --- | --- | --- | --- | --- |
| `B-04` | `pass` | `2026-02-23T18:10:00Z` | `24` | None | `ART-B04-RUNTIME-MANIFEST`, `ART-B04-ABI-MANIFEST`, `ART-B04-EVIDENCE-INDEX` |
| `B-10` | `blocked` | `2026-02-23T18:05:00Z` | `24` | `DASH-B10-PROFILE-GAP` | `ART-B10-CAPABILITY-CONTRACT` |
| `B-11` | `fail` | `2026-02-23T17:50:00Z` | `24` | `DASH-B11-RESULT-FAIL` | `ART-B11-SEED-TRACE`, `ART-B11-SEED-MANIFEST` |
| `B-12` | `stale` | `2026-02-22T08:30:00Z` | `24` | `DASH-B12-EXCEPTION-STALE` | `ART-B12-DRIFT-CONTRACT` |

## DB-SEC-04 Artifact Linkage Ledger

| Artifact ID | Dependency | Profile Scope | Path | SHA-256 | Generated At | Source Revision | Validation | Issue Ref |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `ART-B04-RUNTIME-MANIFEST` | `B-04` | `all` | `reports/conformance/manifests/objc3-runtime-2025Q4.manifest.json` | `84229d46ab6ab735c25c9b70ff697f72bfdfe3e6207bed594abb029a16385d59` | `2026-02-23T18:10:00Z` | `7f3c2b1f4f2f0df1f2e35a5d7ef6f71f5ebbf3a1` | `valid` | `#161/runtime-manifest` |
| `ART-B04-ABI-MANIFEST` | `B-04` | `all` | `reports/conformance/manifests/objc3-abi-2025Q4.example.json` | `200d84a4d5e373e9c55fed5f96d67c78c249e36554e8cd7b53940c3b73ac9f12` | `2026-02-23T18:10:00Z` | `7f3c2b1f4f2f0df1f2e35a5d7ef6f71f5ebbf3a1` | `valid` | `#161/abi-manifest` |
| `ART-B04-EVIDENCE-INDEX` | `B-04` | `all` | `reports/conformance/evidence-index.v0.11.sample.json` | `935b57de908ca30437464dd24e1e1f99d0e381b14518460237d623dea6094384` | `2026-02-23T18:10:00Z` | `7f3c2b1f4f2f0df1f2e35a5d7ef6f71f5ebbf3a1` | `valid` | `#161/evidence-index` |
| `ART-B10-CAPABILITY-CONTRACT` | `B-10` | `all` | `spec/planning/issue_140_capability_report_outputs_plan.md` | `0dd6b59fe742de3d55a5179888fed6f46042ce128e05bc5b94fcd52cf74a76da` | `2026-02-23T18:05:00Z` | `7f3c2b1f4f2f0df1f2e35a5d7ef6f71f5ebbf3a1` | `valid` | `#140/capability-contract` |
| `ART-B11-SEED-TRACE` | `B-11` | `all` | `tests/conformance/spec_open_issues/P0-P3-seed_traceability.csv` | `0ed18120e8579be332869ae2e20f481c98c641c8d423a33180239791fe031da7` | `2026-02-23T17:50:00Z` | `7f3c2b1f4f2f0df1f2e35a5d7ef6f71f5ebbf3a1` | `valid` | `#167/seed-traceability` |
| `ART-B11-SEED-MANIFEST` | `B-11` | `strict-concurrency` | `tests/conformance/spec_open_issues/manifest.json` | `7ec545133a1dcf8ff61a53046f73c09891f4ed726b8bdb7d9cac21d1ec47385b` | `2026-02-23T17:50:00Z` | `7f3c2b1f4f2f0df1f2e35a5d7ef6f71f5ebbf3a1` | `valid` | `#167/seed-manifest` |
| `ART-B12-DRIFT-CONTRACT` | `B-12` | `all` | `spec/planning/issue_158_abstract_machine_drift_lint_plan.md` | `e8d4d86265c1ac1e079933447f0cc50e7f47a29218789f9d7419689d97fa70f6` | `2026-02-22T08:30:00Z` | `7f3c2b1f4f2f0df1f2e35a5d7ef6f71f5ebbf3a1` | `valid` | `#158/drift-lint` |
| `ART-B13-DASHBOARD-MD` | `B-13` | `all` | `reports/conformance/dashboard_v011.md` | `6fb9ab4839d6d145d5156cd6fe826511206bcaf309160abb6d46659ba6b2b11b` | `2026-02-23T20:05:00Z` | `fc34c0a0c744053782d225a875a00d3f89ce41cb` | `valid` | `#173/dashboard-markdown` |
| `ART-B13-DASHBOARD-STATUS` | `B-13` | `all` | `reports/conformance/dashboard_v011.status.json` | `eed777c3e4e8324712230b7067ebedd97d06667208c99c68eb4d74e59ac30c68` | `2026-02-23T20:05:00Z` | `fc34c0a0c744053782d225a875a00d3f89ce41cb` | `valid` | `#173/dashboard-status` |
| `ART-B13-DASHBOARD-SCHEMA` | `B-13` | `all` | `schemas/objc3-conformance-dashboard-status-v1.schema.json` | `5188d2a6d4d6b892367fedec9302f15df12389739201b056eb6d95ca1d52da6a` | `2026-02-23T18:30:00Z` | `7f3c2b1f4f2f0df1f2e35a5d7ef6f71f5ebbf3a1` | `valid` | `#173/dashboard-schema` |
| `ART-EV01-EVIDENCE-PACK` | `B-13` | `all` | `spec/planning/v013_ev01_ev05_evidence_pack_package.md` | `e6b235f617965dd26ab438b0c86f68cde94c82d0bc1f7ad44e491e4b85d33279` | `2026-02-23T20:05:00Z` | `fc34c0a0c744053782d225a875a00d3f89ce41cb` | `valid` | `#789/evidence-pack` |
| `ART-EV02-DASHBOARD-MD` | `B-13` | `all` | `reports/conformance/dashboard_v011.md` | `6fb9ab4839d6d145d5156cd6fe826511206bcaf309160abb6d46659ba6b2b11b` | `2026-02-23T20:05:00Z` | `fc34c0a0c744053782d225a875a00d3f89ce41cb` | `valid` | `#789/dashboard` |
| `ART-EV03-DASHBOARD-STATUS` | `B-13` | `all` | `reports/conformance/dashboard_v011.status.json` | `eed777c3e4e8324712230b7067ebedd97d06667208c99c68eb4d74e59ac30c68` | `2026-02-23T20:05:00Z` | `fc34c0a0c744053782d225a875a00d3f89ce41cb` | `valid` | `#789/dashboard-status` |
| `ART-EV04-RERUN-DIGEST` | `B-13` | `all` | `reports/conformance/reproducibility/v011_rerun_digest_report.md` | `29e05965becc96ab94b86acebab9b4a0150ae3ebfedf107e12af0bc9ea20fecd` | `2026-02-23T20:05:00Z` | `fc34c0a0c744053782d225a875a00d3f89ce41cb` | `valid` | `#789/rerun-digest` |
| `ART-EV05-GATE-HANDOFF-MAP` | `B-13` | `all` | `spec/planning/v013_ev01_ev05_evidence_pack_package.md` | `e6b235f617965dd26ab438b0c86f68cde94c82d0bc1f7ad44e491e4b85d33279` | `2026-02-23T20:05:00Z` | `fc34c0a0c744053782d225a875a00d3f89ce41cb` | `valid` | `#789/gate-handoff` |

## DB-SEC-05 Open Blockers and Failure Taxonomy

| Blocker ID | Severity | State | Taxonomy | Dependency | Profile(s) | Failure Code | Owner |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `BLK-B10-PROFILE-GAP-001` | `high` | `open` | `dependency-ingest` | `B-10` | `strict-system` | `DASH-B10-PROFILE-GAP` | `lane-b` |
| `BLK-B11-RESULT-FAIL-001` | `critical` | `open` | `test-failure` | `B-11` | `strict-concurrency` | `DASH-B11-RESULT-FAIL` | `lane-b` |
| `BLK-B12-STALE-001` | `high` | `open` | `staleness` | `B-12` | `strict`, `strict-concurrency`, `strict-system` | `DASH-B12-EXCEPTION-STALE` | `lane-b` |

## DB-SEC-06 Refresh and Staleness Telemetry

| Field | Value |
| --- | --- |
| `trigger` | `scheduled` |
| `last_successful_refresh` | `2026-02-23T18:15:00Z` |
| `next_scheduled_refresh` | `2026-02-24T06:00:00Z` |
| `merge_latency_target_minutes` | `15` |
| `scheduled_latency_target_minutes` | `30` |
| `rc_fast_refresh_hours` | `4` |
| `stale_dependency_ids` | `B-12` |
| `missed_scheduled_refreshes` | `0` |
| `escalation_state` | `none` |

## DB-SEC-07 Change History

| Snapshot ID | Previous Snapshot | Change Kind | Changed At | Summary |
| --- | --- | --- | --- | --- |
| `20260223-issue789-lanea-013` | `20260223-issue713-lanea-012` | `evidence-pack-refresh` | `2026-02-23T20:05:00Z` | `Rebound EV-01..EV-05 metadata to issue #789 for tranche BATCH-20260223-11S while preserving AC-V013-CONF-01 and downstream handoff mappings.` |
| `20260223-issue713-lanea-012` | `20260223-issue173-laneb-011` | `evidence-pack-refresh` | `2026-02-23T19:45:00Z` | `Published EV-01..EV-05 evidence pack linkage for downstream CON-02/REL-01 consumers.` |
| `20260223-issue173-laneb-011` | `20260223-issue173-laneb-010` | `status-changed` | `2026-02-23T18:20:00Z` | `strict-concurrency moved to fail due required-fail seed mismatch (B-11).` |
| `20260223-issue173-laneb-010` | `20260222-issue173-laneb-009` | `blocker-changed` | `2026-02-23T14:00:00Z` | `Added B-12 stale blocker after freshness threshold breach.` |

## DB-SEC-08 EV-01..EV-05 Evidence Pack Mapping

| EV ID | Artifact ID | Path | Downstream consumer |
| --- | --- | --- | --- |
| `EV-01` | `ART-EV01-EVIDENCE-PACK` | `spec/planning/v013_ev01_ev05_evidence_pack_package.md` | `V013-CONF-02` |
| `EV-02` | `ART-EV02-DASHBOARD-MD` | `reports/conformance/dashboard_v011.md` | `V013-CONF-02`, `V013-REL-01` |
| `EV-03` | `ART-EV03-DASHBOARD-STATUS` | `reports/conformance/dashboard_v011.status.json` | `V013-CONF-02`, `V013-REL-01` |
| `EV-04` | `ART-EV04-RERUN-DIGEST` | `reports/conformance/reproducibility/v011_rerun_digest_report.md` | `V013-CONF-02`, `V013-REL-01` |
| `EV-05` | `ART-EV05-GATE-HANDOFF-MAP` | `spec/planning/v013_ev01_ev05_evidence_pack_package.md` | `V013-CONF-02`, `V013-REL-01` |
