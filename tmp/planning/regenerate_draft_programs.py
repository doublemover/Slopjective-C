from __future__ import annotations

import json
import shutil
import subprocess
from collections import defaultdict
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
TMP = ROOT / "tmp"
PLANNING = TMP / "planning"
PUBLISH = TMP / "github-publish"
BACKLOG_PUBLICATION_POLICY_JSON = "tmp/planning/backlog_publication/backlog_publication_policy.json"
BACKLOG_PUBLICATION_POLICY_MD = "tmp/planning/backlog_publication/backlog_publication_policy.md"
BACKLOG_CONTRACT_DIR = PUBLISH / "backlog_contract"
BACKLOG_PROGRAM_MANIFEST_SCHEMA = "tmp/github-publish/backlog_contract/draft_backlog_program_manifest_v1.schema.json"
BACKLOG_DEPENDENCY_GRAPH_SCHEMA = "tmp/github-publish/backlog_contract/draft_backlog_dependency_graph_v1.schema.json"
BACKLOG_COUNT_SNAPSHOT_SCHEMA = "tmp/github-publish/backlog_contract/draft_backlog_count_snapshot_v1.schema.json"
BACKLOG_MASTER_SNAPSHOT_SCHEMA = "tmp/github-publish/backlog_contract/draft_backlog_master_snapshot_v1.schema.json"

MEASURE_ROOTS = ["scripts", "tests", "native", "docs", "showcase", "stdlib"]


def run(cmd: list[str]) -> str:
    return subprocess.check_output(cmd, cwd=ROOT, text=True, encoding="utf-8").strip()


def git_ls(pattern: str) -> list[str]:
    out = run(["git", "ls-files", pattern])
    return [line for line in out.splitlines() if line]


def count_package_scripts() -> int:
    package = json.loads((ROOT / "package.json").read_text(encoding="utf-8"))
    return len(package.get("scripts", {}))


def count_check_files() -> int:
    total = 0
    for rel in MEASURE_ROOTS:
        base = ROOT / rel
        if not base.exists():
            continue
        total += sum(1 for _ in base.rglob("check_*.py"))
    return total


def count_test_check_files() -> int:
    total = 0
    for rel in MEASURE_ROOTS:
        base = ROOT / rel
        if not base.exists():
            continue
        total += sum(1 for _ in base.rglob("test_check_*.py"))
    return total


def count_m2xx_refs() -> int:
    base = ROOT / "native" / "objc3c" / "src"
    total = 0
    if not base.exists():
        return 0
    for path in base.rglob("*"):
        if not path.is_file():
            continue
        try:
            text = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue
        total += text.count("m2xx-")
    return total


def repo_fact_snapshot() -> dict[str, Any]:
    return {
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "measurement_roots": MEASURE_ROOTS,
        "queries": {
            "package_scripts": "len(package.json.scripts)",
            "check_py_files": "count of check_*.py under scripts/tests/native/docs/showcase/stdlib",
            "test_check_py_files": "count of test_check_*.py under scripts/tests/native/docs/showcase/stdlib",
            "m2xx_refs_in_native_src": "literal occurrences of 'm2xx-' under native/objc3c/src",
            "tracked_ll_files": "git ls-files '*.ll'",
            "tracked_stub_ll_files": "git ls-files '*stub*.ll'",
            "prototype_semantic_surface_present": "filesystem existence check for compiler/objc3c/semantic.py",
        },
        "repo_facts": {
            "package_scripts": count_package_scripts(),
            "check_py_files": count_check_files(),
            "test_check_py_files": count_test_check_files(),
            "m2xx_refs_in_native_src": count_m2xx_refs(),
            "tracked_ll_files": len(git_ls("*.ll")),
            "tracked_stub_ll_files": len(git_ls("*stub*.ll")),
            "prototype_semantic_surface_present": (ROOT / "compiler" / "objc3c" / "semantic.py").exists(),
        },
    }


@dataclass
class IssueSpec:
    code: str
    title: str
    stage: str
    objective: str
    acceptance: list[str]
    dependencies: list[str] = field(default_factory=list)
    notes: list[str] = field(default_factory=list)


@dataclass
class MilestoneSpec:
    code: str
    title: str
    domain: str
    priority: str
    area_labels: list[str]
    focus: str
    exit_gate: str
    corrections: list[str]
    shared_surfaces: list[str]
    blocked_by_milestones: list[str]
    publication_scope: str
    issues: list[IssueSpec]


@dataclass
class ProgramSpec:
    code: str
    title: str
    planning_dir: str
    publish_dir: str
    prerequisite_sequence: str
    recommended_sequence: str
    rationale: list[str]
    notes: list[str]
    milestone_order: list[str]
    parallel_groups: list[list[str]]
    milestones: list[MilestoneSpec]
    include_repo_facts: bool = False


def kind_for_issue(issue: IssueSpec) -> str:
    if issue.stage == "A":
        return "inventory"
    if issue.stage == "E":
        return "closeout"
    if issue.stage == "D":
        return "integration"
    title = issue.title.lower()
    if issue.stage == "B":
        return "implementation" if "implementation" in title else "contract"
    if issue.stage == "C":
        if "contract" in title or "schema" in title:
            return "artifact"
        return "implementation"
    return "contract"


def issue_labels(milestone: MilestoneSpec, issue: IssueSpec) -> list[str]:
    labels = [
        f"milestone:{milestone.code}",
        f"lane:{issue.stage}",
        f"kind:{kind_for_issue(issue)}",
        milestone.domain,
        milestone.priority,
        "type:roadmap",
        "source:planning-checklist",
    ]
    labels.extend(milestone.area_labels)
    if milestone.publication_scope == "internal-first":
        labels.append("publication:internal-first")
    return labels


def stage_acceptance(stage: str) -> list[str]:
    shared = {
        "A": [
            "Freeze the boundary truthfully, including non-goals and unresolved follow-on work.",
            "Capture only the direct blockers and successor surfaces needed for downstream execution.",
            "Back every count or inventory claim with a generated snapshot or a replayable source reference.",
        ],
        "B": [
            "Define one canonical semantic or policy model and retire competing interpretations.",
            "Keep claims narrower than the evidence and fail closed where runtime behavior is intentionally deferred.",
            "Make downstream artifact and runtime work executable without reopening first-order design decisions.",
        ],
        "C": [
            "Make the milestone's machine-owned artifact, schema, lowering, or tooling surface canonical and replayable.",
            "Avoid duplicate transitional outputs when one stable artifact surface will do.",
            "Preserve determinism across generated evidence, packaging, and public workflow entrypoints.",
        ],
        "D": [
            "Land runnable behavior on the real compiler/runtime/tooling path rather than a milestone-local scaffold.",
            "Back the work with executable probes, packaged validation, or operator drills instead of prose-only assertions.",
            "Harden determinism, cleanup, failure handling, and recovery on the live path.",
        ],
        "E": [
            "Publish one truthful closeout gate over the live surfaces landed earlier in the milestone.",
            "Use shared acceptance harnesses and real evidence rather than milestone-local success theater.",
            "Demote or block claims that are not actually earned by the evidence.",
        ],
    }
    return shared[stage]


def stage_notes(stage: str) -> list[str]:
    base = [
        "Prefer integrating with existing compiler/runtime/tooling/package surfaces rather than creating milestone-specific parallel scaffolds.",
        "Store transient validation captures under `tmp/` and make closeout evidence replayable.",
        "Keep public and internal claims narrower than the evidence wherever uncertainty remains.",
    ]
    lane_specific = {
        "A": ["Lane A removes ambiguity; it should not create a second planning layer."],
        "B": ["Lane B should sharpen executable meaning and explicit policy, not merely restate intent."],
        "C": ["Lane C is where the design becomes machine-checkable and replayable."],
        "D": ["Lane D should feel real to a user or operator, not just to a checker."],
        "E": ["Lane E earns credibility; it is not a dumping ground for unfinished feature work."],
    }
    return lane_specific[stage] + base


def publication_rules_lines() -> list[str]:
    return [
        "Use milestone codes and issue codes as stable source identifiers.",
        "Accept GitHub-assigned milestone and issue numbers as live publication artifacts; do not predict or preserve desired numeric IDs in source manifests.",
        "Keep `blocked_by_issue_codes` and GitHub built-in blocker links direct-only.",
        "Back all count claims with generated snapshots or replayable measurement scripts.",
        f"Canonical policy surface: `{BACKLOG_PUBLICATION_POLICY_JSON}` and `{BACKLOG_PUBLICATION_POLICY_MD}`.",
    ]


def artifact_policy_refs() -> list[str]:
    return [BACKLOG_PUBLICATION_POLICY_JSON, BACKLOG_PUBLICATION_POLICY_MD]


MILESTONE_REPO_REFERENCES: dict[str, dict[str, list[str]]] = {
    "M313": {
        "all": [
            "`scripts/ci/run_task_hygiene_gate.py` and `scripts/ci/check_task_hygiene.py` for the current validation-budget path.",
            "`scripts/check_repo_superclean_surface.py`, `scripts/check_documentation_surface.py`, and `scripts/check_objc3c_dependency_boundaries.py` for repo-shape enforcement.",
            "`scripts/render_objc3c_public_command_surface.py`, `docs/runbooks/objc3c_public_command_surface.md`, and `package.json` for public command-surface truth.",
            "`scripts/objc3c_public_workflow_runner.py` `execute_registered_action()` and `run_steps()` for shared workflow composition instead of bespoke milestone runners.",
        ],
        "D": [
            "Use existing integration gates before inventing new ones; start at `scripts/objc3c_public_workflow_runner.py` and the current `validate-*` actions already wired there.",
        ],
    },
    "M314": {
        "all": [
            "`scripts/objc3c_public_workflow_runner.py` `list_actions_payload()`, `describe_action_payload()`, and `execute_registered_action()` are the current command-routing spine.",
            "`package.json` and `docs/runbooks/objc3c_public_command_surface.md` are the public command inventories that must stay synchronized.",
            "`scripts/render_objc3c_public_command_surface.py` is the current generator for the published command appendix.",
        ],
        "C": [
            "Prefer collapsing command aliases into the existing action registry rather than adding more one-off shell wrappers.",
        ],
    },
    "M315": {
        "all": [
            "`README.md`, `docs/runbooks/`, `site/`, and `showcase/` are the main human-facing truth surfaces that tend to accumulate stale milestone residue.",
            "`scripts/check_documentation_surface.py`, `scripts/check_repo_superclean_surface.py`, and `scripts/check_objc3c_dependency_boundaries.py` are the right enforcement points.",
            "`tmp/planning/` and `tmp/github-publish/` should remain the only homes for draft/planning residue after cleanup.",
        ],
    },
    "M316": {
        "all": [
            "`native/objc3c/src/runtime/objc3_runtime.cpp` is the live corrective runtime surface; inspect `objc3_runtime_dispatch_i32()`, `objc3_runtime_bind_current_property_context_for_testing()`, and the runtime state around realized classes/properties.",
            "`tests/tooling/runtime/class_realization_runtime_probe.cpp`, `category_attachment_protocol_runtime_probe.cpp`, and `object_model_lookup_reflection_runtime_probe.cpp` already exercise the corrective object-model path.",
            "`tests/tooling/runtime/property_layout_runtime_probe.cpp`, `property_ivar_execution_matrix_probe.cpp`, and `runtime_property_metadata_reflection_probe.cpp` are the fastest existing probes for synthesized-accessor and property/runtime truth.",
            "`scripts/shared_compiler_runtime_acceptance_harness.py` is the shared acceptance entrypoint to extend rather than cloning new ad hoc runtime harnesses.",
        ],
        "D": [
            "Treat this as a corrective tranche, not stealth full closure; land fixes on the existing runtime path and prove them through the existing runtime probes first.",
        ],
    },
    "M317": {
        "all": [
            "`tmp/planning/draft_backlog_master_summary.md`, `tmp/planning/draft_backlog_master_snapshot.json`, and `tmp/planning/regenerate_draft_programs.py` are the canonical backlog-shaping surfaces.",
            "`tmp/github-publish/*/program_manifest.json` and `dependency_edges.json` are the publish-side artifacts that must stay structurally consistent.",
            f"`{BACKLOG_PUBLICATION_POLICY_JSON}` and `{BACKLOG_PUBLICATION_POLICY_MD}` are the canonical supersession/direct-blocker/publication-scope rules for the full backlog set.",
            "Use the generator and snapshots as the source of truth; avoid hand-editing dozens of individual draft issue bodies when the structure changes.",
        ],
    },
    "M318": {
        "all": [
            "`scripts/check_repo_superclean_surface.py`, `scripts/check_documentation_surface.py`, `scripts/check_objc3c_dependency_boundaries.py`, and `scripts/ci/check_task_hygiene.py` are the current enforcement anchors.",
            "`docs/runbooks/objc3c_maintainer_workflows.md` and `docs/runbooks/objc3c_public_command_surface.md` are the operator-facing policy surfaces likely to need governance ratchets.",
            "`scripts/objc3c_public_workflow_runner.py` should remain the canonical workflow surface when new governance checks become public commands.",
        ],
    },
    "M319": {
        "all": [
            "`native/objc3c/src/runtime/objc3_runtime.cpp` is the main runtime closure file; start with `objc3_runtime_dispatch_i32()` and the realized-class/property state around class, category, protocol, and reflection caches.",
            "`tests/tooling/runtime/class_realization_runtime_probe.cpp`, `category_attachment_protocol_runtime_probe.cpp`, and `object_model_lookup_reflection_runtime_probe.cpp` already cover live class/category/protocol behavior.",
            "`tests/tooling/runtime/property_layout_runtime_probe.cpp`, `property_ivar_execution_matrix_probe.cpp`, `runtime_property_metadata_reflection_probe.cpp`, and `runtime_backed_storage_ownership_reflection_probe.cpp` are the main property/ivar/reflection probes.",
            "`scripts/objc3c_public_workflow_runner.py` `action_validate_object_model_conformance()`, `action_validate_storage_reflection_conformance()`, `action_validate_runnable_object_model()`, and `action_validate_runnable_storage_reflection()` are the existing public entrypoints to extend.",
        ],
        "B": [
            "Make reflection answers match executed runtime behavior; do not treat metadata emission as sufficient evidence.",
        ],
    },
    "M320": {
        "all": [
            "`tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp`, `block_runtime_byref_forwarding_probe.cpp`, and `block_arc_runtime_abi_probe.cpp` are the main escaping-block/byref/ABI probes already in-tree.",
            "`tests/tooling/runtime/arc_helper_runtime_support_probe.cpp`, `arc_debug_instrumentation_probe.cpp`, and `reference_counting_weak_autoreleasepool_probe.cpp` are the main ARC/weak/autorelease probes.",
            "`scripts/objc3c_public_workflow_runner.py` `action_validate_block_arc_conformance()` and `action_validate_runnable_block_arc()` are the current public gates for this surface.",
            "`native/objc3c/src/runtime/objc3_runtime.cpp` already contains ARC/property debug counters and weak-property helpers such as `objc3_runtime_load_weak_current_property_i32()` and `objc3_runtime_store_weak_current_property_i32()`.",
        ],
        "D": [
            "Explicitly test ARC interaction with properties, errors, and suspension points; those cross-feature edges are where fake closure usually survives.",
        ],
    },
    "M321": {
        "all": [
            "`tests/tooling/runtime/live_error_runtime_integration_probe.cpp`, `error_runtime_bridge_helper_probe.cpp`, `live_cleanup_retainable_integration_probe.cpp`, `cleanup_unwind_autoreleasepool_probe.cpp`, and `cleanup_unwind_nested_autoreleasepool_probe.cpp` are the current executable anchors for thrown/error behavior.",
            "`scripts/objc3c_public_workflow_runner.py` `action_validate_error_conformance()` and `action_validate_runnable_error()` are the existing public commands to preserve.",
            "`native/objc3c/src/sema/objc3_semantic_passes.cpp` and the lowering/IR surfaces are the likely places where thrown-value legality, cleanup ordering, and bridge semantics still diverge from runtime reality.",
        ],
        "B": [
            "Force unwind ordering, cleanup behavior, and NSError/status bridging onto one runtime-backed model before broadening public claims.",
        ],
    },
    "M322": {
        "all": [
            "`tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp`, `live_continuation_runtime_integration_probe.cpp`, `task_runtime_abi_completion_probe.cpp`, and `task_runtime_hardening_probe.cpp` already target task/runtime closure.",
            "`tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp`, `actor_runtime_executor_contract_probe.cpp`, `actor_lowering_runtime_probe.cpp`, and `scheduler_executor_runtime_contract_probe.cpp` are the current actor/executor anchors.",
            "`scripts/objc3c_public_workflow_runner.py` `action_validate_concurrency_conformance()` and `action_validate_runnable_concurrency()` are the existing user-facing gates.",
            "`stdlib/advanced_helper_package_surface.json` and `stdlib/program_surface.json` already describe actor/concurrency helper surfaces that should eventually map to real runtime behavior.",
        ],
        "D": [
            "Keep task-runtime and actor-runtime proofs distinct until both are actually closed; otherwise actor metadata can hide scheduler incompleteness.",
        ],
    },
    "M323": {
        "all": [
            "`tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp` and `expansion_host_runtime_boundary_probe.cpp` are the main metaprogramming/runtime anchors.",
            "`tests/tooling/runtime/import_module_execution_matrix_probe.cpp`, `header_module_bridge_generation_probe.cpp`, `cross_module_runtime_packaging_probe.cpp`, and `bridge_packaging_toolchain_probe.cpp` are the best interop starting points.",
            "`scripts/objc3c_public_workflow_runner.py` `action_validate_metaprogramming_conformance()`, `action_validate_runnable_metaprogramming()`, `action_validate_interop_conformance()`, and `action_validate_runnable_interop()` are the existing public gates.",
            "`scripts/check_objc3c_runnable_metaprogramming_conformance.py`, `scripts/check_objc3c_runnable_interop_conformance.py`, and `scripts/check_objc3c_runnable_interop_end_to_end.py` already capture part of the runnable proof surface.",
        ],
        "B": [
            "Keep the two tracks explicit: macro/property-behavior runtime materialization and broader foreign-boundary interop closure should not blur together in prose.",
        ],
    },
    "M324": {
        "all": [
            "`scripts/check_objc3c_conformance_corpus_integration.py`, `check_objc3c_stress_integration.py`, `check_objc3c_external_validation_integration.py`, and `check_objc3c_public_conformance_reporting_integration.py` already cover core claimability inputs.",
            "`scripts/check_objc3c_performance_governance_integration.py`, `check_objc3c_release_foundation_integration.py`, `check_objc3c_release_operations_integration.py`, and `check_objc3c_distribution_credibility_integration.py` are the release/performance credibility gates that must inherit the full-envelope story.",
            "`scripts/objc3c_public_workflow_runner.py` and `docs/runbooks/objc3c_public_conformance_reporting.md`, `objc3c_performance_governance.md`, `objc3c_release_operations.md`, and `objc3c_distribution_credibility.md` are the public claim surfaces to tighten.",
        ],
        "E": [
            "Add an explicit demotion path (`supported`, `experimental`, `unsupported`, `release-blocking`) rather than letting unfinished feature families ride along on a top-level green check.",
        ],
    },
    "M325": {
        "all": [
            "`scripts/objc3c_public_workflow_runner.py` `execute_registered_action()`, `describe_action_payload()`, and the existing inspect/developer-tooling actions are the current CLI spine to preserve.",
            "`docs/runbooks/objc3c_developer_tooling.md` and `docs/runbooks/objc3c_public_command_surface.md` are the current operator-facing tooling contracts.",
            "`native/objc3c/src/` diagnostics, source mapping, and debug-info emission surfaces should remain the source of truth; avoid editor-only shadow parsers.",
            "`scripts/check_objc3c_developer_tooling_integration.py` and the existing developer-tooling report surfaces under `tmp/reports/developer-tooling/` are the most relevant proof scaffolds already in-tree.",
        ],
    },
    "M326": {
        "all": [
            "`scripts/package_objc3c_runnable_toolchain.ps1`, `scripts/build_objc3c_native.ps1`, and `scripts/build_objc3c_package_channels.py` are the main portability and packaging surfaces that already encode host assumptions.",
            "`docs/runbooks/objc3c_packaging_channels.md`, `objc3c_release_foundation.md`, and `objc3c_release_operations.md` are the operator-facing places where support tiers will become user-visible.",
            "`tests/tooling/` packaged validation and runnable end-to-end probes are better leverage points than inventing a second portability-only test harness.",
        ],
        "B": [
            "Use explicit support tiers (`Tier 1`, `Tier 2`, `Experimental`) instead of binary supported/unsupported rhetoric.",
        ],
    },
    "M327": {
        "all": [
            "`scripts/build_objc3c_release_manifest.py`, `publish_objc3c_release_provenance.py`, `build_objc3c_update_manifest.py`, and `publish_objc3c_release_operations_metadata.py` are the current supply-chain and release-trust anchors.",
            "`docs/runbooks/objc3c_release_foundation.md`, `objc3c_release_operations.md`, and `objc3c_distribution_credibility.md` are where security and disclosure policy will need to become operator-visible.",
            "`schemas/objc3c-release-*.schema.json` and the release/distribution evidence under `tmp/artifacts/` and `tmp/reports/` are the existing machine-owned trust surfaces to extend rather than duplicating.",
        ],
    },
    "M328": {
        "all": [
            "`scripts/build_objc3c_update_manifest.py`, `scripts/publish_objc3c_release_operations_metadata.py`, and the release/distribution validation scripts are the right starting points for migration, rollback, and long-horizon compatibility evidence.",
            "`docs/runbooks/objc3c_release_operations.md`, `objc3c_distribution_credibility.md`, and `objc3c_performance_governance.md` already describe parts of the long-horizon operator story.",
            "`tmp/reports/` artifacts from performance, conformance, release operations, and distribution credibility are the substrate for soak and aging evidence; reuse them before inventing a new report family.",
        ],
    },
    "M329": {
        "all": [
            "`stdlib/workspace.json`, `stdlib/package_surface.json`, and `stdlib/advanced_helper_package_surface.json` are the current closest things to package/workspace truth in-tree.",
            "`scripts/package_objc3c_runnable_toolchain.ps1`, `scripts/build_objc3c_package_channels.py`, and the release/update manifest builders are the existing packaging/distribution surfaces a package model must eventually integrate with.",
            "`docs/tutorials/getting_started.md` and the stdlib/showcase program surfaces are the current consumer flows; the package model should serve those first before hosted registry ambition grows.",
        ],
        "B": [
            "Design local workspace semantics, lockfiles, and offline mirrors before spending design budget on a public registry surface.",
        ],
    },
    "M330": {
        "all": [
            "`docs/tutorials/getting_started.md`, `docs/tutorials/README.md`, and `showcase/README.md` are the current starter/onboarding surfaces that will need to converge with templates and canonical apps.",
            "`showcase/portfolio.json`, `stdlib/program_surface.json`, and `stdlib/workspace.json` are the existing structured program surfaces to reuse instead of inventing a parallel examples taxonomy.",
            "`scripts/materialize_objc3c_stdlib_workspace.py` and the runnable toolchain/package workflow scripts are the best starting points for project-template materialization.",
        ],
        "D": [
            "Use canonical apps to pressure package/install/dependency design; they should be consumers of the ecosystem model, not decorative examples bolted on afterwards.",
        ],
    },
    "M331": {
        "all": [
            "`README.md`, `docs/tutorials/getting_started.md`, `docs/tutorials/objc2_to_objc3_migration.md`, `showcase/README.md`, and `site/` are the primary evaluator-facing narrative surfaces to tighten.",
            "`docs/runbooks/objc3c_public_conformance_reporting.md`, `objc3c_performance_governance.md`, and the release/distribution runbooks already carry the evidence-backed public claims this milestone should reuse.",
            "`scripts/render_objc3c_public_command_surface.py` and `docs/runbooks/objc3c_public_command_surface.md` are the canonical entrypoint index external evaluators will hit first.",
        ],
    },
    "M332": {
        "all": [
            "`docs/governance/extension_author_guide_v1.md`, `docs/governance/faq_v1.md`, and `tests/governance/registry_compat/README.md` are the best existing governance-related anchors in-tree.",
            "`scripts/check_objc3c_dependency_boundaries.py`, release/update/security publication tooling, and package/distribution runbooks are the live process surfaces governance needs to codify rather than bypass.",
            "`tmp/github-publish/*/program_manifest.json` and `dependency_edges.json` are good precedents for machine-owned process metadata if governance ends up needing structured review artifacts.",
        ],
    },
}


def milestone_repo_references(milestone_code: str, stage: str) -> list[str]:
    refs = MILESTONE_REPO_REFERENCES.get(milestone_code, {})
    if stage == "all":
        return refs.get("all", [])
    return refs.get("all", []) + refs.get(stage, [])


def stage_guidance(stage: str) -> list[str]:
    hints = {
        "A": [
            "Name concrete files and command surfaces while scoping the boundary; vague subsystem labels slow down the first implementation issue.",
        ],
        "B": [
            "Write the contract against the live code and probe surfaces so the later implementation issues can diff behavior directly instead of rediscovering intent.",
        ],
        "C": [
            "Prefer extending an existing generator, validator, schema, or runner over creating a new milestone-local tool unless the old one is structurally wrong.",
        ],
        "D": [
            "Start by extending existing runnable probes and public workflow actions; only add new integration entrypoints when the current path cannot truthfully express the feature.",
        ],
        "E": [
            "Treat the closeout issue as a claim-audit pass: prune unsupported claims and wire the surviving ones into the existing public validation/reporting flow.",
        ],
    }
    return hints[stage]


def next_unblocks(issue_code: str, issue_index: dict[str, IssueSpec], all_issues: list[IssueSpec]) -> list[str]:
    return [issue.code for issue in all_issues if issue_code in issue.dependencies]


def milestone_issue_count_map(program: ProgramSpec) -> dict[str, int]:
    return {m.code: len(m.issues) for m in program.milestones}


def build_milestone_readme(program: ProgramSpec, milestone: MilestoneSpec) -> str:
    labels = ", ".join(f"`{label}`" for label in issue_labels(milestone, milestone.issues[0]) if not label.startswith("lane:") and not label.startswith("kind:"))
    issues = "\n".join(
        f"- `{issue.code}` [{issue.code.split('-')[0]}][Lane-{issue.stage}][{issue.code.split('-')[1]}] {issue.title}"
        for issue in milestone.issues
    )
    blocked = ", ".join(f"`{code}`" for code in milestone.blocked_by_milestones) if milestone.blocked_by_milestones else "none"
    repo_refs = milestone_repo_references(milestone.code, "all")
    return (
        f"# {milestone.code} {milestone.title}\n\n"
        f"- domain: `{milestone.domain}`\n"
        f"- publication scope: `{milestone.publication_scope}`\n"
        f"- issues: `{len(milestone.issues)}`\n"
        f"- common labels: {labels}\n\n"
        f"## Focus\n{milestone.focus}\n\n"
        f"## Exit gate\n{milestone.exit_gate}\n\n"
        f"## Corrections folded in\n"
        + "\n".join(f"- {item}" for item in milestone.corrections)
        + "\n\n## Shared surfaces\n"
        + "\n".join(f"- {item}" for item in milestone.shared_surfaces)
        + ("\n\n## Useful Repo References\n" + "\n".join(f"- {item}" for item in repo_refs) if repo_refs else "")
        + "\n\n## Execution order\n"
        + f"- Recommended sequence: `{program.recommended_sequence}`\n"
        + f"- Direct milestone blockers: {blocked}\n"
        + f"- Milestone-local issue chain starts at `{milestone.issues[0].code}` and ends at `{milestone.issues[-1].code}`.\n"
        + "\n## Issues\n"
        + issues
        + "\n"
    )


def issue_body(program: ProgramSpec, milestone: MilestoneSpec, issue: IssueSpec, issue_index: dict[str, IssueSpec]) -> str:
    direct_unblocks = next_unblocks(issue.code, issue_index, milestone.issues)
    blockers = ", ".join(f"`{code}`" for code in issue.dependencies) if issue.dependencies else "none"
    unblocks = ", ".join(f"`{code}`" for code in direct_unblocks) if direct_unblocks else "none"
    milestone_blockers = ", ".join(f"`{code}`" for code in milestone.blocked_by_milestones) if milestone.blocked_by_milestones else "none"
    repo_refs = milestone_repo_references(milestone.code, issue.stage)
    deliverables = {
        "A": [
            "Boundary inventory, non-goals, and successor map in the canonical planning artifacts.",
            "Measured counts and inventories pulled from replayable queries instead of hand-entered literals.",
            "Truthful updates to milestone, program, and publication metadata when the boundary changes.",
        ],
        "B": [
            "One canonical semantic or policy contract on the real implementation path.",
            "Guardrails or fail-closed behavior only where they materially prevent overclaiming.",
            "Documentation updates that narrow the public claim surface to the evidence.",
        ],
        "C": [
            "Canonical schema, artifact, lowering, or tooling surfaces under `schemas/`, `scripts/`, `tests/tooling/fixtures/`, or `tmp/reports/` as appropriate.",
            "Replayable generation path for any new machine-owned output.",
            "Generated publication artifacts kept in sync with the planning source of truth.",
        ],
        "D": [
            "Runnable integration on the real compiler/runtime/tooling/package path.",
            "Executable probes, packaged validation, or operator drills proving the live surface.",
            "Recovery and failure-mode behavior documented where the surface becomes operator-visible.",
        ],
        "E": [
            "One merged closeout gate over the live milestone evidence.",
            "Updated support, demotion, or release-blocking metadata when claims are narrower than planned.",
            "Final closeout artifact wired into the publish-ready JSON and blocker graph outputs.",
        ],
    }[issue.stage]
    return (
        f"[{milestone.code}][Lane-{issue.stage}][{issue.code.split('-')[1]}] {issue.title}\n\n"
        f"## Outcome\n{issue.objective}\n\n"
        f"## Why this matters\nThis issue exists inside **{milestone.code} {milestone.title}** because it advances the milestone focus: {milestone.focus} The milestone exit gate is: {milestone.exit_gate}\n\n"
        f"## Design corrections folded in\n"
        + "\n".join(f"- {item}" for item in milestone.corrections)
        + "\n\n## Required deliverables\n"
        + "\n".join(f"- {item}" for item in deliverables)
        + "\n\n## Acceptance criteria\n"
        + "\n".join(f"- {item}" for item in (issue.acceptance + stage_acceptance(issue.stage)))
        + "\n\n## Primary implementation surfaces\n"
        + "\n".join(f"- {item}" for item in milestone.shared_surfaces)
        + ("\n\n## Useful repo references\n" + "\n".join(f"- {item}" for item in repo_refs) if repo_refs else "")
        + "\n\n## Implementation guidance\n"
        + "\n".join(f"- {item}" for item in stage_guidance(issue.stage))
        + "\n\n## Dependencies\n"
        + f"- {blockers}\n"
        + "\n## Notes\n"
        + "\n".join(f"- {item}" for item in (issue.notes + stage_notes(issue.stage)))
        + "\n\n<!-- EXECUTION-ORDER-START -->\n"
        + "## Execution Order\n"
        + f"- Direct milestone blockers: {milestone_blockers}\n"
        + f"- Direct issue blockers: {blockers}\n"
        + f"- Directly unblocks: {unblocks}\n"
        + f"- Execution instruction: Complete the direct blockers for `{issue.code}` and keep the work on the canonical implementation path for `{milestone.code}`.\n"
        + "<!-- EXECUTION-ORDER-END -->\n"
    )


def build_program_readme(program: ProgramSpec, count_snapshot_path: str) -> str:
    milestone_lines = "\n".join(
        f"- {m.code}: {m.code} {m.title} ({len(m.issues)} issues)"
        for m in program.milestones
    )
    lines = [
        f"# {program.title}",
        "",
        f"- milestones: {len(program.milestones)}",
        f"- issues: {sum(len(m.issues) for m in program.milestones)}",
        f"- count snapshot: `{count_snapshot_path}`",
        f"- prerequisite sequence: `{program.prerequisite_sequence}`",
        f"- recommended sequence: `{program.recommended_sequence}`",
        "",
        "## Why this program exists",
    ]
    lines.extend(f"- {item}" for item in program.rationale)
    if program.notes:
        lines.extend(["", "## Notes"])
        lines.extend(f"- {item}" for item in program.notes)
    if program.parallel_groups:
        lines.extend(["", "## Parallel milestone groups"])
        for group in program.parallel_groups:
            lines.append(f"- `{', '.join(group)}`")
    lines.extend(["", "## Publication rules"])
    lines.extend(f"- {item}" for item in publication_rules_lines())
    if program.include_repo_facts:
        snapshot = repo_fact_snapshot()["repo_facts"]
        lines.extend(["", "## Observed repo facts"])
        lines.extend(
            [
                f"- `package.json` scripts: {snapshot['package_scripts']}",
                f"- `check_*.py` files under measured roots: {snapshot['check_py_files']}",
                f"- `test_check_*.py` files under measured roots: {snapshot['test_check_py_files']}",
                f"- `m2xx-*` refs inside `native/objc3c/src`: {snapshot['m2xx_refs_in_native_src']}",
                f"- tracked `.ll` files: {snapshot['tracked_ll_files']}",
                f"- tracked `*stub*.ll` files: {snapshot['tracked_stub_ll_files']}",
                f"- `compiler/objc3c/semantic.py` present: {snapshot['prototype_semantic_surface_present']}",
            ]
        )
    lines.extend(["", "## Milestones", milestone_lines])
    lines.extend(["", "## GitHub publication readiness"])
    lines.extend(
        [
            f"- Per-issue draft bodies are generated under `tmp/planning/{program.planning_dir}/issues/`.",
            f"- Publish-ready JSON bodies are generated under `tmp/github-publish/{program.publish_dir}/issue_bodies/`.",
            "- `program_manifest.json` carries future label and blocker metadata for each milestone and issue.",
            "- `dependency_edges.json` is the exact direct blocker graph to translate into GitHub built-in issue dependencies when the backlog is published.",
        ]
    )
    return "\n".join(lines) + "\n"


def build_publish_readme(program: ProgramSpec) -> str:
    return (
        f"# {program.title} Publish Artifacts\n\n"
        f"This directory contains publish-ready JSON issue bodies and dependency metadata for `{program.code}`.\n\n"
        f"- issue JSON bodies: `issue_bodies/`\n"
        f"- program manifest: `program_manifest.json`\n"
        f"- direct blocker graph: `dependency_edges.json`\n"
        f"- canonical publication policy: `{BACKLOG_PUBLICATION_POLICY_JSON}`\n"
        f"- manifest schema: `{BACKLOG_PROGRAM_MANIFEST_SCHEMA}`\n"
        f"- dependency-graph schema: `{BACKLOG_DEPENDENCY_GRAPH_SCHEMA}`\n"
        f"- stable identifiers in source: milestone codes and issue codes\n"
        f"- live identifiers: GitHub-assigned milestone and issue numbers captured only after publication\n"
    )


def issue_index_for(milestones: list[MilestoneSpec]) -> dict[str, IssueSpec]:
    idx: dict[str, IssueSpec] = {}
    for milestone in milestones:
        for issue in milestone.issues:
            idx[issue.code] = issue
    return idx


def milestone_manifest(program: ProgramSpec, milestone: MilestoneSpec, issue_index: dict[str, IssueSpec]) -> dict[str, Any]:
    return {
        "code": milestone.code,
        "title": milestone.title,
        "domain": milestone.domain,
        "priority": milestone.priority,
        "publication_scope": milestone.publication_scope,
        "area_labels": milestone.area_labels,
        "blocked_by_milestones": milestone.blocked_by_milestones,
        "issue_count": len(milestone.issues),
        "issues": [issue_manifest(program, milestone, issue, issue_index) for issue in milestone.issues],
    }


def issue_manifest(program: ProgramSpec, milestone: MilestoneSpec, issue: IssueSpec, issue_index: dict[str, IssueSpec]) -> dict[str, Any]:
    direct_unblocks = next_unblocks(issue.code, issue_index, milestone.issues)
    return {
        "code": issue.code,
        "title": f"[{milestone.code}][Lane-{issue.stage}][{issue.code.split('-')[1]}] {issue.title}",
        "milestone_code": milestone.code,
        "publication_scope": milestone.publication_scope,
        "domain": milestone.domain,
        "priority": milestone.priority,
        "label_names": issue_labels(milestone, issue),
        "blocked_by_milestones": milestone.blocked_by_milestones,
        "blocked_by_issue_codes": issue.dependencies,
        "direct_unblock_issue_codes": direct_unblocks,
        "kind": kind_for_issue(issue),
        "body": issue_body(program, milestone, issue, issue_index),
    }


def dependency_graph(program: ProgramSpec, issue_index: dict[str, IssueSpec]) -> dict[str, Any]:
    issue_edges = []
    milestone_edges = []
    for milestone in program.milestones:
        for blocker in milestone.blocked_by_milestones:
            milestone_edges.append({"from": blocker, "to": milestone.code})
        for issue in milestone.issues:
            for blocker in issue.dependencies:
                issue_edges.append({"from": blocker, "to": issue.code})
    return {
        "program_code": program.code,
        "schema_version": "draft-backlog-dependency-graph-v1",
        "schema_path": BACKLOG_DEPENDENCY_GRAPH_SCHEMA,
        "policy_refs": artifact_policy_refs(),
        "milestone_edges": milestone_edges,
        "issue_edges": issue_edges,
    }


def write_backlog_contract_files() -> None:
    BACKLOG_CONTRACT_DIR.mkdir(parents=True, exist_ok=True)
    readme = "\n".join(
        [
            "# Draft Backlog Publish Contract",
            "",
            f"- program manifest schema: `{BACKLOG_PROGRAM_MANIFEST_SCHEMA}`",
            f"- dependency graph schema: `{BACKLOG_DEPENDENCY_GRAPH_SCHEMA}`",
            f"- count snapshot schema: `{BACKLOG_COUNT_SNAPSHOT_SCHEMA}`",
            f"- master snapshot schema: `{BACKLOG_MASTER_SNAPSHOT_SCHEMA}`",
            f"- canonical publication policy: `{BACKLOG_PUBLICATION_POLICY_JSON}`",
            "",
            "These schemas define the stable machine-owned artifact shape for the draft backlog programs.",
            "",
        ]
    )
    (BACKLOG_CONTRACT_DIR / "README.md").write_text(readme, encoding="utf-8")

    def write_schema(filename: str, schema: dict[str, Any]) -> None:
        (BACKLOG_CONTRACT_DIR / filename).write_text(json.dumps(schema, indent=2) + "\n", encoding="utf-8")

    issue_manifest_schema = {
        "type": "object",
        "required": [
            "code",
            "title",
            "milestone_code",
            "publication_scope",
            "domain",
            "priority",
            "label_names",
            "blocked_by_milestones",
            "blocked_by_issue_codes",
            "direct_unblock_issue_codes",
            "kind",
            "body",
        ],
        "properties": {
            "code": {"type": "string"},
            "title": {"type": "string"},
            "milestone_code": {"type": "string"},
            "publication_scope": {"type": "string"},
            "domain": {"type": "string"},
            "priority": {"type": "string"},
            "label_names": {"type": "array", "items": {"type": "string"}},
            "blocked_by_milestones": {"type": "array", "items": {"type": "string"}},
            "blocked_by_issue_codes": {"type": "array", "items": {"type": "string"}},
            "direct_unblock_issue_codes": {"type": "array", "items": {"type": "string"}},
            "kind": {"type": "string"},
            "body": {"type": "string"},
        },
        "additionalProperties": False,
    }
    milestone_manifest_schema = {
        "type": "object",
        "required": [
            "code",
            "title",
            "domain",
            "priority",
            "publication_scope",
            "area_labels",
            "blocked_by_milestones",
            "issue_count",
            "issues",
        ],
        "properties": {
            "code": {"type": "string"},
            "title": {"type": "string"},
            "domain": {"type": "string"},
            "priority": {"type": "string"},
            "publication_scope": {"type": "string"},
            "area_labels": {"type": "array", "items": {"type": "string"}},
            "blocked_by_milestones": {"type": "array", "items": {"type": "string"}},
            "issue_count": {"type": "integer"},
            "issues": {"type": "array", "items": issue_manifest_schema},
        },
        "additionalProperties": False,
    }
    write_schema(
        "draft_backlog_program_manifest_v1.schema.json",
        {
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$id": BACKLOG_PROGRAM_MANIFEST_SCHEMA,
            "title": "Draft Backlog Program Manifest v1",
            "type": "object",
            "required": [
                "program_code",
                "schema_version",
                "schema_path",
                "title",
                "generated_at",
                "count_snapshot",
                "policy_refs",
                "prerequisite_sequence",
                "recommended_sequence",
                "parallel_milestone_groups",
                "milestones",
            ],
            "properties": {
                "program_code": {"type": "string"},
                "schema_version": {"const": "draft-backlog-program-manifest-v1"},
                "schema_path": {"const": BACKLOG_PROGRAM_MANIFEST_SCHEMA},
                "title": {"type": "string"},
                "generated_at": {"type": "string"},
                "count_snapshot": {"type": "string"},
                "policy_refs": {"type": "array", "items": {"type": "string"}},
                "prerequisite_sequence": {"type": "string"},
                "recommended_sequence": {"type": "string"},
                "parallel_milestone_groups": {
                    "type": "array",
                    "items": {"type": "array", "items": {"type": "string"}},
                },
                "milestones": {"type": "array", "items": milestone_manifest_schema},
            },
            "additionalProperties": False,
        },
    )
    write_schema(
        "draft_backlog_dependency_graph_v1.schema.json",
        {
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$id": BACKLOG_DEPENDENCY_GRAPH_SCHEMA,
            "title": "Draft Backlog Dependency Graph v1",
            "type": "object",
            "required": [
                "program_code",
                "schema_version",
                "schema_path",
                "policy_refs",
                "milestone_edges",
                "issue_edges",
            ],
            "properties": {
                "program_code": {"type": "string"},
                "schema_version": {"const": "draft-backlog-dependency-graph-v1"},
                "schema_path": {"const": BACKLOG_DEPENDENCY_GRAPH_SCHEMA},
                "policy_refs": {"type": "array", "items": {"type": "string"}},
                "milestone_edges": {
                    "type": "array",
                    "items": {
                        "type": "object",
                        "required": ["from", "to"],
                        "properties": {
                            "from": {"type": "string"},
                            "to": {"type": "string"},
                        },
                        "additionalProperties": False,
                    },
                },
                "issue_edges": {
                    "type": "array",
                    "items": {
                        "type": "object",
                        "required": ["from", "to"],
                        "properties": {
                            "from": {"type": "string"},
                            "to": {"type": "string"},
                        },
                        "additionalProperties": False,
                    },
                },
            },
            "additionalProperties": False,
        },
    )
    write_schema(
        "draft_backlog_count_snapshot_v1.schema.json",
        {
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$id": BACKLOG_COUNT_SNAPSHOT_SCHEMA,
            "title": "Draft Backlog Count Snapshot v1",
            "type": "object",
            "required": [
                "program_code",
                "schema_version",
                "schema_path",
                "policy_refs",
                "generated_at",
                "repo_facts",
                "milestone_count",
                "issue_count",
                "issue_counts_by_milestone",
                "prerequisite_sequence",
                "recommended_sequence",
                "parallel_milestone_groups",
            ],
            "properties": {
                "program_code": {"type": "string"},
                "schema_version": {"const": "draft-backlog-count-snapshot-v1"},
                "schema_path": {"const": BACKLOG_COUNT_SNAPSHOT_SCHEMA},
                "policy_refs": {"type": "array", "items": {"type": "string"}},
                "generated_at": {"type": "string"},
                "repo_facts": {"type": "object"},
                "milestone_count": {"type": "integer"},
                "issue_count": {"type": "integer"},
                "issue_counts_by_milestone": {
                    "type": "object",
                    "additionalProperties": {"type": "integer"},
                },
                "prerequisite_sequence": {"type": "string"},
                "recommended_sequence": {"type": "string"},
                "parallel_milestone_groups": {
                    "type": "array",
                    "items": {"type": "array", "items": {"type": "string"}},
                },
            },
            "additionalProperties": False,
        },
    )
    write_schema(
        "draft_backlog_master_snapshot_v1.schema.json",
        {
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$id": BACKLOG_MASTER_SNAPSHOT_SCHEMA,
            "title": "Draft Backlog Master Snapshot v1",
            "type": "object",
            "required": [
                "schema_version",
                "schema_path",
                "policy_refs",
                "generated_at",
                "repo_facts",
                "total_milestones",
                "total_issues",
                "programs",
            ],
            "properties": {
                "schema_version": {"const": "draft-backlog-master-snapshot-v1"},
                "schema_path": {"const": BACKLOG_MASTER_SNAPSHOT_SCHEMA},
                "policy_refs": {"type": "array", "items": {"type": "string"}},
                "generated_at": {"type": "string"},
                "repo_facts": {"type": "object"},
                "total_milestones": {"type": "integer"},
                "total_issues": {"type": "integer"},
                "programs": {
                    "type": "object",
                    "additionalProperties": {
                        "type": "object",
                        "required": ["milestones", "issues", "milestone_order"],
                        "properties": {
                            "milestones": {"type": "integer"},
                            "issues": {"type": "integer"},
                            "milestone_order": {"type": "array", "items": {"type": "string"}},
                        },
                        "additionalProperties": False,
                    },
                },
            },
            "additionalProperties": False,
        },
    )


def write_program(program: ProgramSpec, repo_snapshot: dict[str, Any]) -> None:
    planning_dir = PLANNING / program.planning_dir
    publish_dir = PUBLISH / program.publish_dir
    if planning_dir.exists():
        shutil.rmtree(planning_dir)
    if publish_dir.exists():
        shutil.rmtree(publish_dir)
    planning_dir.mkdir(parents=True, exist_ok=True)
    publish_dir.mkdir(parents=True, exist_ok=True)
    (planning_dir / "milestones").mkdir(parents=True, exist_ok=True)
    (planning_dir / "issues").mkdir(parents=True, exist_ok=True)
    (publish_dir / "issue_bodies").mkdir(parents=True, exist_ok=True)

    issue_index = issue_index_for(program.milestones)
    count_snapshot = {
        "program_code": program.code,
        "schema_version": "draft-backlog-count-snapshot-v1",
        "schema_path": BACKLOG_COUNT_SNAPSHOT_SCHEMA,
        "policy_refs": artifact_policy_refs(),
        "generated_at": repo_snapshot["generated_at"],
        "repo_facts": repo_snapshot["repo_facts"],
        "milestone_count": len(program.milestones),
        "issue_count": sum(len(m.issues) for m in program.milestones),
        "issue_counts_by_milestone": milestone_issue_count_map(program),
        "prerequisite_sequence": program.prerequisite_sequence,
        "recommended_sequence": program.recommended_sequence,
        "parallel_milestone_groups": program.parallel_groups,
    }
    count_snapshot_path = planning_dir / "count_snapshot.json"
    count_snapshot_path.write_text(json.dumps(count_snapshot, indent=2) + "\n", encoding="utf-8")

    readme = build_program_readme(program, str(count_snapshot_path.relative_to(ROOT)).replace("\\", "/"))
    (planning_dir / "README.md").write_text(readme, encoding="utf-8")
    if program.code == "cleanup_acceleration_program":
        (planning_dir / "cleanup_acceleration_program_summary.md").write_text(readme, encoding="utf-8")

    execution = {
        "program_code": program.code,
        "recommended_milestone_order": program.milestone_order,
        "parallel_milestone_groups": program.parallel_groups,
        "issue_counts_by_milestone": milestone_issue_count_map(program),
        "slot_map": {
            issue.code: i + 1
            for i, issue in enumerate(
                [issue for milestone in program.milestones for issue in milestone.issues]
            )
        },
    }
    (planning_dir / "execution_order.json").write_text(json.dumps(execution, indent=2) + "\n", encoding="utf-8")

    for milestone in program.milestones:
        (planning_dir / "milestones" / f"{milestone.code}.md").write_text(
            build_milestone_readme(program, milestone),
            encoding="utf-8",
        )
        for issue in milestone.issues:
            body = issue_body(program, milestone, issue, issue_index)
            (planning_dir / "issues" / f"{issue.code}.md").write_text(body, encoding="utf-8")
            manifest = issue_manifest(program, milestone, issue, issue_index)
            (publish_dir / "issue_bodies" / f"{issue.code}.json").write_text(
                json.dumps(manifest, indent=2) + "\n",
                encoding="utf-8",
            )

    program_manifest = {
        "program_code": program.code,
        "schema_version": "draft-backlog-program-manifest-v1",
        "schema_path": BACKLOG_PROGRAM_MANIFEST_SCHEMA,
        "title": program.title,
        "generated_at": repo_snapshot["generated_at"],
        "count_snapshot": str(count_snapshot_path.relative_to(ROOT)).replace("\\", "/"),
        "policy_refs": artifact_policy_refs(),
        "prerequisite_sequence": program.prerequisite_sequence,
        "recommended_sequence": program.recommended_sequence,
        "parallel_milestone_groups": program.parallel_groups,
        "milestones": [milestone_manifest(program, milestone, issue_index) for milestone in program.milestones],
    }
    (publish_dir / "program_manifest.json").write_text(json.dumps(program_manifest, indent=2) + "\n", encoding="utf-8")
    (publish_dir / "dependency_edges.json").write_text(json.dumps(dependency_graph(program, issue_index), indent=2) + "\n", encoding="utf-8")
    (publish_dir / "README.md").write_text(build_publish_readme(program), encoding="utf-8")


def make_issue(code: str, title: str, objective: str, stage: str, acceptance: list[str], deps: list[str] | None = None) -> IssueSpec:
    return IssueSpec(code=code, title=title, stage=stage, objective=objective, acceptance=acceptance, dependencies=deps or [])


def cleanup_program() -> ProgramSpec:
    ms: list[MilestoneSpec] = []

    ms.append(MilestoneSpec(
        code="M317",
        title="Backlog publication realignment and supersession hygiene",
        domain="domain:backlog-realignment",
        priority="priority:P0",
        area_labels=["area:planning", "area:workflow"],
        focus="reshape the locally drafted and future GitHub backlog before more work lands, so ambiguous ownership, stale blockers, and duplicated roadmap slices do not survive publication.",
        exit_gate="The backlog is publishable, direct blockers are accurate, overlapping work is superseded or merged deliberately, and no future roadmap packet depends on hardcoded GitHub IDs or stale publication conventions.",
        corrections=[
            "Treat backlog surgery as engineering work because bad issue structure slows delivery and hides missing work.",
            "Separate immediate publication-time realignment from longer-term governance so the necessary cleanup is not buried under later policy work.",
            "Use explicit direct blocker edges instead of broad transitive prose chains.",
        ],
        shared_surfaces=[
            "tmp/planning/ draft programs and milestone packets",
            "tmp/github-publish/ program manifests and issue JSON bodies",
            "issue templates, publication helpers, and maintainer-facing backlog docs",
            "dependency graphs and milestone-level sequencing metadata",
        ],
        blocked_by_milestones=[],
        publication_scope="internal-first",
        issues=[
            make_issue("M317-A001", "Backlog overlap, supersession, and publication-scope inventory", "Inventory the draft backlog, publication conventions, overlap zones, and supersession needs before any later backlog packet is treated as publishable.", "A", ["Identify every overlapping or duplicated milestone/issue surface and mark the intended owner."], []),
            make_issue("M317-B001", "Supersession, blocker, and publication policy", "Define the canonical rules for supersession, direct blockers, internal-first publication scope, and future GitHub creation so later packets are structurally consistent.", "B", ["Direct blockers are first-class and transitive blocker prose is removed from issue bodies."], ["M317-A001"]),
            make_issue("M317-B002", "Existing draft and backlog realignment implementation", "Rewrite the existing local backlog artifacts so they follow the canonical publication policy and no stale future-ID assumptions remain.", "B", ["Every draft packet consumes the same blocker semantics and publication-scope model."], ["M317-B001"]),
            make_issue("M317-C001", "Planning packet, manifest, and blocker-graph contract", "Define the stable manifest and publish-artifact structure used by every draft program and future GitHub publication helper.", "C", ["One canonical manifest shape exists for milestones, issues, labels, blockers, and counts."], ["M317-B001"]),
            make_issue("M317-D001", "Publish-artifact and consistency-audit implementation", "Generate consistent issue markdown, publish JSON, and blocker graphs from the canonical planning source of truth.", "D", ["The publish artifact set is regenerated from one source of truth and is self-consistent."], ["M317-B002", "M317-C001"]),
            make_issue("M317-E001", "Backlog realignment closeout gate", "Publish one closeout gate proving the draft backlog is structurally coherent enough to create on GitHub without manual surgery.", "E", ["All programs pass count, blocker, label, and publication-scope consistency checks."], ["M317-D001"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M313",
        title="Validation architecture, acceptance suites, and checker collapse",
        domain="domain:validation-consolidation",
        priority="priority:P0",
        area_labels=["area:validation", "area:tooling"],
        focus="replace checker-heavy validation sprawl with acceptance-first truth, a justified retained static guard set, shared executable harnesses, and an explicit migration model for legacy validation surfaces.",
        exit_gate="Validation truth comes primarily from executable subsystem suites and shared integration harnesses, retained static checks are justified, and legacy validation surfaces are quarantined or retired deliberately.",
        corrections=[
            "Stop treating raw checker count as the only victory condition; the real target is justified retained checks plus acceptance-first truth.",
            "Make migration explicit so legacy validation surfaces do not linger as undocumented parallel truth systems.",
            "Use generated counts and namespace inventories instead of hand-entered validation statistics.",
        ],
        shared_surfaces=[
            "scripts/ validation runners and retained policy guards",
            "tests/tooling/ and subsystem acceptance suites",
            "CI validation topology and workflow scheduling",
            "tmp/reports/ validation evidence layout",
            "docs/runbooks/ validation and operator contracts",
        ],
        blocked_by_milestones=["M317"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M313-A001", "Validation surface inventory, retained-checker justification, and migration map", "Measure the current validation surface, classify retained static guards by unique value, and define the migration path from checker-first to acceptance-first validation.", "A", ["Every retained static validation surface has a stated reason to exist and a named replacement or retention class."] ),
            make_issue("M313-B001", "Acceptance-first validation model and retained static guard policy", "Define the canonical test pyramid, retained static guard classes, and quarantine/deprecation policy for legacy validation surfaces.", "B", ["The repo has one validation model that distinguishes acceptance truth, integration truth, and retained policy guards."], ["M313-A001"]),
            make_issue("M313-B002", "Shared subsystem integration harness implementation", "Create or consolidate the shared harnesses that later acceptance suites and migration bridges will use instead of issue-local checkers.", "B", ["Subsystem validation runs through shared harnesses rather than milestone-specific scripts."], ["M313-B001"]),
            make_issue("M313-B003", "Legacy checker quarantine and namespace migration implementation", "Move legacy checker surfaces into explicit namespaces and quarantine buckets so they stop masquerading as primary truth.", "B", ["Legacy validation surfaces are classified as active, migration-only, archival, or prohibited."], ["M313-B001"]),
            make_issue("M313-C001", "Acceptance artifact schema and replay contract", "Define the machine-owned artifact contract for executable acceptance evidence, retained-checker reports, and migration status.", "C", ["Acceptance evidence is replayable and uses one canonical artifact schema."], ["M313-B001"]),
            make_issue("M313-C002", "Executable subsystem acceptance suite implementation", "Implement the shared executable acceptance suites that replace milestone-local validation scaffolds.", "C", ["Subsystem suites run through shared harnesses and produce canonical acceptance artifacts."], ["M313-B002", "M313-C001"]),
            make_issue("M313-C003", "Historical validation bridge and deprecation implementation", "Provide transitional compatibility only where needed while making the retirement path of legacy validation surfaces explicit.", "C", ["Any retained bridge is temporary, justified, and visible in generated reports."], ["M313-B003", "M313-C001"]),
            make_issue("M313-D001", "CI validation topology and scheduling contract", "Define how acceptance suites, integration harnesses, and retained static guards are scheduled in CI without reintroducing sprawl.", "D", ["CI scheduling follows the acceptance-first model and does not create a second validation taxonomy."], ["M313-C001"]),
            make_issue("M313-D002", "CI migration to acceptance-first validation implementation", "Move CI and normal operator flows onto the acceptance-first validation topology.", "D", ["Primary CI workflows consume shared acceptance artifacts and explicit retained-checker reports."], ["M313-C002", "M313-C003", "M313-D001"]),
            make_issue("M313-D003", "Validation budget and retained-checker reporting implementation", "Generate measured reports showing retained static guard count, acceptance coverage shape, and migration status using live repo queries.", "D", ["Count and classification claims are generated, not hand-maintained."], ["M313-D002"]),
            make_issue("M313-E001", "Validation consolidation closeout gate", "Publish one closeout gate proving that validation truth, retained static checks, and migration reporting now share one coherent model.", "E", ["The closeout gate fails if checker counts, classifications, or acceptance artifacts drift from generated truth."], ["M313-D002", "M313-D003"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M314",
        title="Command-surface reduction, dead-path removal, and workflow simplification",
        domain="domain:workflow-simplification",
        priority="priority:P0",
        area_labels=["area:workflow", "area:tooling"],
        focus="collapse the public command surface into a coherent set of canonical operator entrypoints, unify public orchestration, and retire dead or misleading workflow paths.",
        exit_gate="The public workflow surface is categorized and legible, dead paths are removed, and normal operator tasks no longer depend on ad hoc aliases or competing orchestration layers.",
        corrections=[
            "Use a canonical command taxonomy and budget instead of a brittle one-number script target.",
            "Push complexity behind parameterized runners rather than allowing every surface to remain publicly callable.",
            "Retire misleading prototype and compatibility aliases instead of leaving them documented forever.",
        ],
        shared_surfaces=[
            "package.json script surface",
            "scripts/ workflow runners and parameterized task entrypoints",
            "README.md and docs/runbooks/ operator workflows",
            "legacy or dead workflow paths under compiler/ and old helper surfaces",
            "CI and local operator entrypoints",
        ],
        blocked_by_milestones=["M317"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M314-A001", "Command surface inventory, public/internal split, and orchestration map", "Measure the public command surface, classify public versus internal entrypoints, and define the one orchestration model that normal workflows should consume.", "A", ["Every public entrypoint belongs to a canonical command category and has a clear public or internal role."] ),
            make_issue("M314-B001", "Public command taxonomy and orchestration policy", "Define the canonical public command categories, deprecation rules, and orchestration-layer policy for all operator-visible workflows.", "B", ["Public commands are categorized and documented by verb family rather than by historical script accumulation."], ["M314-A001"]),
            make_issue("M314-B002", "Public package script collapse and alias retirement implementation", "Collapse redundant public scripts and retire compatibility aliases that no longer deserve to be operator-facing.", "B", ["Alias retirement is deliberate and documented, not silent breakage."], ["M314-B001"]),
            make_issue("M314-B003", "Parameterized task-runner unification implementation", "Move retained complexity behind parameterized task runners and internal entrypoints so the public surface becomes smaller and clearer.", "B", ["The public surface shrinks because internal complexity is hidden behind canonical runners."], ["M314-B001"]),
            make_issue("M314-C001", "Public command contract and generated command appendix schema", "Define the machine-owned artifact contract for the public command surface and its generated documentation.", "C", ["The command appendix is generated from one canonical source of truth."], ["M314-B001"]),
            make_issue("M314-C002", "Task-runner and workflow API implementation", "Implement the canonical runner APIs and command-generation logic that back the public entrypoint surface.", "C", ["One canonical workflow API backs the generated public command surface."], ["M314-B003", "M314-C001"]),
            make_issue("M314-C003", "Command-budget enforcement and documentation sync implementation", "Add generated reporting and enforcement so command drift and documentation drift cannot silently return.", "C", ["Command counts and category drift are measured from the live repo, not edited by hand."], ["M314-C002"]),
            make_issue("M314-D001", "Maintainer and operator workflow integration implementation", "Move the live operator and maintainer workflows onto the canonical command surface and orchestration model.", "D", ["Normal workflows use canonical commands instead of historical aliases or direct internal scripts."], ["M314-C002"]),
            make_issue("M314-D002", "Prototype path retirement and compatibility cleanup implementation", "Remove misleading dead paths and narrow any temporary compatibility surfaces to explicit, short-lived bridges.", "D", ["Dead or ambiguous workflow paths are either removed or clearly marked as temporary compatibility bridges."], ["M314-B002", "M314-D001"]),
            make_issue("M314-E001", "Workflow simplification closeout gate", "Publish one closeout gate proving that the public command surface, generated appendix, and live operator flows now agree.", "E", ["The gate fails if the public command surface or workflow docs drift from generated truth."], ["M314-C003", "M314-D002"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M315",
        title="Source de-scaffolding, stable identifiers, artifact authenticity, and proof hygiene",
        domain="domain:source-hygiene-and-authenticity",
        priority="priority:P0",
        area_labels=["area:source", "area:provenance"],
        focus="remove milestone-era residue from product and generated truth surfaces, define stable identifiers, and make authenticity and provenance of proof artifacts machine-auditable.",
        exit_gate="Product and generated source-of-truth surfaces are free of milestone residue, authenticity classes are explicit, and proof artifacts carry provenance that distinguishes genuine generated outputs from synthetic fixtures or archives.",
        corrections=[
            "Ban milestone residue in product and generated truth surfaces without trying to erase legitimate archival planning history.",
            "Treat synthetic fixtures as valid only when explicitly labeled and fenced from genuine proof claims.",
            "Make stable feature-surface identifiers replace milestone-era labels instead of leaving ad hoc strings behind.",
        ],
        shared_surfaces=[
            "native/objc3c/src/ source comments, identifiers, and contract strings",
            "tracked proof artifacts such as .ll, .json, and other replay surfaces",
            "scripts/ provenance, authenticity, and policy guards",
            "docs/ and planning surfaces that distinguish archival references from product implementation",
            "CI policy and anti-noise enforcement surfaces",
        ],
        blocked_by_milestones=["M317"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M315-A001", "Milestone residue, authenticity, and provenance inventory", "Measure milestone-era residue and classify proof artifacts as genuine generated outputs, synthetic fixtures, or historical archives using live repo queries.", "A", ["Every surfaced residue or proof artifact falls into an explicit authenticity class."] ),
            make_issue("M315-B001", "Stable feature-surface identifier and authenticity policy", "Define the stable replacement identifiers and authenticity rules that future product and generated surfaces must follow.", "B", ["Stable identifiers replace milestone-coded product residue and authenticity rules are explicit."], ["M315-A001"]),
            make_issue("M315-B002", "Product and generated artifact decontamination implementation", "Remove milestone residue from product and generated truth surfaces while preserving legitimate archival references outside those surfaces.", "B", ["No milestone-coded residue remains in product or generated source-of-truth surfaces."], ["M315-B001"]),
            make_issue("M315-B003", "Synthetic-versus-genuine artifact classification implementation", "Apply explicit authenticity classes and labeling to replay and proof artifacts so later claims cannot blur them together.", "B", ["Synthetic fixtures and genuine proof artifacts are distinguishable by machine-owned metadata."], ["M315-B001"]),
            make_issue("M315-C001", "Artifact provenance schema and regeneration contract", "Define the canonical machine-owned provenance and regeneration contract for genuine replay and proof artifacts.", "C", ["Genuine proof artifacts are reproducible from a documented generation path."], ["M315-B001"]),
            make_issue("M315-C002", "Genuine replay artifact regeneration and provenance capture implementation", "Implement regeneration and provenance capture for genuine proof artifacts so claims about native outputs can be replayed.", "C", ["Genuine proof artifacts carry provenance and replay instructions."], ["M315-B002", "M315-C001"]),
            make_issue("M315-C003", "Synthetic fixture relocation and labeling implementation", "Move or relabel synthetic fixtures so they stop presenting as product proof while remaining usable for legitimate test purposes.", "C", ["Synthetic fixtures are fenced from proof claims and clearly identified."], ["M315-B003", "M315-C001"]),
            make_issue("M315-D001", "Anti-residue and proof-hygiene policy enforcement contract", "Define how the repo will mechanically prevent milestone residue and ambiguous proof artifacts from returning.", "D", ["Anti-residue and authenticity rules are enforced through mechanical policy rather than maintainer memory."], ["M315-C001"]),
            make_issue("M315-D002", "Hygiene enforcement and provenance audit implementation", "Implement the live enforcement and reporting path for residue removal and proof authenticity.", "D", ["Drift is reported against machine-generated inventories and provenance rules."], ["M315-C002", "M315-C003", "M315-D001"]),
            make_issue("M315-D003", "Archive boundary and historical-reference compatibility implementation", "Preserve legitimate archival planning references while keeping them out of product and generated truth surfaces.", "D", ["Historical context remains available without contaminating current implementation truth."], ["M315-B002"]),
            make_issue("M315-E001", "Source hygiene and authenticity closeout gate", "Publish one closeout gate proving that residue removal, authenticity classification, and provenance enforcement now share one coherent model.", "E", ["The gate fails if product surfaces, proof artifacts, or archival boundaries drift from measured truth."], ["M315-D002", "M315-D003"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M316",
        title="Runtime corrective tranche: realized dispatch, synthesized accessors, and native output truth",
        domain="domain:runtime-corrective-completion",
        priority="priority:P0",
        area_labels=["area:runtime", "area:codegen"],
        focus="close the immediate runtime/codegen gaps that must be fixed before broader envelope closure: realized dispatch over the object graph, synthesized accessor codegen and execution, and truthful native-output evidence.",
        exit_gate="The runtime has a real dispatch path over the realized object model, synthesized properties generate and execute real accessor code, and native-output evidence is tied to genuine compilation flows rather than synthetic proof theater.",
        corrections=[
            "Keep this milestone narrow as a corrective tranche rather than letting it absorb full object-model closure work.",
            "Make native-output truth a first-class track so proof artifacts cannot mask missing runtime or codegen behavior.",
            "Use this tranche to sharpen later envelope work instead of duplicating it.",
        ],
        shared_surfaces=[
            "native/objc3c/src/runtime/ selector lookup, dispatch, and realization surfaces",
            "native/objc3c/src/ir/ and lower/ property synthesis and accessor emission surfaces",
            "native/objc3c/src/pipeline/ and io/ native-output provenance surfaces",
            "tests/tooling/runtime/ executable probes and native IR/object evidence",
            "docs/spec/runtime claim surfaces that must match the live implementation",
        ],
        blocked_by_milestones=["M313", "M314", "M315"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M316-A001", "Runtime corrective tranche boundary, gap inventory, and non-goals", "Define the exact runtime/codegen gaps that belong in the corrective tranche and what remains intentionally deferred to later closure milestones.", "A", ["The corrective tranche has explicit non-goals and does not pretend to be full object-model closure."] ),
            make_issue("M316-B001", "Realized dispatch semantic model and workload map", "Define the live semantic model and proof workload for selector-pool-backed dispatch over the realized object graph.", "B", ["Dispatch claims are tied to executable workloads rather than narrative descriptions."], ["M316-A001"]),
            make_issue("M316-B002", "Property synthesis and storage-accessor semantic completion", "Define the semantic behavior of synthesized storage accessors that must be reflected in lowering and runtime execution.", "B", ["Synthesized accessor behavior is specified as live runtime behavior, not metadata alone."], ["M316-A001"]),
            make_issue("M316-B003", "Native output truthfulness and proof-authenticity policy", "Define what can and cannot count as native-output proof for this corrective tranche.", "B", ["Only genuinely reproduced native outputs can satisfy proof requirements."], ["M316-A001"]),
            make_issue("M316-B004", "Runtime corrective acceptance workload implementation", "Implement the executable workloads that later lowering and runtime changes must satisfy for dispatch and synthesized accessors.", "B", ["Acceptance workloads exist before final closeout claims are made."], ["M316-B001", "M316-B002"]),
            make_issue("M316-C001", "Dispatch/accessor lowering and provenance artifact contract", "Define the canonical artifact and provenance surfaces for dispatch and synthesized accessor evidence.", "C", ["Lowering artifacts and native proof outputs share one provenance contract."], ["M316-B003"]),
            make_issue("M316-C002", "Real method dispatch and selector-thunk lowering implementation", "Implement the lowering required to drive real method dispatch through the realized selector/object model.", "C", ["Dispatch lowering targets the live runtime path, not a milestone-local probe shim."], ["M316-B001", "M316-C001"]),
            make_issue("M316-C003", "Synthesized getter/setter LLVM IR generation implementation", "Implement real synthesized accessor code generation that targets executable runtime-backed storage behavior.", "C", ["Synthesized getter/setter IR is genuine generated output with replayable provenance."], ["M316-B002", "M316-C001"]),
            make_issue("M316-D001", "Executable proof and ABI contract for corrective closure", "Define the live runtime proof contract that dispatch and synthesized accessor work must satisfy.", "D", ["Executable proof requirements are explicit before final runtime claims are made."], ["M316-B004", "M316-C002", "M316-C003"]),
            make_issue("M316-D002", "Live realized dispatch runtime implementation", "Implement the live realized dispatch path over the actual object graph and selector pool.", "D", ["Dispatch mutates or queries live object state through real method bodies."], ["M316-D001", "M316-C002"]),
            make_issue("M316-D003", "Live synthesized accessor execution and reflection coherence implementation", "Implement executable synthesized accessors and ensure runtime reflection agrees with observed storage behavior.", "D", ["Reflection, synthesized accessors, and storage behavior agree under execution."], ["M316-D001", "M316-C003"]),
            make_issue("M316-E001", "Runtime corrective closeout gate", "Publish one closeout gate proving that dispatch, synthesized accessors, and native-output truth have actually landed.", "E", ["The corrective gate rejects synthetic proof artifacts for native-output claims."], ["M316-D002", "M316-D003"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M318",
        title="Governance hardening, anti-noise budgets, and sustainable progress enforcement",
        domain="domain:governance-and-sustainability",
        priority="priority:P0",
        area_labels=["area:governance", "area:policy"],
        focus="ratchet the cleaned-up repo shape after the corrective tranche so validation, workflow, hygiene, and proof surfaces do not drift back toward noise and ambiguity.",
        exit_gate="The repo has enforceable budgets, waiver rules, and regression reporting over the corrected surfaces, with anti-noise policy wired into normal maintainer practice.",
        corrections=[
            "Place governance after the corrective tranche so it ratchets the corrected shape instead of prematurely freezing a moving target.",
            "Treat exceptions as first-class objects with owners and expiry rather than hidden tribal knowledge.",
            "Make anti-noise policy measurable and replayable through generated reports.",
        ],
        shared_surfaces=[
            "CI and policy guards for scripts, validation surfaces, residue, and artifact authenticity",
            "planning docs and publication helpers that define how new work is added",
            "maintainer review checklists and contribution policy surfaces",
            "tmp/reports/ budgets, waivers, and anti-regression outcomes",
        ],
        blocked_by_milestones=["M316"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M318-A001", "Governance budget, waiver, and anti-noise inventory", "Measure the cleaned-up repo shape and define the budget surfaces that governance will ratchet going forward.", "A", ["Budgets are measured from live repo state rather than inherited from stale cleanup assumptions."] ),
            make_issue("M318-B001", "Sustainable progress policy and exception model", "Define the durable policy for budgets, waivers, expiry, and sustainable review over the corrected repo shape.", "B", ["Exceptions have owners, reasons, and expiry paths."], ["M318-A001"]),
            make_issue("M318-B002", "Maintainer review and regression handling implementation", "Implement the review and regression-handling surfaces that make the governance model part of normal engineering work.", "B", ["Governance shows up in normal maintainer flows instead of as an occasional cleanup ritual."], ["M318-B001"]),
            make_issue("M318-C001", "Governance automation and budget schema contract", "Define the machine-owned reporting contract for budgets, waivers, and anti-regression enforcement.", "C", ["Governance reports are generated from canonical machine-owned data."], ["M318-B001"]),
            make_issue("M318-C002", "Budget enforcement and waiver reporting implementation", "Implement the automated checks and generated reports that enforce budgets and track waivers.", "C", ["Budget drift and waiver drift are measured directly from the live repo."], ["M318-C001", "M318-B002"]),
            make_issue("M318-D001", "New-work proposal and publication workflow integration", "Integrate anti-noise governance into the workflow for adding new roadmap work and publication artifacts.", "D", ["New draft work is checked against budgets and waiver policy before publication."], ["M318-C002"]),
            make_issue("M318-D002", "Long-horizon anti-regression reporting implementation", "Generate long-horizon anti-regression reports that show whether the cleaned-up repo shape is holding over time.", "D", ["Governance evidence captures trend lines, not just one-time pass/fail results."], ["M318-C002"]),
            make_issue("M318-E001", "Governance hardening closeout gate", "Publish one closeout gate proving that the corrected repo shape is now protected by durable governance rather than wishful thinking.", "E", ["The gate fails if budgets, waivers, or new-work flows drift from the generated governance model."], ["M318-D001", "M318-D002"]),
        ],
    ))

    return ProgramSpec(
        code="cleanup_acceleration_program",
        title="Cleanup Acceleration And Runtime Corrective Program",
        planning_dir="cleanup_acceleration_program",
        publish_dir="cleanup_acceleration_program",
        prerequisite_sequence="M317 -> {M313, M314, M315 overlap} -> M316 -> M318",
        recommended_sequence="M317 -> {M313, M314, M315 overlap} -> M316 -> M318",
        rationale=[
            "Collapse validation, workflow, and provenance noise before later closure work compounds it.",
            "Correct the concrete runtime gaps that should not be left vague in later envelope milestones.",
            "Normalize publication, blocker, and anti-noise mechanics before more backlog is created.",
        ],
        notes=[
            "`M317` is an internal-first publication-shaping milestone even though it remains in the draft corpus.",
            "`M313`, `M314`, and `M315` are allowed to overlap once `M317` is complete.",
        ],
        milestone_order=["M317", "M313", "M314", "M315", "M316", "M318"],
        parallel_groups=[["M313", "M314", "M315"]],
        milestones=ms,
        include_repo_facts=True,
    )


def runtime_program() -> ProgramSpec:
    ms: list[MilestoneSpec] = []
    ms.append(MilestoneSpec(
        code="M319",
        title="Full object-model realization, property/ivar closure, and aggregate runtime reflection",
        domain="domain:object-model-runtime-closure",
        priority="priority:P0",
        area_labels=["area:runtime", "area:semantics"],
        focus="complete the live Objective-C 3.0 object model after the corrective tranche: class/metaclass realization, protocols, categories, properties, ivars, aggregate query surfaces, and reflection coherence over the real runtime graph.",
        exit_gate="The runtime realizes and executes the full live object-model graph, and reflection over classes, protocols, categories, properties, ivars, methods, and ownership metadata matches observed executable behavior.",
        corrections=[
            "Build on the realized dispatch and synthesized accessor corrective work instead of duplicating it.",
            "Treat reflection coherence as executable runtime behavior, not metadata garnish.",
            "Force class, protocol, category, property, ivar, and aggregate query behavior onto one canonical runtime truth surface.",
        ],
        shared_surfaces=[
            "native/objc3c/src/runtime/ realized class, category, protocol, property, and reflection surfaces",
            "native/objc3c/src/ir/ and lower/ metadata emission and descriptor lowering surfaces",
            "native/objc3c/src/pipeline/ manifest and runtime-registration surfaces",
            "tests/tooling/runtime/ object-model, property, and reflection probes",
            "docs/spec/runtime claim surfaces that must match the live runtime graph",
        ],
        blocked_by_milestones=["M316", "M318"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M319-A001", "Object-model closure boundary, gap inventory, and non-goals", "Define the exact remaining closure work for classes, metaclasses, protocols, categories, properties, ivars, and aggregate reflection after the corrective tranche.", "A", ["The milestone boundary distinguishes corrective work already closed in `M316` from broader object-model closure work."] ),
            make_issue("M319-B001", "Realized object graph and reflection semantic model", "Define the canonical runtime semantics for the realized object graph and the reflection queries that must agree with it.", "B", ["Reflection answers are specified against observed runtime behavior, not descriptor shape alone."], ["M319-A001"]),
            make_issue("M319-B002", "Loader ordering, category attachment, and conformance workload map", "Define the workload and fixture map needed to prove loader ordering, category attachment, and protocol conformance over the realized runtime graph.", "B", ["Later implementation work is driven by concrete workloads instead of broad narrative claims."], ["M319-B001"]),
            make_issue("M319-B003", "Class, metaclass, protocol, and category realization implementation", "Implement the remaining realized object graph behavior for classes, metaclasses, protocols, and categories.", "B", ["The runtime can realize and query the live class graph and attached behavior under execution."], ["M319-B001", "M319-B002"]),
            make_issue("M319-C001", "Object-model reflection artifact and runtime-registration contract", "Define the canonical artifact and runtime-registration contract for object-model and aggregate reflection evidence.", "C", ["Descriptor, registration, and reflection artifacts share one canonical contract."], ["M319-B001"]),
            make_issue("M319-C002", "Class, category, and protocol realization lowering implementation", "Implement the lowering and registration work that feeds the live realized object graph.", "C", ["Lowering targets the canonical runtime realization path."], ["M319-B003", "M319-C001"]),
            make_issue("M319-C003", "Property/ivar descriptor and aggregate reflection artifact implementation", "Implement the artifact and registration surfaces for properties, ivars, and aggregate runtime reflection.", "C", ["Aggregate reflection outputs are generated from the same runtime truth surface used at execution time."], ["M319-C001"]),
            make_issue("M319-D001", "Executable proof and ABI contract for full object-model closure", "Define the live proof and ABI obligations for the fully realized object model.", "D", ["Runtime proof obligations are explicit before final closeout claims are made."], ["M319-C002", "M319-C003"]),
            make_issue("M319-D002", "Live realized object-model runtime implementation", "Implement the live runtime behavior for the full realized object graph.", "D", ["Runtime object-graph behavior is observable through real execution and query surfaces."], ["M319-D001", "M319-C002"]),
            make_issue("M319-D003", "Live property/ivar/reflection closure implementation", "Implement live property, ivar, and aggregate reflection behavior that agrees with the runtime graph under execution.", "D", ["Property and ivar reflection behavior matches runtime-observed storage and ownership metadata."], ["M319-D001", "M319-C003"]),
            make_issue("M319-E001", "Object-model and reflection closeout gate", "Publish one closeout gate proving that the object model, properties/ivars, and reflection now share one runtime truth surface.", "E", ["The gate fails if reflected behavior and executed behavior diverge."], ["M319-D002", "M319-D003"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M320",
        title="Escaping block/byref execution, ownership transfer, and ARC automation",
        domain="domain:block-arc-runtime-closure",
        priority="priority:P0",
        area_labels=["area:memory", "area:semantics"],
        focus="finish executable block and ARC behavior after object-model closure: escaping block promotion, byref forwarding and copy/dispose helpers, owned weak and unowned captures, inserted lifetime operations, autorelease conventions, and interaction with properties, errors, and concurrency.",
        exit_gate="Escaping blocks, byref captures, and ARC-driven lifetime behavior are fully automated, executable, and coherent across properties, cleanup, and runtime ownership semantics.",
        corrections=[
            "Treat ARC as a real automated ownership transform rather than diagnostics plus debug snapshots.",
            "Force byref forwarding, block promotion, cleanup insertion, and property/error/concurrency interaction onto the same executable path.",
            "Build on the existing source and lowering surfaces without preserving them as proof-only contracts.",
        ],
        shared_surfaces=[
            "native/objc3c/src/sema/ ownership and capture legality surfaces",
            "native/objc3c/src/ir/ block lowering, ARC insertion, and cleanup emission surfaces",
            "native/objc3c/src/runtime/ block promotion, byref, weak, and ownership helper surfaces",
            "tests/tooling/runtime/ block and ARC runtime probes",
            "docs/spec/runtime claim surfaces for block capture and ARC behavior",
        ],
        blocked_by_milestones=["M319"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M320-A001", "Block/byref/ARC closure boundary, gap inventory, and non-goals", "Define the remaining executable block, byref, and ARC gaps after object-model closure, including the required interaction surfaces.", "A", ["The milestone boundary explicitly includes ARC interaction with properties, errors, and concurrency." ]),
            make_issue("M320-B001", "Escaping block, byref, and ownership semantic model", "Define the live semantic model for escaping blocks, byref forwarding, copy/dispose helpers, and ownership transfer.", "B", ["The semantic model covers promotion, forwarding, and ownership interaction under execution."], ["M320-A001"]),
            make_issue("M320-B002", "ARC automation and lifetime insertion policy", "Define the inserted lifetime operations, cleanup behavior, autorelease conventions, and fail-closed edges for ARC automation.", "B", ["ARC automation is specified as real inserted ownership behavior rather than advisory diagnostics."], ["M320-A001"]),
            make_issue("M320-B003", "Byref promotion, copy-dispose, and forwarding implementation", "Implement the byref cell behavior needed for escaping blocks and mutable captured state.", "B", ["Byref forwarding and helper behavior are observable on the live runtime path."], ["M320-B001"]),
            make_issue("M320-C001", "Block/ARC lowering and runtime ABI contract", "Define the canonical lowering and ABI contract for block promotion, byref helpers, ARC lifetime insertion, and cleanup.", "C", ["Lowering and runtime helper expectations are captured in one canonical contract."], ["M320-B001", "M320-B002"]),
            make_issue("M320-C002", "Escaping block promotion and byref lowering implementation", "Implement the lowering required to drive escaping block promotion and byref helper behavior on the live runtime path.", "C", ["Lowering targets the live runtime ABI rather than a proof-only path."], ["M320-B003", "M320-C001"]),
            make_issue("M320-C003", "ARC lifetime insertion and cleanup lowering implementation", "Implement ARC lifetime insertion, weak/unowned behavior, and cleanup lowering over the real runtime surface.", "C", ["ARC lowering composes with cleanup, properties, and later concurrency/error work."], ["M320-B002", "M320-C001"]),
            make_issue("M320-D001", "Executable proof and ABI contract for block/ARC closure", "Define the live proof obligations for escaping blocks, byref behavior, and ARC automation over the real runtime.", "D", ["Proof obligations explicitly cover properties, errors, and concurrency interaction."], ["M320-C002", "M320-C003"]),
            make_issue("M320-D002", "Live escaping block and byref runtime implementation", "Implement the live runtime behavior for escaping blocks and byref cells.", "D", ["Escaping blocks and byref mutation behave correctly under execution rather than in proof-only helper paths."], ["M320-D001", "M320-C002"]),
            make_issue("M320-D003", "Live ARC automation and ownership closure implementation", "Implement the live runtime closure of ARC automation, ownership transfer, and weak/unowned handling.", "D", ["ARC-inserted ownership behavior is live, automated, and coherent with the runtime."], ["M320-D001", "M320-C003"]),
            make_issue("M320-E001", "Block/ARC closeout gate", "Publish one closeout gate proving that escaping blocks, byref behavior, and ARC automation now execute over one coherent runtime path.", "E", ["The gate rejects paths that still depend on debug-only or proof-only ownership helpers."], ["M320-D002", "M320-D003"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M321",
        title="Throws, cleanup, bridged errors, and executable propagation closure",
        domain="domain:error-runtime-closure",
        priority="priority:P0",
        area_labels=["area:errors", "area:abi"],
        focus="turn the existing error semantics and lowering contracts into a fully executable runtime path: thrown values, try/throw/catch execution, cleanup and unwind behavior, NSError/status/Result bridging, and cross-module ABI-safe propagation.",
        exit_gate="Errors and throws run end to end through the real runtime and ABI surfaces, including cleanup, bridging, unwind ordering, and cross-module propagation, without deferred-runtime caveats.",
        corrections=[
            "Convert existing throw/error packets into a real executable path rather than a deferred-runtime promise.",
            "Treat bridged error behavior as part of the live ABI story instead of a compile-time legality check.",
            "Make unwind ordering, cleanup, ARC interaction, and cross-module propagation explicit rather than implied.",
        ],
        shared_surfaces=[
            "native/objc3c/src/sema/ throws, bridge, and cleanup semantic surfaces",
            "native/objc3c/src/ir/ throw, catch, unwind, and bridge-helper lowering surfaces",
            "native/objc3c/src/runtime/ error storage, cleanup, and bridge helper surfaces",
            "tests/tooling/runtime/ error runtime probes and executable integration fixtures",
            "docs/spec/runtime claim surfaces for errors, throws, and bridging",
        ],
        blocked_by_milestones=["M320"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M321-A001", "Error runtime closure boundary, gap inventory, and non-goals", "Define the remaining runtime closure work for throws, cleanup, unwind ordering, bridging, and cross-module propagation.", "A", ["The milestone boundary explicitly includes unwind ordering and cross-module propagation." ]),
            make_issue("M321-B001", "Error propagation, unwind ordering, and cleanup semantic model", "Define the live semantic model for thrown values, cleanup scopes, unwind ordering, and catch behavior.", "B", ["Throw/catch and cleanup semantics are expressed as runtime behavior rather than source-level intent."], ["M321-A001"]),
            make_issue("M321-B002", "Bridged error, cross-module propagation, and compatibility policy", "Define the compatibility rules for bridged errors and cross-module propagation across ABI boundaries.", "B", ["Bridged errors and cross-module behavior have a narrow, truthful compatibility model."], ["M321-A001"]),
            make_issue("M321-B003", "Throws ABI and helper semantics implementation", "Implement the helper and ABI semantics required to drive real throw/catch/cleanup behavior.", "B", ["Throws support is grounded in stable helper and ABI semantics rather than placeholder flags."], ["M321-B001", "M321-B002"]),
            make_issue("M321-C001", "Error lowering and runtime artifact contract", "Define the canonical artifact contract for lowering, bridge helpers, and runtime error evidence.", "C", ["Lowering and runtime evidence share one canonical artifact contract."], ["M321-B002"]),
            make_issue("M321-C002", "Throws/catch/cleanup lowering implementation", "Implement lowering for try/throw/catch, unwind cleanup, and error propagation over the real runtime path.", "C", ["Lowering reflects unwind ordering and cleanup behavior that actually executes."], ["M321-B003", "M321-C001"]),
            make_issue("M321-C003", "Bridge helpers and cross-module propagation artifact implementation", "Implement the bridging and cross-module artifact surfaces that later runtime integration and claimability work will consume.", "C", ["Cross-module propagation behavior is captured in canonical machine-owned evidence."], ["M321-B002", "M321-C001"]),
            make_issue("M321-D001", "Live throws/catch/cleanup runtime implementation", "Implement the live runtime behavior for throw, catch, cleanup, and unwind ordering.", "D", ["Throw/catch/cleanup now execute on the real runtime path without deferred-runtime caveats."], ["M321-C002"]),
            make_issue("M321-D002", "Live bridged error propagation and interop runtime implementation", "Implement the live runtime behavior for bridged errors and cross-module propagation.", "D", ["Bridged error propagation behaves consistently across module boundaries."], ["M321-C003", "M321-D001"]),
            make_issue("M321-E001", "Error runtime closeout gate", "Publish one closeout gate proving that throws, cleanup, unwind ordering, and bridged propagation now execute over one coherent runtime path.", "E", ["The gate rejects paths that still depend on deferred-runtime or bridge-only proof behavior."], ["M321-D001", "M321-D002"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M322",
        title="Async/task/actor runtime execution, scheduling, and isolation closure",
        domain="domain:concurrency-runtime-closure",
        priority="priority:P0",
        area_labels=["area:concurrency", "area:runtime"],
        focus="complete runtime-backed concurrency semantics: continuations, tasks, executors, cancellation, actor mailboxes, cross-actor hops, isolation enforcement, sendability, and interaction with ARC and thrown-error cleanup.",
        exit_gate="Async/task/actor features execute over a real runtime scheduler and isolation model, and the emitted lowering and metadata surfaces describe the same behavior users can observe at runtime.",
        corrections=[
            "Treat scheduler, executor, and isolation semantics as runtime responsibilities rather than helper-token placeholders.",
            "Keep task runtime and actor runtime as distinct internal tracks even though they close under one milestone.",
            "Require explicit interaction with ARC, cleanup, and interop instead of closing concurrency in isolation.",
        ],
        shared_surfaces=[
            "native/objc3c/src/sema/ async, task, actor, and sendability surfaces",
            "native/objc3c/src/ir/ continuation, task, and actor lowering surfaces",
            "native/objc3c/src/runtime/ task scheduler, continuation, executor, and actor runtime surfaces",
            "tests/tooling/runtime/ continuation, task, and actor probes",
            "docs/spec/runtime claim surfaces for async, tasks, actors, and isolation",
        ],
        blocked_by_milestones=["M321"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M322-A001", "Concurrency runtime closure boundary, gap inventory, and non-goals", "Define the remaining runtime closure work for continuations, tasks, executors, actors, isolation, sendability, and their interaction surfaces.", "A", ["The milestone boundary keeps task runtime and actor runtime explicit as separate internal tracks." ]),
            make_issue("M322-B001", "Async/task/actor runtime semantic model", "Define the live semantic model for continuations, task lifecycle, executors, actor mailboxes, and isolation behavior.", "B", ["Concurrency semantics are expressed as runtime behavior users can observe."], ["M322-A001"]),
            make_issue("M322-B002", "Scheduler, executor, cancellation, and workload policy", "Define the scheduler/executor behavior and workload map that later implementation must satisfy.", "B", ["Scheduler and executor behavior are tied to concrete workloads and failure-mode handling."], ["M322-A001"]),
            make_issue("M322-B003", "Continuation and task lifecycle implementation", "Implement the live semantic behavior for continuations, tasks, cancellation, and lifecycle state changes.", "B", ["Task lifecycle behavior is observable and deterministic on the live runtime path."], ["M322-B001", "M322-B002"]),
            make_issue("M322-B004", "Actor isolation, sendability, and hop semantics implementation", "Implement the live semantic behavior for actor isolation, sendability checks, and cross-actor hops.", "B", ["Actor isolation and sendability behavior are explicit and fail closed where required."], ["M322-B001", "M322-B002"]),
            make_issue("M322-C001", "Concurrency lowering and runtime ABI contract", "Define the canonical lowering and ABI contract for continuations, tasks, executors, actors, and isolation metadata.", "C", ["Lowering and runtime expectations for concurrency are captured in one canonical contract."], ["M322-B001"]),
            make_issue("M322-C002", "Continuation and task lowering implementation", "Implement lowering for continuations, tasks, cancellation, and executor interaction.", "C", ["Task lowering targets the real runtime scheduler/executor path."], ["M322-B003", "M322-C001"]),
            make_issue("M322-C003", "Actor metadata, isolation, and hop lowering implementation", "Implement lowering for actor metadata, isolation boundaries, and cross-actor hop behavior.", "C", ["Actor lowering targets the real runtime actor model rather than placeholder tokens."], ["M322-B004", "M322-C001"]),
            make_issue("M322-D001", "Live async/task/executor runtime implementation", "Implement the live runtime behavior for continuations, tasks, scheduling, and executors.", "D", ["Tasks and executors run on the real runtime path and interact correctly with cleanup and ownership."], ["M322-C002"]),
            make_issue("M322-D002", "Live actor runtime and isolation closure implementation", "Implement the live runtime behavior for actors, isolation enforcement, sendability, and cross-actor hops.", "D", ["Actor isolation and cross-actor behavior are observable and coherent under execution."], ["M322-C003", "M322-D001"]),
            make_issue("M322-E001", "Concurrency runtime closeout gate", "Publish one closeout gate proving that async/task/actor behavior now executes over one coherent runtime scheduler and isolation model.", "E", ["The gate rejects placeholder-token concurrency behavior that lacks real runtime execution semantics."], ["M322-D001", "M322-D002"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M323",
        title="Metaprogramming, property-behavior runtime materialization, and broader interop closure",
        domain="domain:metaprogramming-interop-closure",
        priority="priority:P0",
        area_labels=["area:macros", "area:interop"],
        focus="finish the remaining modeled-but-not-runtime-complete feature families: macro/package/provenance execution semantics, property behaviors, runtime materialization of generated behavior, import/export and foreign-boundary interop, and cross-module compatibility over the real toolchain.",
        exit_gate="Metaprogramming, property behaviors, and supported interop surfaces are runtime-backed, packageable, and cross-module stable enough to support real code instead of comparison-only guidance.",
        corrections=[
            "Keep metaprogramming/property-behavior closure and interop closure as explicit internal tracks even though they share one milestone.",
            "Treat property behaviors as runtime-observable behavior rather than compile-time decoration.",
            "Force interop claims through ABI, ownership, error, and async compatibility on the real packaged toolchain.",
        ],
        shared_surfaces=[
            "native/objc3c/src/sema/ metaprogramming and interop semantic surfaces",
            "native/objc3c/src/ir/ macro, property-behavior, and bridge artifact lowering surfaces",
            "native/objc3c/src/pipeline/ runtime import, host-cache, and cross-module artifact surfaces",
            "native/objc3c/src/runtime/ metaprogramming and bridge helper runtime surfaces",
            "tests/tooling/runtime/ metaprogramming and interop executable probes",
        ],
        blocked_by_milestones=["M322"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M323-A001", "Metaprogramming/property-behavior/interop boundary, gap inventory, and non-goals", "Define the remaining runtime closure work for macro execution, property behaviors, runtime materialization, and broader interop.", "A", ["The milestone boundary makes the metaprogramming/property-behavior track distinct from the interop track." ]),
            make_issue("M323-B001", "Macro provenance, package trust, and runtime-cache policy", "Define the canonical policy for macro provenance, package trust boundaries, and runtime cache behavior.", "B", ["Macro and package behavior is bounded by an explicit trust model."], ["M323-A001"]),
            make_issue("M323-B002", "Property-behavior executable semantic model", "Define the runtime-observable semantic model for property behaviors and generated behavior materialization.", "B", ["Property behaviors are specified as executed runtime behavior, not decoration."], ["M323-A001"]),
            make_issue("M323-B003", "Interop ABI, ownership, error, and async boundary policy", "Define the supported interop boundary model across ABI, ownership, error propagation, and async interaction.", "B", ["Interop claims are narrow, truthful, and tied to real boundary semantics."], ["M323-A001"]),
            make_issue("M323-C001", "Metaprogramming/property-behavior artifact contract", "Define the canonical machine-owned artifact contract for macro materialization and property-behavior runtime evidence.", "C", ["Macro/property-behavior evidence is machine-owned and replayable."], ["M323-B001", "M323-B002"]),
            make_issue("M323-C002", "Property-behavior and macro runtime materialization implementation", "Implement the live materialization path for macro-backed behavior and property behaviors.", "C", ["Generated behavior is materially observable on the live runtime path."], ["M323-C001"]),
            make_issue("M323-C003", "Interop bridge and cross-module artifact implementation", "Implement the canonical interop artifact surfaces for bridges, import/export behavior, and packaged compatibility evidence.", "C", ["Interop evidence is generated from the real packaged toolchain path."], ["M323-B003"]),
            make_issue("M323-D001", "Live property-behavior and macro-backed runtime implementation", "Implement the live runtime behavior for property-behavior materialization and macro-backed generated behavior.", "D", ["Property behaviors and macro-backed behavior execute on the real runtime path."], ["M323-C002"]),
            make_issue("M323-D002", "Live interop runtime closure implementation", "Implement the live runtime closure for the supported interop boundary behavior.", "D", ["Interop behavior is coherent across ABI, ownership, error, and async boundaries."], ["M323-C003"]),
            make_issue("M323-D003", "Packaged cross-module interop proof and compatibility implementation", "Prove supported interop behavior over real packaged, cross-module workflows instead of repo-local comparison surfaces.", "D", ["Interop claims are backed by packaged cross-module evidence rather than comparison-only guidance."], ["M323-D002"]),
            make_issue("M323-E001", "Metaprogramming and interop closeout gate", "Publish one closeout gate proving that macro/property-behavior closure and interop closure are both runtime-backed and packageable.", "E", ["The gate demotes any feature family that still depends on host-cache snapshots or comparison-only evidence."], ["M323-D001", "M323-D003"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M324",
        title="Full-envelope conformance, stability, and production claimability",
        domain="domain:full-envelope-claimability",
        priority="priority:P0",
        area_labels=["area:conformance", "area:release"],
        focus="use the completed runtime and language surfaces to earn production-strength completeness and stability claims: one support matrix, one claim taxonomy, one demotion model, and integrated evidence across conformance, stress, performance, external replay, packaging, updates, and distribution trust.",
        exit_gate="The project can make narrow but production-strength claims about the full intended language/runtime envelope because supported, experimental, unsupported, and release-blocking behavior are explicit and the integrated evidence stack proves the claims.",
        corrections=[
            "Turn the existing validation, performance, release, and credibility machinery into an envelope-level claim system.",
            "Force one support matrix and demotion model across the whole intended language/runtime surface.",
            "Make production-strength claims contingent on integrated evidence rather than milestone language alone.",
        ],
        shared_surfaces=[
            "docs/runbooks/ public command, conformance, performance, release, and credibility surfaces",
            "scripts/ public workflow runner, reporting, packaging, update, and trust-report surfaces",
            "tests/conformance, stress, and external-validation corpora",
            "tmp/reports/ and tmp/artifacts/ release-evidence surfaces",
            "README/site/tutorial/showcase/support-matrix claim surfaces",
        ],
        blocked_by_milestones=["M323"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M324-A001", "Full-envelope support matrix, claim taxonomy, and demotion model", "Define the canonical support matrix, claim taxonomy, and explicit demotion model for supported, experimental, unsupported, and release-blocking behavior.", "A", ["Later closeout work has an explicit path to demote claims that are not actually earned."] ),
            make_issue("M324-B001", "Production-strength claim and support-window policy", "Define the conditions under which the project can make production-strength claims about the full intended envelope.", "B", ["Public claims are coupled to explicit support classes and support-window rules."], ["M324-A001"]),
            make_issue("M324-B002", "Release-blocker, completeness-gate, and rollout criteria", "Define the release-blocking taxonomy and rollout criteria that decide whether full-envelope claims can ship.", "B", ["Release blockers are explicit and grounded in the support matrix."], ["M324-A001"]),
            make_issue("M324-B003", "Stability regression and rollout criteria implementation", "Implement the regression and rollout criteria surfaces that later evidence packaging and closeout will consume.", "B", ["Claimability work includes a concrete regression and rollout discipline."], ["M324-B001", "M324-B002"]),
            make_issue("M324-C001", "Full-envelope dashboard and reporting contract", "Define the canonical dashboard and reporting contract for full-envelope claimability evidence.", "C", ["One reporting contract spans conformance, stress, performance, packaging, updates, and trust evidence."], ["M324-B001", "M324-B002"]),
            make_issue("M324-C002", "Cross-surface acceptance matrix and claim publication implementation", "Implement the integrated acceptance matrix and publication surfaces that express full-envelope claimability.", "C", ["Claim publication is generated from integrated evidence instead of milestone prose."], ["M324-C001", "M324-B003"]),
            make_issue("M324-D001", "Full-envelope soak, stress, and external-validation integration", "Integrate soak, stress, conformance, and external replay evidence into one live full-envelope claim path.", "D", ["The claim path is fed by integrated live evidence rather than per-slice reports only."], ["M324-C002"]),
            make_issue("M324-D002", "Release-candidate compatibility and stability evidence packaging", "Package the release-candidate evidence that backs full-envelope claimability and demotion decisions.", "D", ["Release-candidate claimability is bounded by explicit compatibility and stability evidence."], ["M324-D001"]),
            make_issue("M324-E001", "Production-readiness closeout gate", "Publish one closeout gate proving exactly which parts of the full intended envelope are supported, experimental, unsupported, or release-blocking.", "E", ["The closeout gate demotes unsupported claims instead of stretching the evidence."], ["M324-D002"]),
        ],
    ))

    return ProgramSpec(
        code="runtime_envelope_completion_program",
        title="Runtime Envelope Completion Program",
        planning_dir="runtime_envelope_completion_program",
        publish_dir="runtime_envelope_completion_program",
        prerequisite_sequence="M317 -> {M313, M314, M315 overlap} -> M316 -> M318",
        recommended_sequence="M317 -> {M313, M314, M315 overlap} -> M316 -> M318 -> M319 -> M320 -> M321 -> M322 -> M323 -> M324",
        rationale=[
            "Finish the runtime and semantic closure work that the current docs still describe as incomplete.",
            "Build on the cleanup/corrective tranche instead of duplicating it.",
            "Turn modeled source and lowering boundaries into fully executable runtime behavior across the intended language/runtime envelope.",
        ],
        notes=[
            "`M323` remains one milestone but is rewritten around two explicit internal tracks: metaprogramming/property-behavior closure and interop closure.",
        ],
        milestone_order=["M319", "M320", "M321", "M322", "M323", "M324"],
        parallel_groups=[],
        milestones=ms,
    )


def post_program() -> ProgramSpec:
    ms: list[MilestoneSpec] = []

    ms.append(MilestoneSpec(
        code="M325",
        title="Developer tooling, LSP, formatting, and debugger integration",
        domain="domain:developer-tooling-integration",
        priority="priority:P1",
        area_labels=["area:tooling", "area:ux"],
        focus="turn the completed language/runtime surface into a usable development experience with one authoritative CLI-to-editor contract, language-server capabilities, formatting, navigation, debugger symbol/stepping support, and trustworthy diagnostics.",
        exit_gate="A serious user can edit, navigate, format, inspect, and debug Objective-C 3 projects without dropping to ad hoc internal scripts for normal workflows.",
        corrections=[
            "Treat editor/debugger support as a product surface rather than a nice-to-have after semantic closure.",
            "Scope the work tightly to LSP, formatting, diagnostics, and debugger integration instead of sprawling into bespoke editor plugins.",
            "Keep the implementation tied to real compiler/runtime outputs rather than editor-only shadow parsers or metadata.",
        ],
        shared_surfaces=[
            "scripts/ public workflow runner, inspect commands, and developer-tooling surfaces",
            "native/objc3c/src/ diagnostics, source mapping, and debug-info emission surfaces",
            "docs/runbooks/ developer-tooling and operator contracts",
            "editor-facing protocol, formatting, and navigation tooling under scripts/ and tmp/reports/",
            "tests/tooling/ and workspace drills for editor/debugger behavior",
        ],
        blocked_by_milestones=["M324"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M325-A001", "Developer tooling boundary, capability map, and gap inventory", "Inventory the live developer tooling gaps after full-envelope closure and define the canonical editor/debugger capability map.", "A", ["The milestone boundary covers LSP, formatting, diagnostics, and debugger integration rather than generic editor sprawl."] ),
            make_issue("M325-B001", "Diagnostics, formatting, and symbol-resolution policy", "Define the canonical diagnostics, formatting, and symbol-resolution behavior expected by users and editor clients.", "B", ["Editor-visible semantics are grounded in the real compiler output model."], ["M325-A001"]),
            make_issue("M325-B002", "Language-server capability and fallback policy", "Define the LSP capability model and truthful fallback behavior when a capability is unavailable.", "B", ["LSP claims stay narrower than the actual implementation and fail closed cleanly."], ["M325-A001"]),
            make_issue("M325-B003", "Debugger, source-map, and stepping semantics", "Define how debug info, stepping, breakpoints, and source mapping behave over the real toolchain outputs.", "B", ["Debugger behavior is tied to real emitted artifacts, not assumptions."], ["M325-A001"]),
            make_issue("M325-C001", "Editor protocol and debug artifact contract", "Define the canonical machine-owned artifact contract for LSP, formatter, and debugger integration surfaces.", "C", ["Tooling evidence and protocol behavior are captured by one canonical contract."], ["M325-B001", "M325-B002", "M325-B003"]),
            make_issue("M325-C002", "Language-server and navigation implementation", "Implement the live language-server and navigation path against the real compiler/runtime model.", "C", ["Navigation and symbol resolution rely on canonical compiler-produced data."], ["M325-C001"]),
            make_issue("M325-C003", "Formatter and source-map/debug-info implementation", "Implement formatter stability plus source-map and debug-info generation over the real toolchain.", "C", ["Formatter and debugger behavior are reproducible and tied to emitted artifacts."], ["M325-C001"]),
            make_issue("M325-D001", "Runnable workspace editor and debug integration implementation", "Prove the live editor/debugger path over runnable workspaces and real projects.", "D", ["A real workspace can use the canonical editor/debugger flows without internal-only helpers."], ["M325-C002", "M325-C003"]),
            make_issue("M325-D002", "CLI-to-editor contract and packaged tooling integration", "Integrate the live editor/debugger surface with the packaged and documented public workflow model.", "D", ["CLI, packaged tooling, and editor integrations tell the same truth."], ["M325-D001"]),
            make_issue("M325-E001", "Developer tooling closeout gate", "Publish one closeout gate proving that LSP, formatting, diagnostics, and debugger behavior are live and coherent.", "E", ["The gate rejects editor claims that are not backed by real compiler/runtime outputs."], ["M325-D002"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M326",
        title="Cross-platform, toolchain-matrix, and compatibility hardening",
        domain="domain:platform-toolchain-hardening",
        priority="priority:P1",
        area_labels=["area:platform", "area:compatibility"],
        focus="prove the toolchain and runtime on the host and compiler matrix that a serious language project actually needs: OS/arch variants, LLVM/Clang ranges, package/install channel compatibility, and deterministic fallback behavior when a platform is outside support.",
        exit_gate="The supported host/toolchain matrix is narrow but real, verified, tiered, and enforced through the same public packaging and validation surfaces users rely on.",
        corrections=[
            "Use support tiers instead of a flat supported/unsupported binary.",
            "Force install, packaging, and runtime behavior to agree across supported hosts instead of treating portability as only a compile concern.",
            "Make unsupported-host failure paths explicit so widening support later remains truthful.",
        ],
        shared_surfaces=[
            "packaging, installer, release, and update workflows under scripts/",
            "native/objc3c build, runtime, and toolchain probing surfaces",
            "docs/runbooks/ supported-platform and operator compatibility contracts",
            "tmp/reports/ platform matrix and install smoke evidence",
            "tests/tooling/ host/toolchain compatibility and package smoke drills",
        ],
        blocked_by_milestones=["M325"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M326-A001", "Supported platform/toolchain matrix and gap inventory", "Measure the real host/toolchain coverage and define the tiered support matrix the project is prepared to claim.", "A", ["Support claims are tied to a tiered, measured host/toolchain matrix." ]),
            make_issue("M326-B001", "Platform support tiers and compatibility claim policy", "Define Tier 1, Tier 2, experimental, and unsupported platform/toolchain claims.", "B", ["Support claims are narrow, tiered, and measurable."], ["M326-A001"]),
            make_issue("M326-B002", "Unsupported-host fail-closed and fallback semantics", "Define deterministic behavior when a host or toolchain falls outside the supported matrix.", "B", ["Unsupported-host behavior is explicit and non-deceptive."], ["M326-A001"]),
            make_issue("M326-B003", "Toolchain-range and archive compatibility semantics", "Define the compatibility rules for toolchain ranges, packaged archives, and install/update surfaces across the matrix.", "B", ["Packaging and install compatibility is part of the support matrix, not a separate story."], ["M326-A001"]),
            make_issue("M326-C001", "Platform matrix artifact and schema contract", "Define the canonical machine-owned artifact contract for platform-matrix and compatibility evidence.", "C", ["Support-tier evidence is machine-owned and replayable."], ["M326-B001", "M326-B002", "M326-B003"]),
            make_issue("M326-C002", "Cross-platform build/package validation implementation", "Implement the live build and package validation path for the supported matrix.", "C", ["Supported-platform validation uses the same public package/install flows users consume."], ["M326-C001"]),
            make_issue("M326-C003", "Toolchain-range replay and compatibility evidence implementation", "Implement replayable evidence for toolchain-range and compatibility claims across the support tiers.", "C", ["Toolchain-range claims are backed by generated evidence."], ["M326-C001"]),
            make_issue("M326-D001", "Runnable cross-platform install-matrix integration", "Prove the live build, package, install, and smoke path across the supported host/toolchain matrix.", "D", ["Supported matrix claims are backed by runnable install and smoke evidence."], ["M326-C002", "M326-C003"]),
            make_issue("M326-D002", "Support-tier publication and packaged smoke integration", "Integrate the tiered support matrix into the public package, install, and documentation surfaces.", "D", ["Tiered support claims and packaged smoke proofs agree across user-facing surfaces."], ["M326-D001"]),
            make_issue("M326-E001", "Platform hardening closeout gate", "Publish one closeout gate proving that the supported host/toolchain matrix is real, tiered, and package-verified.", "E", ["The gate demotes unsupported platform claims instead of overextending the matrix."], ["M326-D002"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M327",
        title="Security hardening, macro trust, and supply-chain resilience",
        domain="domain:security-hardening",
        priority="priority:P1",
        area_labels=["area:security", "area:supply-chain"],
        focus="harden the now-complete toolchain against operational failure modes: macro/package trust boundaries, artifact integrity, installer/update abuse, release-key handling, runtime memory-safety regressions, and executable response workflows.",
        exit_gate="The project has a published security posture with meaningful hardening, auditable trust boundaries, and repeatable response and publication workflows.",
        corrections=[
            "Treat macro execution, package provenance, installer/update paths, and runtime behavior as one security surface instead of isolated concerns.",
            "Make security policy executable through checks, reports, and publication workflows rather than prose-only assurances.",
            "Add clear advisory and disclosure outputs so response work is visible and repeatable.",
        ],
        shared_surfaces=[
            "macro, package, provenance, and host-cache surfaces under native/objc3c and scripts/",
            "release foundation, packaging channels, release operations, and distribution credibility workflows",
            "runtime safety probes and hardening diagnostics",
            "security runbooks, trust reports, advisory, and response-policy surfaces",
            "tmp/reports/ security posture, hardening, and audit evidence",
        ],
        blocked_by_milestones=["M326"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M327-A001", "Security posture, trust-boundary, and gap inventory", "Inventory the real security posture and trust boundaries across macros, packages, installers, updates, runtime safety, and release publication.", "A", ["Security work is scoped against an explicit trust-boundary inventory." ]),
            make_issue("M327-B001", "Security response and disclosure policy", "Define the project's advisory, disclosure, and incident-response policy for the now-complete toolchain.", "B", ["Response behavior is explicit enough to support real operator expectations."], ["M327-A001"]),
            make_issue("M327-B002", "Macro/package/provenance trust semantics", "Define the executable trust semantics for macro execution, package provenance, and supply-chain validation.", "B", ["Macro and package trust claims are bounded by explicit semantics."], ["M327-A001"]),
            make_issue("M327-B003", "Installer/update/release-key hardening semantics", "Define the hardening and trust semantics for installers, updates, release-key handling, and abuse resistance.", "B", ["Release and update trust surfaces are part of the same hardening model."], ["M327-A001"]),
            make_issue("M327-C001", "Security artifact and reporting contract", "Define the canonical artifact contract for hardening, advisory, disclosure, and trust evidence.", "C", ["Security posture and advisory evidence are machine-owned and replayable."], ["M327-B001", "M327-B002", "M327-B003"]),
            make_issue("M327-C002", "Supply-chain audit and hardening checks implementation", "Implement the hardening checks and audit paths that enforce the security posture on the live toolchain.", "C", ["Hardening checks run on the real package/release/runtime surfaces."], ["M327-C001"]),
            make_issue("M327-C003", "Security publication and advisory evidence implementation", "Implement the publication surfaces that expose security posture, advisories, and hardening evidence truthfully.", "C", ["Security publication uses the same canonical evidence contract as the hardening checks."], ["M327-C001"]),
            make_issue("M327-D001", "Runnable security drill and release-response integration", "Prove the live response and hardening path over the real release/update surfaces.", "D", ["The security response path is executable, not just described."], ["M327-C002", "M327-C003"]),
            make_issue("M327-D002", "Runtime hardening and memory-safety regression integration", "Integrate runtime hardening and regression evidence into the live security posture.", "D", ["Runtime hardening is part of the same executable security program."], ["M327-D001"]),
            make_issue("M327-E001", "Security hardening closeout gate", "Publish one closeout gate proving that the security posture, hardening, and response workflows are real and auditable.", "E", ["The gate rejects security claims that are not backed by live hardening and response evidence."], ["M327-D002"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M330",
        title="Testing framework, templates, and canonical application architecture surfaces",
        domain="domain:application-architecture-and-testing",
        priority="priority:P1",
        area_labels=["area:ergonomics", "area:applications"],
        focus="give users a credible way to build real software with the language: first-party test harnesses, project templates, fixture/app architecture guidance, canonical medium-sized applications, and reproducible example workspaces that show how the language should actually be used.",
        exit_gate="The project has first-party testing and application-structure guidance backed by runnable templates and canonical example applications rather than only showcase slices and probes.",
        corrections=[
            "Move beyond showcase slices into canonical application and test architecture people can actually copy.",
            "Use canonical apps and templates to pressure later package and adoption decisions.",
            "Keep examples runnable and packageable through the same public workflows as the rest of the toolchain.",
        ],
        shared_surfaces=[
            "templates, example workspaces, and first-party testing tooling",
            "stdlib/program, tutorial, and showcase surfaces",
            "package/dependency and release/install integration paths",
            "docs/tutorials/ and runbooks for application structure and testing",
            "tests/tooling/ and real app smoke suites",
        ],
        blocked_by_milestones=["M327"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M330-A001", "Application architecture, testing, and template gap inventory", "Inventory the missing first-party testing, template, and canonical-application surfaces needed for real-user workflows.", "A", ["The milestone boundary is shaped by real user workflows, not just showcase demos." ]),
            make_issue("M330-B001", "First-party testing and fixture semantics", "Define the canonical first-party testing behavior, fixture rules, and test-runner expectations for Objective-C 3 projects.", "B", ["Testing semantics are credible enough to support real project work."], ["M330-A001"]),
            make_issue("M330-B002", "Project template and workspace semantics", "Define the canonical project-template and workspace structure for real Objective-C 3 projects.", "B", ["Templates are opinionated enough to anchor later package and migration guidance."], ["M330-A001"]),
            make_issue("M330-B003", "Canonical application architecture and layering semantics", "Define the canonical medium-sized application structure the project will recommend and support.", "B", ["Canonical apps reflect real layering, not toy examples."], ["M330-A001"]),
            make_issue("M330-C001", "Testing and template artifact contract", "Define the canonical artifact contract for testing frameworks, templates, and canonical app evidence.", "C", ["Template and testing artifacts are machine-owned and replayable."], ["M330-B001", "M330-B002", "M330-B003"]),
            make_issue("M330-C002", "First-party test harness and template implementation", "Implement the first-party test harness and project templates over the real toolchain.", "C", ["Templates and testing surfaces are runnable on the canonical toolchain path."], ["M330-C001"]),
            make_issue("M330-C003", "Canonical application workspace and evidence implementation", "Implement the canonical medium-sized application workspaces and evidence surfaces.", "C", ["Canonical apps are replayable, packageable, and suitable for later adoption work."], ["M330-C001"]),
            make_issue("M330-D001", "Runnable templates and canonical app integration", "Prove the live template and canonical app path over the real package/install workflow.", "D", ["Real users can clone the canonical path instead of stitching together showcases."], ["M330-C002", "M330-C003"]),
            make_issue("M330-D002", "Package/install/dependency integration for canonical apps", "Ensure canonical applications are the surfaces that pressure package, install, and dependency integration decisions.", "D", ["Canonical apps exercise the later package ecosystem under realistic conditions."], ["M330-D001"]),
            make_issue("M330-E001", "Application architecture and testing closeout gate", "Publish one closeout gate proving that templates, first-party testing, and canonical apps are all live and coherent.", "E", ["The gate rejects example paths that cannot be run through the canonical user workflow."], ["M330-D002"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M329",
        title="Package manager, registry, and dependency workflow ecosystem",
        domain="domain:package-ecosystem",
        priority="priority:P1",
        area_labels=["area:modules", "area:ecosystem"],
        focus="create a real dependency story around Objective-C 3 modules and packages: local workspace semantics, lockfiles, provenance-aware resolution, offline mirrors, package authoring, and only then hosted registry/publication behavior.",
        exit_gate="Users can publish, consume, lock, and reproduce Objective-C 3 packages through a coherent dependency workflow rather than bespoke repo-local conventions.",
        corrections=[
            "Design local package/workspace semantics before hosted registry behavior.",
            "Keep provenance, offline support, and reproducibility attached to dependency management from the start.",
            "Let canonical apps and templates pressure the dependency model before broad registry ambitions take over.",
        ],
        shared_surfaces=[
            "stdlib, package, release, and update metadata surfaces",
            "dependency-resolution and package-authoring tooling under scripts/",
            "registry and lockfile schemas plus offline mirror support",
            "docs/tutorials/ and runbooks for package authoring and consumption",
            "tests/tooling/ dependency, reproducibility, and mirror drills",
        ],
        blocked_by_milestones=["M330"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M329-A001", "Local package/workspace semantics and dependency gap inventory", "Inventory the missing package and dependency surfaces starting from local workspace behavior rather than hosted registry behavior.", "A", ["The milestone boundary privileges local package/workspace truth before public registry ambition." ]),
            make_issue("M329-B001", "Dependency resolution, provenance, and lock policy", "Define the canonical dependency-resolution and lock behavior for Objective-C 3 packages.", "B", ["Dependency resolution and locking are narrow, deterministic, and provenance-aware."], ["M329-A001"]),
            make_issue("M329-B002", "Local workspace, lockfile, and offline mirror semantics", "Define the semantics for local package workspaces, lockfiles, and offline mirrors.", "B", ["Offline and local package behavior are first-class, not later extras."], ["M329-A001"]),
            make_issue("M329-B003", "Registry and package publication semantics", "Define the semantics for public package publication and registry behavior only after the local dependency model is clear.", "B", ["Registry behavior is layered on top of a proven local package model."], ["M329-B001", "M329-B002"]),
            make_issue("M329-C001", "Package ecosystem artifact and schema contract", "Define the canonical machine-owned artifact contract for package, lockfile, mirror, and registry evidence.", "C", ["Package ecosystem evidence is generated from one canonical artifact contract."], ["M329-B001", "M329-B002", "M329-B003"]),
            make_issue("M329-C002", "Local dependency workflow and package authoring tooling implementation", "Implement the local package/workspace, dependency-resolution, and authoring path over the real toolchain.", "C", ["The first implementation target is the local and reproducible package workflow."], ["M329-C001"]),
            make_issue("M329-C003", "Registry/mirror integration and reproducibility evidence implementation", "Implement the registry, mirror, and reproducibility evidence path on top of the proven local package model.", "C", ["Registry and mirror claims are tied to generated evidence rather than assumptions."], ["M329-C002"]),
            make_issue("M329-D001", "Runnable package workflow integration", "Prove the live package workflow using real templates and canonical apps.", "D", ["Package workflows are validated against real user-shaped workspaces, not synthetic package-only probes."], ["M329-C002"]),
            make_issue("M329-D002", "Packaged dependency and offline mirror integration", "Prove the live offline and packaged dependency workflow, including mirror behavior and reproducibility.", "D", ["Offline and packaged dependency behavior are real parts of the user workflow."], ["M329-C003", "M329-D001"]),
            make_issue("M329-E001", "Package ecosystem closeout gate", "Publish one closeout gate proving that the dependency model, local package path, and registry/mirror path are coherent and reproducible.", "E", ["The gate rejects registry claims that are not earned by the local package model and runnable evidence."], ["M329-D002"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M328",
        title="Compatibility maintenance, migrations, soak, and long-horizon operations",
        domain="domain:long-horizon-operations",
        priority="priority:P1",
        area_labels=["area:versioning", "area:operations"],
        focus="move from milestone closure to sustained product operation: version-support maintenance, compatibility drift detection, migrations, long-running soak, rollback discipline, deprecation control, and supportable release cadence over time.",
        exit_gate="The project can maintain compatibility and stability over time rather than only proving a one-time complete release snapshot.",
        corrections=[
            "Focus this milestone on compatibility, deprecation, migration, rollback, and aging evidence rather than inventing dependency semantics.",
            "Treat soak and aging evidence as a real operational requirement once package and application flows exist.",
            "Use the completed package and canonical-app surfaces as the substrate for long-horizon operations work.",
        ],
        shared_surfaces=[
            "release operations, versioning, update, and rollback workflows",
            "performance, conformance, stress, and external-validation evidence surfaces",
            "docs/runbooks/ support-window, migration, and operator procedures",
            "tmp/reports/ soak, rollout, compatibility, and aging evidence",
            "tests/tooling/ long-horizon replay and migration drills",
        ],
        blocked_by_milestones=["M329"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M328-A001", "Compatibility maintenance, migrations, and soak gap inventory", "Inventory the remaining long-horizon compatibility, migration, rollback, and soak gaps once packages and canonical apps are live.", "A", ["The milestone boundary is grounded in real package/app/operator workflows rather than abstract maintenance goals." ]),
            make_issue("M328-B001", "Deprecation and compatibility maintenance policy", "Define the durable policy for deprecation, compatibility maintenance, support windows, and explicit drift handling.", "B", ["Compatibility maintenance is expressed as a long-horizon policy rather than a one-release promise."], ["M328-A001"]),
            make_issue("M328-B002", "Migration, rollback, and support-window semantics", "Define the executable semantics for migration, rollback, version drift handling, and support-window behavior.", "B", ["Migration and rollback are operator-visible behaviors, not one-off release chores."], ["M328-A001"]),
            make_issue("M328-B003", "Aging-regression and release-cadence criteria", "Define the criteria for soak duration, aging regressions, and supportable release cadence.", "B", ["Long-horizon operations are bounded by explicit aging and cadence criteria."], ["M328-A001"]),
            make_issue("M328-C001", "Compatibility maintenance artifact and schema contract", "Define the canonical artifact contract for migration, rollback, soak, and aging evidence.", "C", ["Long-horizon maintenance evidence is machine-owned and replayable."], ["M328-B001", "M328-B002", "M328-B003"]),
            make_issue("M328-C002", "Migration replay, soak, and aging evidence implementation", "Implement the evidence generation path for migrations, rollbacks, soak, and aging regressions.", "C", ["Long-horizon claims are backed by generated evidence from the real package/app workflows."], ["M328-C001"]),
            make_issue("M328-D001", "Runnable soak, migration, and rollback integration", "Prove the live migration, rollback, and soak paths over the real package and application ecosystem.", "D", ["Long-horizon operations are exercised on the real user-facing workflow surfaces."], ["M328-C002"]),
            make_issue("M328-D002", "Support-window publication and long-horizon operator integration", "Integrate support-window, migration, and aging evidence into the public and operator-facing surfaces.", "D", ["Compatibility maintenance is visible in the actual release and operator surfaces."], ["M328-D001"]),
            make_issue("M328-E001", "Long-horizon operations closeout gate", "Publish one closeout gate proving that compatibility maintenance and soak behavior are durable, not one-time milestone theater.", "E", ["The gate rejects compatibility and support claims that are not backed by long-horizon evidence."], ["M328-D002"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M331",
        title="Adoption program, migration playbooks, and ecosystem legibility",
        domain="domain:adoption-and-legibility",
        priority="priority:P1",
        area_labels=["area:docs", "area:adoption"],
        focus="turn technical completeness into practical adoption: migration playbooks from ObjC2 and adjacent ecosystems, capability narratives, performance and interoperability guidance, educational paths, and ecosystem-facing documentation that explains when and why to choose Objective-C 3.",
        exit_gate="A serious external evaluator can understand the language, compare it honestly, migrate incrementally, and judge fit without relying on private maintainer context.",
        corrections=[
            "Treat adoption as a disciplined product/documentation program rather than scattered tutorials and showcase blurbs.",
            "Base migration and evaluator guidance on the real outputs from canonical apps, package workflows, and support claims.",
            "Keep public comparison language narrower than the actual support matrix and evidence.",
        ],
        shared_surfaces=[
            "README, site, tutorials, migration guides, showcase, and stdlib program docs",
            "performance, interoperability, and support-matrix reporting surfaces",
            "canonical applications, templates, and package ecosystem workflows",
            "docs/runbooks/ and public command surfaces where external users need legible entry points",
            "tmp/reports/ adoption, migration, and documentation evidence",
        ],
        blocked_by_milestones=["M328"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M331-A001", "Adoption, migration, and evaluator gap inventory", "Inventory the remaining adoption and evaluator gaps once templates, packages, and long-horizon support surfaces are live.", "A", ["Adoption work is grounded in actual user-facing package/app/support surfaces." ]),
            make_issue("M331-B001", "Public adoption and migration claim policy", "Define the truthful public-claim rules for adoption, migration, and evaluator-facing guidance.", "B", ["Adoption claims are bounded by support classes and evidence."], ["M331-A001"]),
            make_issue("M331-B002", "Capability narrative and comparison semantics", "Define how the language will compare itself to adjacent ecosystems without slipping into marketing-only language.", "B", ["Comparison guidance is evidence-backed and support-matrix-aware."], ["M331-A001"]),
            make_issue("M331-B003", "Migration playbook and interop guidance semantics", "Define the canonical migration and interop guidance model for real evaluators and adopters.", "B", ["Migration playbooks are grounded in real package/app/tooling behavior."], ["M331-A001"]),
            make_issue("M331-C001", "Adoption and migration artifact contract", "Define the canonical artifact contract for migration guides, comparison matrices, and evaluator evidence.", "C", ["Adoption and migration materials are machine-owned enough to stay in sync with live support claims."], ["M331-B001", "M331-B002", "M331-B003"]),
            make_issue("M331-C002", "Migration guides, comparison matrix, and onboarding implementation", "Implement the migration, comparison, and onboarding surfaces using the real package/app/tooling outputs from earlier milestones.", "C", ["Onboarding and migration guidance are derived from live, runnable surfaces."], ["M331-C001"]),
            make_issue("M331-D001", "Runnable onboarding and migration integration", "Prove the live onboarding and migration path over canonical apps, packages, and supported workflows.", "D", ["An external evaluator can follow one canonical path without relying on internal-only context."], ["M331-C002"]),
            make_issue("M331-D002", "Evaluator evidence and canonical path integration", "Integrate evaluator evidence into the canonical public path so the language is legible from the outside.", "D", ["Evaluator-facing evidence reflects the same supported path used by real users."], ["M331-D001"]),
            make_issue("M331-E001", "Adoption and legibility closeout gate", "Publish one closeout gate proving that migration, comparison, and onboarding guidance are truthful, runnable, and externally legible.", "E", ["The gate rejects adoption language that outruns the real support and evidence surface."], ["M331-D002"]),
        ],
    ))

    ms.append(MilestoneSpec(
        code="M332",
        title="Governance, extension lifecycle, and ecosystem sustainability",
        domain="domain:governance-and-sustainability",
        priority="priority:P1",
        area_labels=["area:governance", "area:ecosystem"],
        focus="make the project sustainable once people depend on it: extension/RFC workflow, compatibility review, package ecosystem governance, contributor and maintainer operating model, and policy surfaces that prevent the completed language from decaying into ad hoc evolution.",
        exit_gate="The language and ecosystem have a durable governance and evolution model that can handle extensions, compatibility pressure, releases, and outside contribution without losing coherence.",
        corrections=[
            "Codify governance from the real package, support, security, and adoption surfaces rather than inventing it in a vacuum.",
            "Treat governance as an engineering surface with executable review paths and machine-owned evidence.",
            "Keep extension and compatibility review coupled to the actual release and support model already established.",
        ],
        shared_surfaces=[
            "governance docs, RFC/extension flows, and compatibility review policies",
            "package ecosystem and support-matrix claim surfaces",
            "release, update, and security response workflows",
            "contributor, maintainer, and extension-author guidance under docs/",
            "tmp/reports/ governance, process, and review evidence",
        ],
        blocked_by_milestones=["M331"],
        publication_scope="public-roadmap",
        issues=[
            make_issue("M332-A001", "Governance, extension lifecycle, and process gap inventory", "Inventory the remaining governance and extension-lifecycle gaps once package, adoption, support, and security surfaces are live.", "A", ["Governance work is grounded in the already-implemented product and ecosystem surfaces." ]),
            make_issue("M332-B001", "Extension/RFC/compatibility review policy", "Define the canonical policy for extensions, RFCs, compatibility review, and language evolution.", "B", ["Language evolution is bounded by an explicit compatibility review model."], ["M332-A001"]),
            make_issue("M332-B002", "Maintainer, contributor, and package-governance semantics", "Define the operating model for maintainers, contributors, and package ecosystem governance.", "B", ["Governance semantics cover both language evolution and ecosystem stewardship."], ["M332-A001"]),
            make_issue("M332-C001", "Governance artifact and policy contract", "Define the canonical machine-owned artifact contract for governance, extension review, and stewardship evidence.", "C", ["Governance evidence is structured enough to remain synchronized with the live process."], ["M332-B001", "M332-B002"]),
            make_issue("M332-C002", "Extension workflow and review tooling implementation", "Implement the live workflow and tooling that executes the governance and extension review model.", "C", ["Governance runs through executable review tooling rather than prose-only process."], ["M332-C001"]),
            make_issue("M332-D001", "Runnable review and extension integration", "Prove the live review and extension path on the real project surfaces.", "D", ["Governance is exercised on live workflows rather than hypothetical examples."], ["M332-C002"]),
            make_issue("M332-D002", "Governance publication and stewardship evidence implementation", "Integrate governance and stewardship evidence into the public-facing process surfaces.", "D", ["Governance publication reflects the actual review and stewardship process."], ["M332-D001"]),
            make_issue("M332-E001", "Governance and sustainability closeout gate", "Publish one closeout gate proving that the finished language and ecosystem have a durable, executable governance model.", "E", ["The gate rejects governance claims that are not backed by live review tooling and stewardship evidence."], ["M332-D002"]),
        ],
    ))

    return ProgramSpec(
        code="post_m324_adoption_program",
        title="Post-M324 Durability And Adoption Program",
        planning_dir="post_m324_adoption_program",
        publish_dir="post_m324_adoption_program",
        prerequisite_sequence="M317 -> {M313, M314, M315 overlap} -> M316 -> M318 -> M319 -> M320 -> M321 -> M322 -> M323 -> M324",
        recommended_sequence="M317 -> {M313, M314, M315 overlap} -> M316 -> M318 -> M319 -> M320 -> M321 -> M322 -> M323 -> M324 -> M325 -> M326 -> M327 -> M330 -> M329 -> M328 -> M331 -> M332",
        rationale=[
            "Capture the work that still matters after full language/runtime closure.",
            "Do necessary durability work first, then ecosystem/adoption work, while letting canonical apps pressure package and adoption design.",
            "Produce publish-ready drafts that already carry real label names and direct blocker metadata for future GitHub creation.",
        ],
        notes=[
            "Necessary durability work first: `M325`, `M326`, `M327`, `M330`, `M329`, `M328`.",
            "Adoption and sustainability closeout second: `M331`, `M332`.",
        ],
        milestone_order=["M325", "M326", "M327", "M330", "M329", "M328", "M331", "M332"],
        parallel_groups=[],
        milestones=ms,
    )


def write_master_summary(programs: list[ProgramSpec], snapshot: dict[str, Any]) -> None:
    total_milestones = sum(len(p.milestones) for p in programs)
    total_issues = sum(sum(len(m.issues) for m in p.milestones) for p in programs)
    lines = [
        "# Draft Backlog Master Summary",
        "",
        f"- generated_at: `{snapshot['generated_at']}`",
        f"- total_milestones: `{total_milestones}`",
        f"- total_issues: `{total_issues}`",
        f"- package.json scripts: `{snapshot['repo_facts']['package_scripts']}`",
        f"- check_*.py files: `{snapshot['repo_facts']['check_py_files']}`",
        f"- test_check_*.py files: `{snapshot['repo_facts']['test_check_py_files']}`",
        f"- m2xx refs in native src: `{snapshot['repo_facts']['m2xx_refs_in_native_src']}`",
        f"- tracked .ll files: `{snapshot['repo_facts']['tracked_ll_files']}`",
        f"- tracked stub .ll files: `{snapshot['repo_facts']['tracked_stub_ll_files']}`",
        f"- compiler/objc3c/semantic.py present: `{snapshot['repo_facts']['prototype_semantic_surface_present']}`",
        "",
        "## Publication rules",
    ]
    lines.extend(f"- {item}" for item in publication_rules_lines())
    lines.extend([
        "",
        "## Programs",
    ])
    for program in programs:
        issue_count = sum(len(m.issues) for m in program.milestones)
        lines.append(f"- `{program.code}`: `{len(program.milestones)}` milestones, `{issue_count}` issues")
    (PLANNING / "draft_backlog_master_summary.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (PLANNING / "draft_backlog_master_snapshot.json").write_text(
        json.dumps(
            {
                "schema_version": "draft-backlog-master-snapshot-v1",
                "schema_path": BACKLOG_MASTER_SNAPSHOT_SCHEMA,
                "policy_refs": artifact_policy_refs(),
                "generated_at": snapshot["generated_at"],
                "repo_facts": snapshot["repo_facts"],
                "total_milestones": total_milestones,
                "total_issues": total_issues,
                "programs": {
                    program.code: {
                        "milestones": len(program.milestones),
                        "issues": sum(len(m.issues) for m in program.milestones),
                        "milestone_order": program.milestone_order,
                    }
                    for program in programs
                },
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )


def main() -> None:
    snapshot = repo_fact_snapshot()
    write_backlog_contract_files()
    programs = [cleanup_program(), runtime_program(), post_program()]
    for program in programs:
        write_program(program, snapshot)
    write_master_summary(programs, snapshot)


if __name__ == "__main__":
    main()
