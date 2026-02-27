# Objective-C 3.0 - Future Work Plan v0.11 {#future-work-v011}

_Working draft v0.11 - last updated 2026-02-23_

This document defines the next planning cycle after closure of current open issue sets. It expands unresolved specification questions into parallel implementation, tooling, governance, and program-control work.

## FW.1 Scope and Inputs {#future-work-v011-1}

This plan is seeded from open-issues sections in:

- `Part 0 - Baseline and Normative References` (`0.6 Open issues`).
- `Part 3 - Types, Nullability, Optionals, Generics, Key Paths` (`3.9 Open issues`).
- `Part 10 - Metaprogramming, Derives, Macros, Property Behaviors` (`10.7 Open issues`).

Each task below includes:

- objective,
- artifacts,
- done criteria,
- dependencies.

## FW.2 Seed Issue Traceability {#future-work-v011-2}

| Seed source | Open issue seed | Primary expansion tasks |
| --- | --- | --- |
| Part 0 `0.6` | Publish concrete artifact manifests for `objc3-runtime-2025Q4` and `objc3-abi-2025Q4`. | `A-01`, `A-02`, `B-01`, `B-02`, `B-03`, `B-04`, `B-13` |
| Part 0 `0.6` | Keep abstract-machine section synchronized with Parts 3/6/7/8. | `A-03`, `A-04`, `B-12`, `D-07`, `D-08` |
| Part 3 `3.9` (issue 1) | Canonical value-optional source spelling and alias policy. | `A-05`, `A-06`, `B-05`, `B-06`, `D-06` |
| Part 3 `3.9` (issue 2) | Canonical generic free-function mangling string vs semantic-only invariants. | `A-07`, `A-08`, `B-07`, `B-08`, `D-06` |
| Part 3 `3.9` (issue 3) | Reification control scope (`@reify_generics` vs module/profile). | `A-09`, `A-10`, `A-11`, `B-09`, `B-10` |
| Part 10 `10.7` | Extension-governance process for experimental derives/macros across vendors. | `C-01` through `C-14`, plus `D-10` |

## FW.3 Parallel Lane Topology {#future-work-v011-3}

| Lane | Goal | Lane dependencies | Exit signal |
| --- | --- | --- | --- |
| Lane A - Normative closure | Resolve spec-decision ambiguity from Parts 0/3 and publish normative closure package. | No upstream lane dependency; seeds all other lanes. | All Part 0/3 seeded issues mapped to ratified decisions and example coverage. |
| Lane B - Implementation and tooling | Implement manifest, parser, mangling, reification, and conformance automation tied to Lane A decisions. | Depends on specific Lane A decisions; consumes Lane C capability IDs where needed. | Toolchain artifacts and conformance evidence are reproducible and dashboarded. |
| Lane C - Governance and ecosystem | Establish cross-vendor extension governance for derives/macros and ratify policy references for Part 10. | Starts in parallel; feeds policy constraints to Lanes B and D. | Governance v1 ratified with pilots and registry schema in place. |
| Lane D - Program control and release readiness | Orchestrate sequencing, quality gates, risk burn-down, and cycle handoff into v0.12 planning. | Depends on critical outputs from Lanes A/B/C. | Readiness dossier accepted and carryover packet published. |

## FW.4 Lane A - Normative Closure and Cross-Part Semantics {#future-work-v011-4}

### FW.4.1 Lane Goal {#future-work-v011-4-1}

Convert open ambiguity in Part 0 and Part 3 into ratified, testable, cross-referenced normative decisions.

### FW.4.2 Task Set (14 Tasks) {#future-work-v011-4-2}

| Task | Objective | Artifacts | Done criteria | Depends on |
| --- | --- | --- | --- | --- |
| `A-01` | Define and publish machine-readable schema for `objc3-runtime-2025Q4` and `objc3-abi-2025Q4` manifests. | `spec/conformance/objc3_artifact_manifest_schema_v1.md`, `schemas/objc3_artifact_manifest.schema.json` | Schema is reviewed by spec editors, example manifests validate, and Part 0 references are ready. | None |
| `A-02` | Define manifest schema versioning and compatibility policy. | `spec/conformance/objc3_artifact_manifest_versioning_v1.md`, `spec/DECISIONS_LOG.md` entry | Major/minor compatibility and deprecation windows are explicit and accepted by release owners. | `A-01` |
| `A-03` | Define cross-part abstract-machine synchronization protocol for Parts 0/3/6/7/8. | `spec/process/abstract_machine_sync_checklist.md`, update proposal for `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md` | Protocol assigns owners, required cross-links, and change-trigger rules for all listed parts. | None |
| `A-04` | Run baseline abstract-machine consistency audit across Parts 0/3/6/7/8. | `reports/spec_sync/abstract_machine_audit_2026Q1.md` | Every delta is classified as normative conflict, editorial mismatch, or missing example with owner/date. | `A-03` |
| `A-05` | Decide canonical value-optional spelling policy (`Optional<T>` and/or `optional<T>`). | Part 3 patch draft for `3.9`, `spec/DECISIONS_LOG.md` entry | Canonical spelling and alias policy are ratified and remove ambiguity from open issue 3.9.1. | None |
| `A-06` | Specify diagnostics and migration behavior for noncanonical optional spelling. | Part 3 diagnostics patch draft, `spec/EDGE_CASE_EXAMPLES_AND_FIXITS.md` updates | Warning/error matrix and fix-it behavior are defined for all conformance profiles. | `A-05` |
| `A-07` | Decide canonical generic free-function mangling string strategy. | Part 3 patch draft for `3.9`, `spec/DECISIONS_LOG.md` entry | Canonical representation and invariants are explicit and open issue 3.9.2 is closed. | None |
| `A-08` | Publish mangling invariants with positive/negative examples to prevent tool divergence. | `spec/MODULE_METADATA_AND_ABI_TABLES.md` appendix draft, `tests/vectors/generic_fn_mangling_vectors.csv` | At least 20 positive and 10 negative vectors are approved by compiler/runtime owners. | `A-07` |
| `A-09` | Decide reification control scope model (declaration, module, or profile). | Part 3 patch draft for `3.9`, `spec/DECISIONS_LOG.md` entry | Scope model is selected with rationale and compatibility implications documented. | None |
| `A-10` | Specify syntax and semantics for chosen reification-control model. | `spec/FORMAL_GRAMMAR_AND_PRECEDENCE.md` patch draft, Part 3 examples | Grammar, constraints, and profile gating are normative with no unresolved placeholders. | `A-09` |
| `A-11` | Define capability identifiers for optional/reification reservation surfaces. | `spec/CROSS_CUTTING_RULE_INDEX.md` additions, capability registry draft | Stable IDs, collision policy, and forward-compat rules are documented and review-approved. | `A-05`, `A-09` |
| `A-12` | Update conformance profile gates for all Lane A decisions. | `spec/CONFORMANCE_PROFILE_CHECKLIST.md` patch draft, gate matrix | Each new rule maps to Core/Strict/Optional claims with required pass criteria. | `A-06`, `A-08`, `A-10`, `A-11` |
| `A-13` | Add edge-case examples covering optional spelling, mangling, and reification outcomes. | `spec/EDGE_CASE_EXAMPLES_AND_FIXITS.md` patch draft | Examples include expected diagnostics and fix-its for representative success/failure cases. | `A-06`, `A-08`, `A-10` |
| `A-14` | Ratify consolidated Part 0 + Part 3 closure bundle for handoff to implementation lanes. | `spec/reviews/future_work_v011_lane_a_bundle.md` | Editorial sign-off is recorded and all seeded Part 0/3 open issues have closure references. | `A-04`, `A-12`, `A-13` |

## FW.5 Lane B - Implementation and Conformance Tooling {#future-work-v011-5}

### FW.5.1 Lane Goal {#future-work-v011-5-1}

Translate Lane A decisions into deterministic build artifacts, diagnostics behavior, and conformance automation.

### FW.5.2 Task Set (14 Tasks) {#future-work-v011-5-2}

| Task | Objective | Artifacts | Done criteria | Depends on |
| --- | --- | --- | --- | --- |
| `B-01` | Implement manifest generation pipeline for runtime/ABI conformance artifacts. | `tools/conformance/generate_objc3_manifests.ps1 (removed)`, sample outputs in `reports/conformance/manifests/` | Clean builds emit both manifests with deterministic field ordering and required metadata. | `A-01` |
| `B-02` | Implement manifest schema validation command and CI hook. | `tools/conformance/validate_objc3_manifest.ps1 (removed)`, CI step configuration draft | Invalid manifests fail with actionable diagnostics; golden manifests pass in CI and local runs. | `A-01` |
| `B-03` | Add reproducibility checks for repeated manifest generation runs. | `tests/conformance/manifests/MANIFEST-DET-01.md`, `MANIFEST-DET-02.md`, hash harness | Three repeated builds on same profile produce byte-identical manifests. | `B-01`, `B-02` |
| `B-04` | Publish manifests from CI using immutable naming and retention policy. | CI workflow draft (`.github/workflows/objc3-manifest-publish.yml (removed)` or equivalent), artifact policy note | Every merged build publishes signed manifests with stable lookup metadata. | `B-01`, `B-02`, `B-03` |
| `B-05` | Implement parser behavior for canonical and allowed alias optional spellings. | Parser patch plan, `tests/parse/OPT-SPELL-*` | Parser accepts/rejects spellings exactly per Lane A policy across profiles. | `A-05` |
| `B-06` | Implement diagnostics/fix-its for noncanonical optional spelling paths. | Diagnostics patch plan, `tests/diag/OPT-DIAG-*` | Diagnostic messages and fix-it edits match Lane A migration spec with profile-sensitive severity. | `A-06`, `B-05` |
| `B-07` | Implement canonical generic free-function mangler. | Mangler patch plan, `tests/abi/MANGLE-GFN-*` | Symbol emission matches canonical vectors with zero diffs against approved corpus. | `A-07`, `A-08` |
| `B-08` | Update demangler/reflection behavior for new generic function mangling form. | Demangler patch plan, `tests/abi/DEMANGLE-GFN-*` | Encode/decode round-trip and reflection display pass full vector suite. | `B-07` |
| `B-09` | Implement reification scope controls in semantic analysis. | Sema patch plan, `tests/sema/REIFY-SCOPE-*` | Declaration/module/profile scope behavior is enforced exactly as specified in Lane A. | `A-09`, `A-10` |
| `B-10` | Emit capability report entries for optional/reification features. | Capability report generator patch plan, sample capability JSON | Capability report uses stable IDs and fails on unknown/unsupported IDs. | `A-11`, `B-09` |
| `B-11` | Add conformance tests that directly close seeded Part 0/3 issues. | `tests/conformance/spec_open_issues/P0-P3-*`, mapping file from test IDs to issue IDs | Each seeded issue has at least one required-pass and one required-fail test case. | `A-12`, `B-06`, `B-08`, `B-10` |
| `B-12` | Implement automated abstract-machine drift lint across Parts 0/3/6/7/8. | `tools/spec_lints/abstract_machine_sync_lint.ps1`, CI lint job draft | CI fails when synchronization protocol rules are violated. | `A-03`, `A-04` |
| `B-13` | Build conformance evidence dashboard for manifests, tests, and capability outputs. | `reports/conformance/dashboard_v011.md`, machine-readable status JSON | Dashboard links all required artifacts and exposes pass/fail state per profile. | `B-04`, `B-10`, `B-11`, `B-12` |
| `B-14` | Run release-candidate toolchain dry run and publish blocker report. | `reports/releases/rc_v011_dry_run.md`, blocker triage list | Blockers are severity-ranked, owner-assigned, and ready for D-lane go/no-go gates. | `B-13` |

## FW.6 Lane C - Governance and Ecosystem Process {#future-work-v011-6}

### FW.6.1 Lane Goal {#future-work-v011-6-1}

Close Part 10 governance ambiguity by defining and piloting a repeatable cross-vendor extension process for derives/macros.

### FW.6.2 Task Set (14 Tasks) {#future-work-v011-6-2}

| Task | Objective | Artifacts | Done criteria | Depends on |
| --- | --- | --- | --- | --- |
| `C-01` | Define cross-vendor governance charter for experimental derives/macros. | `spec/governance/macro_derive_extension_charter_v1.md` | Charter defines scope, authority, and decision outputs and is approved by steering owners. | None |
| `C-02` | Define capability naming and namespace policy for extension IDs. | `spec/governance/capability_namespace_policy_v1.md`, reserved-prefix table | Namespace collisions are mechanically detectable and naming examples cover public/private/vendor spaces. | `C-01` |
| `C-03` | Define proposal intake template with required evidence fields. | `templates/experimental_extension_proposal.md` | Template requires syntax, semantics, diagnostics, determinism, and security evidence sections. | `C-01` |
| `C-04` | Define extension review rubric and acceptance thresholds. | `spec/governance/extension_review_rubric_v1.md` | Rubric has weighted criteria for safety, portability, ergonomics, and tooling cost. | `C-01` |
| `C-05` | Define extension lifecycle states and promotion/deprecation criteria. | `spec/governance/extension_lifecycle_v1.md` | State transitions and minimum evidence for promotion/removal are explicit and review-approved. | `C-03`, `C-04` |
| `C-06` | Define lockfile, provenance, and reproducibility policy for macro/derive packages. | `spec/governance/macro_package_provenance_policy_v1.md` | Signed package metadata, hash pinning, and mismatch handling are normative. | `C-01` |
| `C-07` | Define security incident response workflow for compromised macro packages. | `spec/governance/macro_security_incident_playbook_v1.md` | Playbook includes severity classes, containment procedures, and disclosure timelines. | `C-06` |
| `C-08` | Define vendor conformance declaration template for extension capability claims. | `templates/vendor_extension_conformance_claim.md` | Claims must cite capability IDs, test IDs, and provenance evidence. | `C-02`, `C-05` |
| `C-09` | Define mandatory interop/conformance test obligations before extension promotion. | `spec/governance/extension_test_obligations_v1.md`, test catalog skeleton | Required test families and failure thresholds are defined per lifecycle state. | `C-05`, `C-08` |
| `C-10` | Define multi-vendor review board operating cadence and voting rules. | `spec/governance/review_board_operating_model_v1.md` | Quorum, veto, tie-break, and publication cadence are approved and scheduled. | `C-01` |
| `C-11` | Stand up public registry format for extension capabilities and lifecycle state. | `registries/experimental_extensions/index.schema.json`, registry README | Registry schema validates sample entries and includes lifecycle/version metadata. | `C-02`, `C-08`, `C-10` |
| `C-12` | Run two pilot extensions through full governance workflow. | `pilots/extension_proposals/PILOT-001.md`, `PILOT-002.md`, review records | Both pilots complete intake, review, testing obligations, and board decision publication. | `C-03`, `C-04`, `C-09`, `C-11` |
| `C-13` | Publish onboarding and FAQ documentation for extension authors/vendors. | `docs/governance/extension_author_guide_v1.md`, `docs/governance/faq_v1.md` | Docs provide end-to-end process guidance with links to templates and registry. | `C-12` |
| `C-14` | Ratify governance v1 and integrate references into Part 10 follow-on edits. | Part 10 patch plan, ratification record | Part 10 open issue has explicit closure linkage to ratified governance docs. | `C-12`, `C-13` |

## FW.7 Lane D - Program Control, Integration, and Handoff {#future-work-v011-7}

### FW.7.1 Lane Goal {#future-work-v011-7-1}

Convert lane outputs into executable sequencing, measurable quality gates, and a clean handoff package for the next planning cycle.

### FW.7.2 Task Set (14 Tasks) {#future-work-v011-7-2}

| Task | Objective | Artifacts | Done criteria | Depends on |
| --- | --- | --- | --- | --- |
| `D-01` | Produce integrated dependency map across all lane tasks. | `spec/planning/future_work_v011_dependency_map.md` | All 56 tasks have predecessors/successors and a single critical path is identified. | `A-01`, `A-03`, `A-05`, `A-07`, `A-09`, `C-01` |
| `D-02` | Define cycle milestones, gate dates, and review checkpoints. | `spec/planning/future_work_v011_milestones.md` | Milestones map to task IDs and include objective entry/exit criteria. | `D-01` |
| `D-03` | Assign primary and backup owners for every task. | `spec/planning/future_work_v011_ownership_matrix.md` | Each task has accountable owner, backup owner, and review SLA. | `D-02` |
| `D-04` | Build risk register linked to unresolved or high-volatility decisions. | `spec/planning/future_work_v011_risk_register.md` | Every risk has likelihood, impact, mitigation task links, and trigger metrics. | `D-01`, `A-05`, `A-07`, `A-09`, `C-01` |
| `D-05` | Define quality gates and pass thresholds for cycle exit. | `spec/planning/future_work_v011_quality_gates.md` | Conformance, diagnostics stability, and reproducibility thresholds are explicit and measurable. | `B-11`, `B-13` |
| `D-06` | Produce migration guidance for optional, mangling, and reification changes. | `docs/migration/v011_language_changes.md` | Guide includes migration paths, fix-it usage, and compatibility fallback recommendations. | `A-12`, `B-06`, `B-08`, `B-09` |
| `D-07` | Define docs synchronization workflow for normative and tooling artifacts. | `spec/planning/future_work_v011_docs_sync_workflow.md` | Workflow defines trigger conditions, required reviewers, and publication cadence. | `A-14`, `B-13`, `C-14` |
| `D-08` | Define discrepancy triage process for spec/toolchain divergence. | `spec/planning/future_work_v011_discrepancy_triage.md`, issue template | Severity matrix and response SLA are adopted and validated on sample discrepancy. | `D-07` |
| `D-09` | Run 25/50/75 percent cross-lane integration checkpoints. | `reports/planning/v011_checkpoint_25.md`, `v011_checkpoint_50.md`, `v011_checkpoint_75.md` | Each checkpoint records variance, mitigation actions, and updated critical path. | `D-02`, `D-03`, `B-13`, `C-12` |
| `D-10` | Run external review cycle with vendors and ecosystem maintainers. | `reports/reviews/v011_external_feedback.md`, action ledger | Feedback is triaged and mapped to tasks, accepted changes, or explicit deferrals. | `C-10`, `D-02`, `D-03` |
| `D-11` | Execute internal conformance dress rehearsal before readiness decision. | `reports/releases/v011_conformance_dress_rehearsal.md` | Rehearsal covers all targeted profiles with no unresolved P0 blockers. | `B-14`, `D-05` |
| `D-12` | Compile integrated release-readiness dossier for planning gate review. | `reports/releases/v011_readiness_dossier.md` | Dossier links required artifacts from all lanes and includes go/no-go recommendation. | `D-06`, `D-08`, `D-10`, `D-11` |
| `D-13` | Decide carryover/de-scope list for unfinished tasks. | `spec/planning/future_work_v011_carryover.md` | Each incomplete task is classified as carryover, defer, or drop with rationale. | `D-12` |
| `D-14` | Publish v0.12 kickoff packet from approved carryover. | `spec/FUTURE_WORK_V012_BOOTSTRAP.md`, kickoff checklist | Next-cycle bootstrap is published with prioritized carryover and dependency map. | `D-13` |

## FW.8 First 8 Highest-Priority Tasks and Recommended Order {#future-work-v011-8}

| Order | Task | Why this position | Immediate dependency note |
| --- | --- | --- | --- |
| `1` | `A-01` | Unblocks manifest schema, CI publication, and reproducibility tooling. | No prerequisite. |
| `2` | `A-03` | Defines synchronization contract needed to prevent semantic drift across core parts. | No prerequisite. |
| `3` | `C-01` | Establishes governance authority early so extension decisions have a valid process. | No prerequisite. |
| `4` | `A-05` | Resolves optional spelling ambiguity that directly impacts parser and migration behavior. | No prerequisite. |
| `5` | `A-07` | Resolves mangling ambiguity before ABI/tooling implementation starts. | No prerequisite. |
| `6` | `A-09` | Resolves reification scope before grammar/sema implementation. | No prerequisite. |
| `7` | `B-01` | Starts manifest implementation as soon as schema is available to create early feedback loops. | Requires `A-01`. |
| `8` | `D-01` | Locks critical-path planning once key normative/governance seeds are decided. | Requires `A-01`, `A-03`, `A-05`, `A-07`, `A-09`, `C-01`. |

## FW.9 Task Count Summary {#future-work-v011-9}

| Lane | Task count |
| --- | --- |
| Lane A - Normative closure | `14` |
| Lane B - Implementation and tooling | `14` |
| Lane C - Governance and ecosystem | `14` |
| Lane D - Program control and handoff | `14` |
| **Total** | **`56`** |

