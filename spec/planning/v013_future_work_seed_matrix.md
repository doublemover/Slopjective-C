# v0.13 Future Work Seed Matrix

Snapshot date: 2026-02-23
Contract anchor: `V013-TOOL-03`

## Seed Table

| Seed ID | Family | Worklane | Proposed GH issue title | Primary artifact targets | Depends on | Shard class | Acceptance gate ID |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `V013-SPEC-02` | `FAM-SPEC` | `WL-SPEC` | `[v0.13][spec] Establish native syntax baseline contracts` | `spec/IMPLEMENTATION_EXECUTION_ROADMAP.md` | `none` | `small` | `AC-V013-SPEC-02` |
| `V013-SPEC-01` | `FAM-SPEC` | `WL-SPEC` | `[v0.13][spec] Lock deterministic planning validation profile` | `spec/CONFORMANCE_PROFILE_CHECKLIST.md` | `none` | `small` | `AC-V013-SPEC-01` |
| `V013-TOOL-01` | `FAM-TOOL` | `WL-TOOL` | `[v0.13][tooling] Stabilize issue/task hygiene contracts` | `scripts/check_issue_checkbox_drift.py` | `none` | `small` | `AC-V013-TOOL-01` |
| `V013-TOOL-02` | `FAM-TOOL` | `WL-TOOL` | `[v0.13][tooling] Deterministic seed payload build path` | `scripts/generate_seed_issue_payloads.py` | `V013-SPEC-01` | `medium` | `AC-V013-TOOL-02` |
| `V013-TOOL-03` | `FAM-TOOL` | `WL-TOOL` | `[v0.13][tooling] Generate seed DAG and batch skeletons from matrix` | `scripts/generate_seed_batches.py`, `spec/planning/v013_seed_dependency_graph.json` | `V013-SPEC-02` | `medium` | `AC-V013-TOOL-03` |
| `V013-GOV-01` | `FAM-GOV` | `WL-GOV` | `[v0.13][governance] Publish initial board workflow contract` | `reports/reviews/v013_review_board_calendar.md` | `V013-SPEC-02` | `small` | `AC-V013-GOV-01` |
| `V013-GOV-02` | `FAM-GOV` | `WL-GOV` | `[v0.13][governance] Expand extension registry compat policy` | `tests/governance/registry_compat/README.md` | `V013-GOV-01` | `medium` | `AC-V013-GOV-02` |
| `V013-SPEC-03` | `FAM-SPEC` | `WL-SPEC` | `[v0.13][spec] Normalize abstract-machine sync ledger` | `reports/spec_sync/abstract_machine_audit_2026Q2.md` | `V013-TOOL-02` | `medium` | `AC-V013-SPEC-03` |
| `V013-DOC-01` | `FAM-TOOL` | `WL-TOOL` | `[v0.13][tooling] Harden seed generation diagnostics docs` | `docs/objc3c-native/src/library-api.md` | `V013-TOOL-03` | `small` | `AC-V013-DOC-01` |
| `V013-CONF-01` | `FAM-CONF` | `WL-CONF` | `[v0.13][conformance] Publish baseline release evidence schema` | `reports/conformance/release_evidence_contract.v013.json` | `V013-SPEC-03`, `V013-GOV-02` | `medium` | `AC-V013-CONF-01` |
| `V013-GOV-03` | `FAM-GOV` | `WL-GOV` | `[v0.13][governance] Lock cadence and quorum package` | `spec/governance/review_board_operating_model_v1.md` | `V013-GOV-02` | `small` | `AC-V013-GOV-03` |
| `V013-TOOL-04` | `FAM-TOOL` | `WL-TOOL` | `[v0.13][tooling] Refresh dispatch planning evidence packet` | `spec/planning/compiler/m134/m134_closeout_evidence_20260227.md` | `V013-DOC-01` | `medium` | `AC-V013-TOOL-04` |
| `V013-CONF-02` | `FAM-CONF` | `WL-CONF` | `[v0.13][conformance] Generate quality-gate decision rollup` | `reports/conformance/spec_open_issues_v013_summary.md` | `V013-CONF-01` | `small` | `AC-V013-CONF-02` |
| `V013-SPEC-04` | `FAM-SPEC` | `WL-SPEC` | `[v0.13][spec] Capture profile-gate delta package` | `spec/CONFORMANCE_PROFILE_CHECKLIST.md` | `V013-GOV-03`, `V013-TOOL-04` | `medium` | `AC-V013-SPEC-04` |
| `V013-GOV-04` | `FAM-GOV` | `WL-GOV` | `[v0.13][governance] Publish macro tabletop closeout controls` | `spec/governance/macro_security_incident_playbook_v1.md` | `V013-GOV-03` | `medium` | `AC-V013-GOV-04` |
| `V013-REL-01` | `FAM-REL` | `WL-REL` | `[v0.13][release] Assemble seed wave readiness package` | `spec/FUTURE_WORK_V013_BOOTSTRAP.md` | `V013-SPEC-04`, `V013-CONF-02` | `large` | `AC-V013-REL-01` |
| `V013-CONF-03` | `FAM-CONF` | `WL-CONF` | `[v0.13][conformance] Certify dispatch replay quality gates` | `reports/security/v013_macro_tabletop.md` | `V013-GOV-04` | `medium` | `AC-V013-CONF-03` |
| `V013-REL-02` | `FAM-REL` | `WL-REL` | `[v0.13][release] Integrate cross-lane closeout packet` | `spec/planning/compiler/m134/m134_closeout_evidence_20260227.md` | `V013-REL-01` | `large` | `AC-V013-REL-02` |
| `V013-CONF-04` | `FAM-CONF` | `WL-CONF` | `[v0.13][conformance] Finalize parity and reproducibility evidence` | `reports/conformance/release_evidence_contract.v013.json` | `V013-CONF-03` | `medium` | `AC-V013-CONF-04` |
| `V013-REL-03` | `FAM-REL` | `WL-REL` | `[v0.13][release] Publish signoff and evidence consolidation package` | `spec/FUTURE_WORK_V013_BOOTSTRAP.md` | `V013-REL-02`, `V013-CONF-04` | `large` | `AC-V013-REL-03` |

## Dependency Edge Table

| Edge ID | Predecessor | Successor | Type | Rationale |
| --- | --- | --- | --- | --- |
| `EDGE-V013-001` | `V013-SPEC-01` | `V013-TOOL-02` | `hard` | `Issue hygiene contract must be stable before payload synthesis.` |
| `EDGE-V013-002` | `V013-SPEC-02` | `V013-TOOL-03` | `hard` | `Matrix-driven DAG generation depends on baseline syntax contract.` |
| `EDGE-V013-003` | `V013-SPEC-02` | `V013-GOV-01` | `hard` | `Governance baseline follows spec baseline publication.` |
| `EDGE-V013-004` | `V013-GOV-01` | `V013-GOV-02` | `hard` | `Registry compatibility policy extends initial governance rules.` |
| `EDGE-V013-005` | `V013-TOOL-02` | `V013-SPEC-03` | `hard` | `Spec sync normalization consumes deterministic payload outputs.` |
| `EDGE-V013-006` | `V013-TOOL-03` | `V013-DOC-01` | `soft` | `Diagnostics docs are grounded in generated DAG contract.` |
| `EDGE-V013-007` | `V013-SPEC-03` | `V013-CONF-01` | `hard` | `Conformance schema requires normalized spec sync data.` |
| `EDGE-V013-008` | `V013-GOV-02` | `V013-CONF-01` | `hard` | `Conformance schema must map governance compatibility policy.` |
| `EDGE-V013-009` | `V013-GOV-02` | `V013-GOV-03` | `hard` | `Cadence package builds on established governance policy.` |
| `EDGE-V013-010` | `V013-DOC-01` | `V013-TOOL-04` | `soft` | `Dispatch evidence packet updates rely on diagnostics guidance.` |
| `EDGE-V013-011` | `V013-CONF-01` | `V013-CONF-02` | `hard` | `Quality-gate rollup depends on baseline evidence schema.` |
| `EDGE-V013-012` | `V013-GOV-03` | `V013-SPEC-04` | `hard` | `Profile-gate deltas require current governance cadence controls.` |
| `EDGE-V013-013` | `V013-TOOL-04` | `V013-SPEC-04` | `hard` | `Spec profile deltas depend on refreshed dispatch evidence.` |
| `EDGE-V013-014` | `V013-GOV-03` | `V013-GOV-04` | `hard` | `Macro tabletop controls extend quorum package outputs.` |
| `EDGE-V013-015` | `V013-SPEC-04` | `V013-REL-01` | `hard` | `Release readiness requires profile-gate delta completion.` |
| `EDGE-V013-016` | `V013-CONF-02` | `V013-REL-01` | `hard` | `Release readiness requires quality-gate rollup completion.` |
| `EDGE-V013-017` | `V013-GOV-04` | `V013-CONF-03` | `hard` | `Replay quality gates depend on macro tabletop closeout controls.` |
| `EDGE-V013-018` | `V013-REL-01` | `V013-REL-02` | `hard` | `Cross-lane packet integration follows readiness assembly.` |
| `EDGE-V013-019` | `V013-CONF-03` | `V013-CONF-04` | `hard` | `Final parity evidence depends on replay quality gate certification.` |
| `EDGE-V013-020` | `V013-REL-02` | `V013-REL-03` | `hard` | `Final signoff requires integrated closeout packet state.` |
| `EDGE-V013-021` | `V013-CONF-04` | `V013-REL-03` | `hard` | `Final signoff requires reproducibility evidence completion.` |

## Wave Eligibility Table

| Wave | Seeds eligible for execution (all hard predecessors satisfied) |
| --- | --- |
| `W0` | `V013-SPEC-02`, `V013-SPEC-01`, `V013-TOOL-01` |
| `W1` | `V013-TOOL-02`, `V013-TOOL-03`, `V013-GOV-01` |
| `W2` | `V013-GOV-02`, `V013-SPEC-03`, `V013-DOC-01` |
| `W3` | `V013-CONF-01`, `V013-GOV-03`, `V013-TOOL-04` |
| `W4` | `V013-CONF-02`, `V013-SPEC-04`, `V013-GOV-04` |
| `W5` | `V013-REL-01`, `V013-CONF-03` |
| `W6` | `V013-REL-02`, `V013-CONF-04` |
| `W7` | `V013-REL-03` |

## Batch Skeleton Table

| Batch ID | Class | Included seed IDs | Entry prerequisites | Exit signal |
| --- | --- | --- | --- | --- |
| `BATCH-V013-L-01` | `large` | `V013-SPEC-02`, `V013-SPEC-01`, `V013-TOOL-01`, `V013-TOOL-02`, `V013-TOOL-03`, `V013-GOV-01`, `V013-GOV-02`, `V013-SPEC-03`, `V013-DOC-01`, `V013-CONF-01`, `V013-GOV-03`, `V013-TOOL-04`, `V013-CONF-02`, `V013-SPEC-04`, `V013-GOV-04`, `V013-REL-01`, `V013-CONF-03`, `V013-REL-02`, `V013-CONF-04`, `V013-REL-03` | `V013-TOOL-03` matrix contract locked and lane evidence synchronized. | `AC-V013-REL-03` closeout packet published with deterministic replay evidence. |

## Priority Scoring Table

| Seed ID | CPI | DUV | RBV | ERC | ECP | DC | Priority score | Tier |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `V013-SPEC-02` | `5` | `5` | `4` | `4` | `5` | `1` | `90` | `Tier-0` |
| `V013-SPEC-01` | `5` | `4` | `4` | `3` | `4` | `1` | `84` | `Tier-0` |
| `V013-TOOL-01` | `4` | `4` | `3` | `3` | `4` | `2` | `76` | `Tier-1` |
| `V013-TOOL-02` | `4` | `4` | `4` | `3` | `4` | `2` | `78` | `Tier-1` |
| `V013-TOOL-03` | `5` | `5` | `4` | `3` | `5` | `2` | `88` | `Tier-0` |
| `V013-GOV-01` | `4` | `3` | `3` | `3` | `3` | `2` | `70` | `Tier-2` |
| `V013-GOV-02` | `4` | `4` | `4` | `3` | `4` | `2` | `79` | `Tier-1` |
| `V013-SPEC-03` | `4` | `4` | `4` | `4` | `4` | `2` | `80` | `Tier-1` |
| `V013-DOC-01` | `3` | `3` | `3` | `2` | `3` | `2` | `63` | `Tier-2` |
| `V013-CONF-01` | `4` | `4` | `4` | `4` | `4` | `2` | `81` | `Tier-1` |
| `V013-GOV-03` | `4` | `3` | `4` | `3` | `3` | `2` | `71` | `Tier-2` |
| `V013-TOOL-04` | `4` | `3` | `3` | `3` | `3` | `2` | `69` | `Tier-2` |
| `V013-CONF-02` | `4` | `4` | `3` | `3` | `4` | `2` | `77` | `Tier-1` |
| `V013-SPEC-04` | `5` | `4` | `4` | `4` | `5` | `2` | `86` | `Tier-0` |
| `V013-GOV-04` | `4` | `3` | `4` | `3` | `3` | `2` | `72` | `Tier-2` |
| `V013-REL-01` | `5` | `5` | `5` | `4` | `5` | `2` | `92` | `Tier-0` |
| `V013-CONF-03` | `4` | `4` | `4` | `3` | `4` | `2` | `78` | `Tier-1` |
| `V013-REL-02` | `5` | `4` | `5` | `4` | `5` | `2` | `89` | `Tier-0` |
| `V013-CONF-04` | `4` | `3` | `4` | `3` | `4` | `2` | `73` | `Tier-2` |
| `V013-REL-03` | `5` | `5` | `5` | `5` | `5` | `2` | `95` | `Tier-0` |
