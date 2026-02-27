# Objective-C 3.0 - Implementation Execution Roadmap {#impl-roadmap}

_Working draft v0.11 - last updated 2026-02-23_

This roadmap tracks thirty-three completed execution phases:

- Phase 1 completed: roadmap and Section E closure (`#1` through `#110`).
- Phase 2 completed: v0.11 follow-on execution (`#111` through `#133`).
- Phase 3 completed: FW Wave 1 execution (`#134` through `#142`).
- Phase 4 completed: FW Wave 2 execution (`#143` through `#148`).
- Phase 5 completed: FW Wave 3 execution (`#149` through `#154`).
- Phase 6 completed: FW Wave 4 execution (`#155` through `#160`).
- Phase 7 completed: FW Wave 5 execution (`#161` through `#166`).
- Phase 8 completed: FW Wave 6 execution (`#167` through `#172`).
- Phase 9 completed: FW Wave 7 execution (`#173` through `#178`).
- Phase 10 completed: FW Wave 8 execution (`#179` through `#184`).
- Phase 11 completed: FW Wave 9 execution (`#185` through `#192`).
- Phase 12 completed: seeded unchecked-task execution (`#193` through `#702`).
- Phase 13 completed: v0.13 foundation planning (`#703`, `#704`, `#706`, `#707`).
- Phase 14 completed: v0.13 seed wave W0 closure (`#708`, `#709`, `#710`, `#711`, `#712`).
- Phase 15 completed: v0.13 seed wave W1 closure (`#713`, `#714`, `#715`, `#716`, `#717`).
- Phase 16 completed: v0.13 seed wave W1B closure (`#718`, `#719`, `#720`, `#721`).
- Phase 17 completed: v0.13 seed wave W2 closure (`#722`, `#723`, `#725`, `#726`).
- Phase 18 completed: v0.13 seed wave W3 closure (`#727`, `#728`, `#729`, `#730`).
- Phase 19 completed: v0.13 automation hardening W1 closure (`#731`, `#732`, `#733`, `#734`).
- Phase 20 completed: v0.13 automation hardening W2 closure (`#735`, `#736`, `#737`, `#738`, `#739`).
- Phase 21 completed: v0.13 automation hardening W3 closure (`#740`).
- Phase 22 completed: v0.13 execution backlog tranche closeout (`#741`, `#742`, `#743`, `#744`).
- Phase 23 completed: v0.13 post-closeout control-plane sync closure (`#745`, `#746`, `#749`, `#750`).
- Phase 24 completed: v0.13 control-plane continuity W1 closure (`#751`, `#752`, `#753`, `#754`).
- Phase 25 completed: v0.13 activation tooling W1 closure (`#755`, `#756`, `#757`, `#758`).
- Phase 26 completed: v0.13 activation tooling W2 closure (`#759`, `#760`, `#761`, `#762`).
- Phase 27 completed: v0.13 catalog integrity W1 closure (`#763`, `#764`, `#765`, `#766`).
- Phase 28 completed: v0.13 catalog integrity W2 closure (`#767`, `#768`, `#769`, `#770`).
- Phase 29 completed: v0.13 catalog integrity W3 closure (`#771`, `#772`, `#773`, `#774`).
- Phase 30 completed: v0.13 forward scope activation W1 closure (`#775`, `#776`, `#777`, `#778`).
- Phase 31 completed: v0.13 seed wave W0 reseed 1 closure (`#779`, `#780`, `#781`, `#782`, `#783`).
- Phase 32 completed: v0.13 seed wave W1 reseed 1 closure (`#784`, `#785`, `#786`, `#787`, `#788`).
- Phase 33 completed: v0.13 seed wave W1/W2 reseed 2 closure (`#789`, `#790`, `#791`, `#792`, `#793`).

## IR.1 Scope {#impl-roadmap-1}

Closeout scope reflected in this phase:

- Automation hardening foundations are complete through W2
  (`BATCH-20260223-11G`) for `HB-02`, `HB-04`, `HB-05`, `HB-07`, and `HB-08`.
- Automation hardening W3 (`BATCH-20260223-11H`) is complete with closure of
  `#740` (`HB-10`), finishing the hardening backlog (`HB-01`..`HB-10`).
- Execution backlog tranche closeout is complete with lane issues
  `#741`..`#744` closed and milestone `#21` closed.
- Post-closeout control-plane sync is complete under `BATCH-20260223-11I`
  (issues `#745`, `#746`, `#749`, `#750`) with milestone `#22` closed and
  zero-open baseline archived.
- Control-plane continuity W1 is complete under `BATCH-20260223-11J`
  (issues `#751`, `#752`, `#753`, `#754`) with milestone `#23` closed and
  next-activation queue packaging published.
- Activation tooling W1 is complete under `BATCH-20260223-11K`
  (issues `#755`, `#756`, `#757`, `#758`) with milestone `#24` closed and
  trigger-check automation integrated into tooling/tests/workflow/docs.
- Activation tooling W2 is complete under `BATCH-20260223-11L`
  (issues `#759`, `#760`, `#761`, `#762`) with milestone `#25` closed and
  T4 governance-overlay semantics integrated end-to-end.
- Catalog integrity W1 is complete under `BATCH-20260223-11M`
  (issues `#763`, `#764`, `#765`, `#766`) with milestone `#26` closed,
  deterministic status-integrity diagnostics wired into tooling/CI, and strict
  gate mode available behind `CATALOG_STATUS_STRICT=1`.
- Catalog integrity W2 is complete under `BATCH-20260223-11N`
  (issues `#767`, `#768`, `#769`, `#770`) with milestone `#27` closed, zero
  missing-status rows in the catalog, and strict gate defaults promoted in
  package/workflow tooling.
- Catalog integrity W3 is complete under `BATCH-20260223-11O`
  (issues `#771`, `#772`, `#773`, `#774`) with milestone `#28` closed,
  `execution_status_rationale` and `execution_status_evidence_refs` backfilled
  across all catalog rows, and metadata completeness checks integrated in
  package/workflow hygiene paths.
- Forward scope activation W1 is complete under `BATCH-20260223-11P`
  (issues `#775`, `#776`, `#777`, `#778`) with milestone `#29` closed,
  post-idle scope reseed packages published, deterministic lane-capacity
  controls and activation reopen playbook integrated, and seed-to-issue
  payload tooling validated.
- Seed wave W0 reseed 1 is complete under `BATCH-20260223-11Q`
  (issues `#779`, `#780`, `#781`, `#782`, `#783`) with milestone `#30`
  closed and all five W0 seed rows merged with validation evidence.
- Seed wave W1 reseed 1 is complete under `BATCH-20260223-11R`
  (issues `#784`, `#785`, `#786`, `#787`, `#788`) with milestone `#31`
  closed and all five W1 seed rows merged with validation evidence.
- Seed wave W1/W2 reseed 2 is complete under `BATCH-20260223-11S`
  (issues `#789`, `#790`, `#791`, `#792`, `#793`) with milestone `#32`
  closed and mixed-wave reseed outputs merged with validation evidence.

## IR.2 Active issue bands {#impl-roadmap-2}

- Lane A: `0` open issues (issue `#789` closed).
- Lane B: `0` open issues (issue `#790` closed).
- Lane C: `0` open issues (issue `#791` closed).
- Lane D: `0` open issues (issue `#792` closed).
- Integrator lane: `0` open issues (issue `#793` closed).
- W3 hardening lane: `0` open issues (milestone closed).
- Execution tranche lane set (`#741`..`#744`): `0` open issues (milestone
  `#21` closed).
- Post-closeout sync lane set (`#745`, `#746`, `#749`, `#750`): `0` open
  issues (milestone `#22` closed).
- Continuity W1 lane set (`#751`, `#752`, `#753`, `#754`): `0` open issues
  (milestone `#23` closed).
- Activation tooling W1 lane set (`#755`, `#756`, `#757`, `#758`): `0` open
  issues (milestone `#24` closed).
- Activation tooling W2 lane set (`#759`, `#760`, `#761`, `#762`): `0` open
  issues (milestone `#25` closed).
- Catalog integrity W1 lane set (`#763`, `#764`, `#765`, `#766`): `0` open
  issues (milestone `#26` closed).
- Catalog integrity W2 lane set (`#767`, `#768`, `#769`, `#770`): `0` open
  issues (milestone `#27` closed).
- Catalog integrity W3 lane set (`#771`, `#772`, `#773`, `#774`): `0` open
  issues (milestone `#28` closed).
- Forward scope activation W1 lane set (`#775`, `#776`, `#777`, `#778`): `0`
  open issues (milestone `#29` closed).
- Seed wave W0 reseed 1 lane set (`#779`, `#780`, `#781`, `#782`, `#783`):
  `0` open issues (milestone `#30` closed).
- Seed wave W1 reseed 1 lane set (`#784`, `#785`, `#786`, `#787`, `#788`):
  `0` open issues (milestone `#31` closed).
- Seed wave W1/W2 reseed 2 lane set (`#789`, `#790`, `#791`, `#792`, `#793`):
  `0` open issues (milestone `#32` closed).

## IR.3 Workpack and planning references {#impl-roadmap-3}

- Core/Strict historical workpack: `tests/conformance/workpacks/e3_core_strict_workpack.md`
- Concurrency/System/Optional historical workpack: `tests/conformance/workpacks/e3_concurrency_system_opt_workpack.md`
- Active microtask backlog (generated from open GH issues): `spec/EXECUTION_MICROTASK_BACKLOG.md`
- Seeded task review catalog (`SPT-0001`..`SPT-0510`): `spec/planning/remaining_task_review_catalog.md`
- Seeded task review catalog JSON: `spec/planning/remaining_task_review_catalog.json`
- Wave 1 shard plan and ownership model: `spec/planning/v012_wave1_parallel_execution_plan.md`
- Wave 2 shard plan and ownership model: `spec/planning/v012_wave2_parallel_execution_plan.md`
- Wave 3 shard plan and ownership model: `spec/planning/v012_wave3_parallel_execution_plan.md`
- Wave 4 shard plan and ownership model: `spec/planning/v012_wave4_parallel_execution_plan.md`
- Wave 5 shard plan and ownership model: `spec/planning/v012_wave5_parallel_execution_plan.md`
- Wave 6 shard plan and ownership model: `spec/planning/v012_wave6_parallel_execution_plan.md`
- Wave 7 shard plan and ownership model: `spec/planning/v012_wave7_parallel_execution_plan.md`
- Wave 15 completed batch manifest: `spec/planning/v012_wave15_batch_manifest_20260223.md`
- Wave 16 candidate shards + completed manifest:
  - `spec/planning/v012_wave16_candidate_shards_20260223.md`
  - `spec/planning/v012_wave16_batch_manifest_20260223.md`
- v0.13 Wave 1 completed manifest:
  - `spec/planning/v013_wave1_batch_manifest_20260223.md`
- v0.13 Wave 2 W0 completed manifest:
  - `spec/planning/v013_wave2_w0_batch_manifest_20260223.md`
- v0.13 Wave 2 W1 completed manifest:
  - `spec/planning/v013_wave2_w1_batch_manifest_20260223.md`
- v0.13 Wave 2 W1B completed manifest:
  - `spec/planning/v013_wave2_w1b_batch_manifest_20260223.md`
- v0.13 Wave 2 W2 completed manifest + closeout templates:
  - `spec/planning/v013_wave2_w2_batch_manifest_20260223.md`
  - `spec/planning/v013_wave2_w2_closeout_comment_templates.md`
- v0.13 Wave 2 W3 completed manifest + closeout templates:
  - `spec/planning/v013_wave2_w3_batch_manifest_20260223.md`
  - `spec/planning/v013_wave2_w3_closeout_comment_templates.md`
- v0.13 hardening W1 completed manifest + closeout templates:
  - `spec/planning/v013_hardening_w1_batch_manifest_20260223.md`
  - `spec/planning/v013_hardening_w1_closeout_comment_templates.md`
- v0.13 hardening W2 completed manifest + closeout templates:
  - `spec/planning/v013_hardening_w2_batch_manifest_20260223.md`
  - `spec/planning/v013_hardening_w2_closeout_comment_templates.md`
- v0.13 hardening W3 completed manifest + closeout templates:
  - `spec/planning/v013_hardening_w3_batch_manifest_20260223.md`
  - `spec/planning/v013_hardening_w3_closeout_comment_templates.md`
- v0.13 execution tranche closeout artifacts:
  - `spec/planning/v013_dq01_execution_batch_manifest_20260223.md`
  - `spec/planning/v013_execution_tranche_20260223.md`
- v0.13 post-closeout sync manifest:
  - `spec/planning/v013_post_closeout_sync_batch_manifest_20260223.md`
- v0.13 continuity W1 manifest:
  - `spec/planning/v013_control_plane_continuity_w1_batch_manifest_20260223.md`
- v0.13 continuity W1 final metrics evidence:
  - `spec/planning/evidence/integrator/v013_continuity_w1_final_metrics_sync_20260223.md`
- v0.13 activation tooling W1 manifest:
  - `spec/planning/v013_activation_tooling_w1_batch_manifest_20260223.md`
- v0.13 activation tooling W1 final metrics evidence:
  - `spec/planning/evidence/integrator/v013_activation_tooling_w1_final_metrics_sync_20260223.md`
- v0.13 activation tooling W2 manifest:
  - `spec/planning/v013_activation_tooling_w2_batch_manifest_20260223.md`
- v0.13 activation tooling W2 final metrics evidence:
  - `spec/planning/evidence/integrator/v013_activation_tooling_w2_final_metrics_sync_20260223.md`
- v0.13 catalog integrity W1 manifest:
  - `spec/planning/v013_catalog_integrity_w1_batch_manifest_20260223.md`
- v0.13 catalog integrity W1 preflight metrics evidence:
  - `spec/planning/evidence/integrator/v013_catalog_integrity_w1_preflight_metrics_20260223.md`
- v0.13 catalog integrity W1 final metrics evidence:
  - `spec/planning/evidence/integrator/v013_catalog_integrity_w1_final_metrics_sync_20260223.md`
- v0.13 catalog integrity W2 manifest:
  - `spec/planning/v013_catalog_integrity_w2_batch_manifest_20260223.md`
- v0.13 catalog integrity W2 preflight metrics evidence:
  - `spec/planning/evidence/integrator/v013_catalog_integrity_w2_preflight_metrics_20260223.md`
- v0.13 catalog integrity W2 final metrics evidence:
  - `spec/planning/evidence/integrator/v013_catalog_integrity_w2_final_metrics_sync_20260223.md`
- v0.13 catalog integrity W3 manifest:
  - `spec/planning/v013_catalog_integrity_w3_batch_manifest_20260223.md`
- v0.13 catalog integrity W3 preflight metrics evidence:
  - `spec/planning/evidence/integrator/v013_catalog_integrity_w3_preflight_metrics_20260223.md`
- v0.13 catalog integrity W3 final metrics evidence:
  - `spec/planning/evidence/integrator/v013_catalog_integrity_w3_final_metrics_sync_20260223.md`
- v0.13 forward scope activation W1 manifest:
  - `spec/planning/v013_forward_scope_activation_w1_batch_manifest_20260223.md`
- v0.13 forward scope activation W1 closeout templates:
  - `spec/planning/v013_forward_scope_activation_w1_closeout_comment_templates.md`
- v0.13 forward scope activation W1 preflight metrics evidence:
  - `spec/planning/evidence/integrator/v013_forward_scope_activation_w1_preflight_metrics_20260223.md`
- v0.13 forward scope activation W1 final metrics evidence:
  - `spec/planning/evidence/integrator/v013_forward_scope_activation_w1_final_metrics_sync_20260223.md`
- v0.13 seed wave W0 reseed 1 manifest:
  - `spec/planning/v013_seed_wave_w0_reseed1_batch_manifest_20260223.md`
- v0.13 seed wave W0 reseed 1 closeout templates:
  - `spec/planning/v013_seed_wave_w0_reseed1_closeout_comment_templates.md`
- v0.13 seed wave W0 reseed 1 preflight metrics evidence:
  - `spec/planning/evidence/integrator/v013_seed_wave_w0_reseed1_preflight_metrics_20260223.md`
- v0.13 seed wave W0 reseed 1 final metrics evidence:
  - `spec/planning/evidence/integrator/v013_seed_wave_w0_reseed1_final_metrics_sync_20260223.md`
- v0.13 seed wave W1 reseed 1 manifest:
  - `spec/planning/v013_seed_wave_w1_reseed1_batch_manifest_20260223.md`
- v0.13 seed wave W1 reseed 1 preflight metrics evidence:
  - `spec/planning/evidence/integrator/v013_seed_wave_w1_reseed1_preflight_metrics_20260223.md`
- v0.13 seed wave W1 reseed 1 final metrics evidence:
  - `spec/planning/evidence/integrator/v013_seed_wave_w1_reseed1_final_metrics_sync_20260223.md`
- v0.13 seed wave W1/W2 reseed 2 manifest:
  - `spec/planning/v013_seed_wave_w1w2_reseed2_batch_manifest_20260223.md`
- v0.13 seed wave W1/W2 reseed 2 preflight metrics evidence:
  - `spec/planning/evidence/integrator/v013_seed_wave_w1w2_reseed2_preflight_metrics_20260223.md`
- v0.13 seed wave W1/W2 reseed 2 closeout templates:
  - `spec/planning/v013_seed_wave_w1w2_reseed2_closeout_comment_templates.md`
- v0.13 seed wave W1/W2 reseed 2 final metrics evidence:
  - `spec/planning/evidence/integrator/v013_seed_wave_w1w2_reseed2_final_metrics_sync_20260223.md`
- v0.13 next-dispatch queue scaffold:
  - `spec/planning/v013_wave2_next_dispatch_queue_20260223.md`
- v0.13 next-activation queue package:
  - `spec/planning/v013_next_activation_queue_20260223.md`
- Activation-trigger checker tooling:
  - `scripts/check_activation_triggers.py`
  - `tests/tooling/test_check_activation_triggers.py`
- v0.13 hardening next-dispatch queue scaffold:
  - `spec/planning/v013_hardening_next_dispatch_queue_20260223.md`
- v0.13 milestone archive reconciliation package:
  - `spec/planning/v013_milestone_archive_reconciliation.md`
- v0.13 release-chain closeout artifacts:
  - `spec/planning/future_work_v011_carryover.md`
  - `spec/FUTURE_WORK_V013_BOOTSTRAP.md`
  - `spec/planning/v013_kickoff_checklist.md`
  - `spec/planning/v013_rel03_signoff_consolidation_package.md`
- Checkbox-to-issue closeout helper: `scripts/close_spt_issues_from_checkboxes.py`
- Expanded v0.11 future-work task inventory: `spec/FUTURE_WORK_V011.md`
- Runtime manifest guidance and sample artifacts (`#115`):
  - `spec/conformance/objc3-runtime-2025Q4_manifest_validation.md`
  - `schemas/objc3-runtime-2025Q4.manifest.schema.json`
  - `reports/conformance/manifests/objc3-runtime-2025Q4.manifest.json`
- ABI manifest guidance and sample artifacts (`#116`):
  - `spec/conformance/objc3_abi_manifest_validation_v0.11_A02.md`
  - `schemas/objc3-abi-2025Q4.schema.json`
  - `reports/conformance/manifests/objc3-abi-2025Q4.example.json`
- Abstract-machine synchronization protocol (`#117`):
  - `spec/process/ABSTRACT_MACHINE_SYNC_PROTOCOL.md`
- Macro/derive extension governance policy (`#121`):
  - `spec/governance/MACRO_DERIVE_EXTENSION_GOVERNANCE.md`
- Part 3 decision closure package (`#118`, `#119`, `#120`):
  - `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`
  - `spec/DECISIONS_LOG.md`
  - `spec/planning/issue_118_value_optional_spelling_decision_package.md`
  - `spec/planning/issue_119_generic_mangling_policy_decision_package.md`
  - `spec/planning/issue_120_reification_scope_decision_package.md`
- FW Wave 1 issue seeding source:
  - `spec/FUTURE_WORK_V011.md`
- FW Wave 2 planning artifacts (`#143`..`#147`):
  - `spec/planning/issue_143_abstract_machine_audit_plan.md`
  - `spec/planning/issue_144_profile_gate_matrix_package.md`
  - `spec/planning/issue_145_edge_case_examples_package.md`
  - `spec/planning/issue_146_capability_namespace_policy_package.md`
  - `spec/planning/future_work_v011_milestones.md`
- FW Wave 3 planning artifacts (`#149`..`#153`):
  - `spec/planning/issue_149_lane_a_consolidated_closure_bundle.md`
  - `spec/planning/issue_150_manifest_generation_pipeline_plan.md`
  - `spec/planning/issue_151_manifest_validation_ci_plan.md`
  - `spec/planning/issue_152_extension_proposal_intake_template_package.md`
  - `spec/planning/issue_153_extension_review_rubric_package.md`
- FW Wave 4 planning artifacts (`#155`..`#159`):
  - `spec/planning/issue_155_manifest_reproducibility_checks_plan.md`
  - `spec/planning/issue_156_optional_spelling_parser_behavior_plan.md`
  - `spec/planning/issue_157_generic_function_mangler_plan.md`
  - `spec/planning/issue_158_abstract_machine_drift_lint_plan.md`
  - `spec/planning/issue_159_future_work_ownership_matrix_package.md`
- FW Wave 5 planning artifacts (`#161`..`#165`):
  - `spec/planning/issue_161_manifest_ci_publish_policy_package.md`
  - `spec/planning/issue_162_optional_spelling_diagnostics_fixit_plan.md`
  - `spec/planning/issue_163_demangler_reflection_update_plan.md`
  - `spec/planning/issue_164_extension_lifecycle_states_package.md`
  - `spec/planning/issue_165_future_work_risk_register_package.md`
- FW Wave 6 planning artifacts (`#167`..`#171`):
  - `spec/planning/issue_167_seeded_issue_conformance_tests_plan.md`
  - `spec/planning/issue_168_macro_package_provenance_policy_package.md`
  - `spec/planning/issue_169_vendor_conformance_declaration_template_package.md`
  - `spec/planning/issue_170_review_board_operating_model_package.md`
  - `spec/planning/issue_171_migration_guidance_package.md`
- FW Wave 7 planning artifacts (`#173`..`#177`):
  - `spec/planning/issue_173_conformance_evidence_dashboard_package.md`
  - `spec/planning/issue_174_macro_security_incident_playbook_package.md`
  - `spec/planning/issue_175_extension_test_obligations_package.md`
  - `spec/planning/issue_176_extension_registry_format_package.md`
  - `spec/planning/issue_177_external_review_cycle_plan.md`
- FW Wave 8 planning artifacts (`#179`..`#183`):
  - `spec/planning/issue_179_rc_toolchain_dry_run_package.md`
  - `spec/planning/issue_180_extension_pilots_workflow_package.md`
  - `spec/planning/issue_181_extension_onboarding_faq_package.md`
  - `spec/planning/issue_182_governance_v1_ratification_package.md`
  - `spec/planning/issue_183_docs_sync_workflow_package.md`
- FW Wave 9 planning artifacts (`#185`..`#191`):
  - `spec/planning/issue_185_quality_gates_thresholds_package.md`
  - `spec/planning/issue_186_discrepancy_triage_package.md`
  - `spec/planning/issue_187_cross_lane_checkpoint_package.md`
  - `spec/planning/issue_188_conformance_dress_rehearsal_package.md`
  - `spec/planning/issue_189_readiness_dossier_package.md`
  - `spec/planning/issue_190_carryover_descoping_package.md`
  - `spec/planning/issue_191_v012_kickoff_packet_package.md`
- Conformance evidence bundle schema/index/release gate (`#122`..`#125`):
  - `schemas/objc3-conformance-evidence-bundle-v1.schema.json`
  - `scripts/generate_conformance_evidence_index.py`
  - `scripts/check_release_evidence.py`
  - `.github/workflows/conformance-evidence-gate.yml`
- Parallel-governance artifacts (`#130`..`#133`):
  - `spec/planning/PARALLEL_LANE_OWNERSHIP_AND_HANDOFF.md`
  - `spec/planning/ISSUE_CLOSEOUT_EVIDENCE_TEMPLATE.md`
  - `spec/planning/ROADMAP_REFRESH_CADENCE_AND_SNAPSHOT_PROTOCOL.md`
  - `.github/ISSUE_TEMPLATE/roadmap_execution.yml`
  - `.github/ISSUE_TEMPLATE/conformance_execution.yml`
- Issue/checkbox drift guard (`#128`):
  - `scripts/check_issue_checkbox_drift.py`

Each execution item shall include:

- objective,
- artifacts,
- dependencies,
- done criteria,
- and closeout evidence references.

## IR.4 Epic operating model {#impl-roadmap-4}

### IR.4.1 Epic #111 (spec evolution) {#impl-roadmap-4-1}

- Completed. All child issues (`#115`..`#121`) are closed and linked to
  committed artifacts/evidence.

### IR.4.2 Epic #112 (conformance evidence) {#impl-roadmap-4-2}

- Completed (`#122`..`#125` closed with schema/index/checklist/gate artifacts).

### IR.4.3 Epic #113 (tooling and task hygiene) {#impl-roadmap-4-3}

- Completed (`#126`..`#129` closed with deterministic generation + drift checks).

### IR.4.4 Epic #114 (parallel execution governance) {#impl-roadmap-4-4}

- Completed (`#130`..`#133` closed with lane governance + templates).

### IR.4.5 Epic #142 (FW Wave 1 execution) {#impl-roadmap-4-5}

- Completed. All child issues (`#134`..`#141`) and epic closeout evidence are
  merged and synchronized.
- Child completion state: `#134`..`#141` closed.

### IR.4.6 Epic #148 (FW Wave 2 execution) {#impl-roadmap-4-6}

- Completed. All child issues (`#143`..`#147`) and epic closeout evidence are
  merged and synchronized.
- Child completion state: `#143`..`#147` closed.

### IR.4.7 Epic #154 (FW Wave 3 execution) {#impl-roadmap-4-7}

- Completed. All child issues (`#149`..`#153`) and epic closeout evidence are
  merged and synchronized.
- Child completion state: `#149`..`#153` closed.

### IR.4.8 Epic #160 (FW Wave 4 execution) {#impl-roadmap-4-8}

- Completed. All child issues (`#155`..`#159`) and epic closeout evidence are
  merged and synchronized.
- Child completion state: `#155`..`#159` closed.

### IR.4.9 Epic #166 (FW Wave 5 execution) {#impl-roadmap-4-9}

- Completed. All child issues (`#161`..`#165`) and epic closeout evidence are
  merged and synchronized.
- Child completion state: `#161`..`#165` closed.

### IR.4.10 Epic #172 (FW Wave 6 execution) {#impl-roadmap-4-10}

- Completed. All child issues (`#167`..`#171`) and epic closeout evidence are
  merged and synchronized.
- Child completion state: `#167`..`#171` closed.

### IR.4.11 Epic #178 (FW Wave 7 execution) {#impl-roadmap-4-11}

- Completed. All child issues (`#173`..`#177`) and epic closeout evidence are
  merged and synchronized.
- Child completion state: `#173`..`#177` closed.

### IR.4.12 Epic #184 (FW Wave 8 execution) {#impl-roadmap-4-12}

- Completed. All child issues (`#179`..`#183`) and epic closeout evidence are
  merged and synchronized.
- Child completion state: `#179`..`#183` closed.

### IR.4.13 Epic #192 (FW Wave 9 execution) {#impl-roadmap-4-13}

- Completed. All child issues (`#185`..`#191`) and epic closeout evidence are
  merged and synchronized.
- Child completion state: `#185`..`#191` closed.

## IR.5 Next-wave execution order {#impl-roadmap-5}

Next-wave execution order:

1. Wave 8 closeout is complete under
   `spec/planning/v012_wave8_batch_manifest_20260223.md`
   (`BATCH-20260223-02`) with 32 deterministic `SPT-*` issue closures.
2. Wave 9 closeout is complete under
   `spec/planning/v012_wave9_batch_manifest_20260223.md`
   (`BATCH-20260223-03`) with 31 deterministic `SPT-*` issue closures.
3. Execute Wave 10 worklane shards from
   `spec/planning/v012_wave10_candidate_shards_20260223.md` with file-isolated
   ownership under active manifest
   `spec/planning/v012_wave10_batch_manifest_20260223.md`
   (`BATCH-20260223-04`).
4. Wave 10 primary shards are complete (`SPT-0282`..`SPT-0293`,
   `SPT-0383`..`SPT-0390`, `SPT-0443`..`SPT-0449`) with deterministic closeout
   automation applied.
5. Post-Wave-10 extension shards are complete (`issue_180`, `issue_181`,
   `issue_186`, `issue_177`, and residual `issue_190` rows), closing an
   additional 32 `SPT-*` issues.
6. Execute Wave 11 candidate queue from
   `spec/planning/v012_wave11_candidate_shards_20260223.md` under active
   manifest
   `spec/planning/v012_wave11_batch_manifest_20260223.md`
   (`BATCH-20260223-05`).
7. Wave 11 closeout is complete under
   `spec/planning/v012_wave11_batch_manifest_20260223.md`
   (`BATCH-20260223-05`) with 29 deterministic `SPT-*` issue closures.
8. Wave 12 closeout is complete under
   `spec/planning/v012_wave12_batch_manifest_20260223.md`
   (`BATCH-20260223-06`) with 28 deterministic `SPT-*` issue closures.
9. Wave 13 closeout is complete under
   `spec/planning/v012_wave13_batch_manifest_20260223.md`
   (`BATCH-20260223-07`) with 19 deterministic `SPT-*` issue closures.
10. Wave 14 closeout is complete under
    `spec/planning/v012_wave14_batch_manifest_20260223.md`
    (`BATCH-20260223-08`) with 14 deterministic `SPT-*` issue closures.
11. Wave 15 closeout is complete under
    `spec/planning/v012_wave15_batch_manifest_20260223.md`
    (`BATCH-20260223-09`) with 15 deterministic `SPT-*` issue closures.
12. Wave 16 closeout is complete under
    `spec/planning/v012_wave16_batch_manifest_20260223.md`
    (`BATCH-20260223-10`) with 106 deterministic `SPT-*` issue closures.
13. Seeded unchecked-task execution queue is exhausted (`open SPT-* issues = 0`);
    no additional Wave 16 follow-on shard is required.
14. v0.13 Wave 1 foundation-planning batch is complete under manifest
    `spec/planning/v013_wave1_batch_manifest_20260223.md`
    (`BATCH-20260223-10A`) covering `#703`, `#704`, `#706`, `#707`.
15. v0.13 Wave 2 W0 seed batch is complete under manifest
    `spec/planning/v013_wave2_w0_batch_manifest_20260223.md`
    (`BATCH-20260223-10B`) covering `#708`, `#709`, `#710`, `#711`, `#712`.
16. v0.13 Wave 2 W1 is complete under manifest
    `spec/planning/v013_wave2_w1_batch_manifest_20260223.md`
    (`BATCH-20260223-11A`) covering `#713`, `#714`, `#715`, `#716`, `#717`.
17. Next dispatch queue staging is captured in
    `spec/planning/v013_wave2_next_dispatch_queue_20260223.md`.
18. W1B execution is complete under
    `spec/planning/v013_wave2_w1b_batch_manifest_20260223.md`
    (`BATCH-20260223-11C`) covering `#718`, `#719`, `#720`, `#721`.
19. Next unlocked dependency chain after W1B is `V013-SPEC-04` -> `V013-CONF-02`
    -> `V013-CONF-03` with release-lane gates continuing from those outputs.
20. W2 execution is complete under
    `spec/planning/v013_wave2_w2_batch_manifest_20260223.md`
    (`BATCH-20260223-11D`) covering `#722`, `#723`, `#725`, `#726`.
21. Next unlocked dependency chain after W2 is `V013-REL-02` -> `V013-REL-03`
    with kickoff publication gated by REL-02 completion.
22. W3 execution is complete under
    `spec/planning/v013_wave2_w3_batch_manifest_20260223.md`
    (`BATCH-20260223-11E`) covering `#727`, `#728`, `#729`, `#730`.
23. Milestone `v0.13 Seed Wave W3` is closed with `open_issues=0` and
    `closed_issues=4`.
24. Automation hardening W1 is complete under
    `spec/planning/v013_hardening_w1_batch_manifest_20260223.md`
    (`BATCH-20260223-11F`) covering `#731`, `#732`, `#733`, `#734`.
25. Milestone `v0.13 Automation Hardening W1` is closed with `open_issues=0`
    and `closed_issues=4`; next queue is staged in
    `spec/planning/v013_hardening_next_dispatch_queue_20260223.md`.
26. Automation hardening W2 is complete under
    `spec/planning/v013_hardening_w2_batch_manifest_20260223.md`
    (`BATCH-20260223-11G`) covering `#735`, `#736`, `#737`, `#738`, `#739`.
27. Milestone `v0.13 Automation Hardening W2` is closed with `open_issues=0`
    and `closed_issues=5`; W3 kickoff was staged from the same dispatch record.
28. Automation hardening W3 is complete under
    `spec/planning/v013_hardening_w3_batch_manifest_20260223.md`
    (`BATCH-20260223-11H`) covering `#740`.
29. Milestone `v0.13 Automation Hardening W3` is closed with `open_issues=0`
    and `closed_issues=1`; hardening backlog `HB-01`..`HB-10` is fully closed.
30. Execution tranche DQ01 is recorded under
    `spec/planning/v013_dq01_execution_batch_manifest_20260223.md`
    (`BATCH-20260223-DQ01`) for lane issues `#741`, `#742`, `#743`, `#744`.
31. DQ02 closeout synchronization is archived in
    `spec/planning/v013_execution_tranche_20260223.md` and
    `spec/planning/v013_wave2_next_dispatch_queue_20260223.md`, reducing
    actionable tranche rows to `0`.
32. Milestone `#21` (`v0.13 Execution Backlog Tranche 2026-02-23`) is closed
    with lane issues `#741`..`#744` closed.
33. Post-closeout control-plane sync is complete under
    `spec/planning/v013_post_closeout_sync_batch_manifest_20260223.md`
    (`BATCH-20260223-11I`) covering `#745`, `#746`, `#749`, `#750`.
34. Milestone `#22` (`v0.13 Post-Closeout Control Plane Sync`) is closed with
    `open_issues=0`; zero-open baseline state is captured in
    `spec/planning/v013_next_activation_queue_20260223.md`.
35. Control-plane continuity W1 is complete under
    `spec/planning/v013_control_plane_continuity_w1_batch_manifest_20260223.md`
    (`BATCH-20260223-11J`) covering `#751`, `#752`, `#753`, `#754`.
36. Milestone `#23` (`v0.13 Control-Plane Continuity W1`) is closed with
    `open_issues=0`; roadmap/backlog continuity sync is complete.
37. Activation tooling W1 is complete under
    `spec/planning/v013_activation_tooling_w1_batch_manifest_20260223.md`
    (`BATCH-20260223-11K`) covering `#755`, `#756`, `#757`, `#758`.
38. Milestone `#24` (`v0.13 Activation Tooling W1`) is closed with
    `open_issues=0`; activation trigger automation is now integrated into
    script, tests, workflow, and queue documentation.
39. Activation tooling W2 is complete under
    `spec/planning/v013_activation_tooling_w2_batch_manifest_20260223.md`
    (`BATCH-20260223-11L`) covering `#759`, `#760`, `#761`, `#762`.
40. Milestone `#25` (`v0.13 Activation Tooling W2`) is closed with
    `open_issues=0`; T4 governance-overlay support is now integrated into
    checker outputs, tests, workflow commands, and queue docs.
41. Catalog integrity W1 is complete under
    `spec/planning/v013_catalog_integrity_w1_batch_manifest_20260223.md`
    (`BATCH-20260223-11M`) covering `#763`, `#764`, `#765`, `#766`.
42. Milestone `#26` (`v0.13 Catalog Integrity W1`) is closed with
    `open_issues=0`; deterministic missing-status diagnostics are integrated in
    tooling/CI, and strict no-missing-status mode is available behind
    `CATALOG_STATUS_STRICT=1`.
43. Catalog integrity W2 is complete under
    `spec/planning/v013_catalog_integrity_w2_batch_manifest_20260223.md`
    (`BATCH-20260223-11N`) covering `#767`, `#768`, `#769`, `#770`.
44. Milestone `#27` (`v0.13 Catalog Integrity W2`) is closed with
    `open_issues=0`; catalog missing-status count is reduced to `0` and strict
    status-integrity gating is enabled by default in package/workflow paths.
45. Catalog integrity W3 is complete under
    `spec/planning/v013_catalog_integrity_w3_batch_manifest_20260223.md`
    (`BATCH-20260223-11O`) covering `#771`, `#772`, `#773`, `#774`.
46. Milestone `#28` (`v0.13 Catalog Integrity W3`) is closed with
    `open_issues=0`; catalog status metadata gaps are reduced to `0` and
    metadata completeness gating is enabled in package/workflow hygiene paths.
47. Forward scope activation W1 is complete under
    `spec/planning/v013_forward_scope_activation_w1_batch_manifest_20260223.md`
    (`BATCH-20260223-11P`) covering `#775`, `#776`, `#777`, `#778`.
48. Milestone `#29` (`v0.13 Forward Scope Activation W1`) is closed with
    `open_issues=0`; lane outputs are merged with validation evidence and
    integrator closeout synchronization complete.
49. Seed wave W0 reseed 1 is complete under
    `spec/planning/v013_seed_wave_w0_reseed1_batch_manifest_20260223.md`
    (`BATCH-20260223-11Q`) covering `#779`, `#780`, `#781`, `#782`, `#783`.
50. Milestone `#30` (`v0.13 Seed Wave W0 Reseed 1`) is closed with
    `open_issues=0`; all W0 seeds in this tranche are merged and validated.
51. Seed wave W1 reseed 1 is complete under
    `spec/planning/v013_seed_wave_w1_reseed1_batch_manifest_20260223.md`
    (`BATCH-20260223-11R`) covering `#784`, `#785`, `#786`, `#787`, `#788`.
52. Milestone `#31` (`v0.13 Seed Wave W1 Reseed 1`) is closed with
    `open_issues=0`; all listed W1 seeds are merged and validated.
53. Seed wave W1/W2 reseed 2 is complete under
    `spec/planning/v013_seed_wave_w1w2_reseed2_batch_manifest_20260223.md`
    (`BATCH-20260223-11S`) covering `#789`, `#790`, `#791`, `#792`, `#793`.
54. Milestone `#32` (`v0.13 Seed Wave W1/W2 Reseed 2`) is closed with
    `open_issues=0`; all selected seeds are merged and validated.

## IR.6 Completion gates {#impl-roadmap-6}

An issue is complete only when:

1. normative/spec behavior is settled,
2. diagnostics and tooling expectations are explicit,
3. test or validation evidence exists and is reproducible,
4. issue comments include commit and evidence links,
5. parent epic status is synchronized in the same batch.

## IR.7 Reporting cadence {#impl-roadmap-7}

- Refresh this roadmap and `spec/EXECUTION_MICROTASK_BACKLOG.md` after each
  merged batch that changes issue state.
- Update active epics in the same batch as child issue closeout updates.
- Keep snapshot dates explicit and consistent across planning artifacts.

## IR.8 GitHub execution metrics snapshot {#impl-roadmap-8}

Snapshot date: `2026-02-23` (post-closeout state for milestone `#32`)

- Closed issues: `789`
- Open issues: `0`
- Open milestones: `0`
- Minimum 2x follow-on task target: `1578`
- Generated microtasks from open issues: `0`
- Expanded v0.11 task inventory (`spec/FUTURE_WORK_V011.md`): `56` tasks
- Active lane split: `A=0`, `B=0`, `C=0`, `D=0`, `INT=0`
- Active batch: `none` (no open issue tranche)
- Latest completed batch: `BATCH-20260223-11S` (v0.13 seed wave W1/W2 reseed 2
  closure, `#789`, `#790`, `#791`, `#792`, `#793`)
- Latest closed milestone: `#32` (`v0.13 Seed Wave W1/W2 Reseed 2`)
- Latest extension closeout sweep: `59` additional `SPT-*` closures after Wave 10 baseline (already completed before Wave 11)

The generated microtask backlog now reflects zero open issues with milestone
`#32` closed. Seeded `SPT-*` execution remains fully closed as baseline while
dependency-valid reseed outputs are synchronized for the next activation gate.
