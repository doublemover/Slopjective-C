#!/usr/bin/env python3
"""Unified public workflow runner for the live objc3c command surface."""

from __future__ import annotations

import hashlib
import json
import os
import re
import subprocess
import sys
from dataclasses import asdict
from datetime import datetime, timezone
from pathlib import Path
from time import perf_counter
from typing import Sequence

SCRIPT_ROOT = Path(__file__).resolve().parents[1]
ROOT = Path(__file__).resolve().parents[2]
for import_root in (ROOT, SCRIPT_ROOT):
    if str(import_root) not in sys.path:
        sys.path.insert(0, str(import_root))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.subprocesses import run_capture
from objc3c_tooling.public_workflow_output import extract_public_workflow_report_paths as extract_report_paths

from scripts.objc3c_workflow.action_spec import ActionHandler, ActionSpec
from scripts.objc3c_workflow.actions.docs import (
    action_build_native_docs,
    action_build_public_command_contract,
    action_build_public_command_surface,
    action_build_site,
    action_check_documentation_surface,
    action_check_markdown,
    action_check_native_docs,
    action_check_public_command_budget,
    action_check_public_command_contract,
    action_check_public_command_surface,
    action_check_site,
    action_format_markdown,
    action_lint_markdown,
    action_validate_documentation_surface,
)
from scripts.objc3c_workflow.actions.external_validation import (
    action_check_external_validation_surface,
    action_publish_external_repro_corpus,
    action_test_external_validation_replay,
    action_validate_external_validation,
    action_validate_external_validation_integration,
)
from scripts.objc3c_workflow.actions.native_build import (
    BUILD_PS1,
    action_build_native_binaries,
    action_build_native_contracts,
    action_build_native_full,
    action_build_native_reconfigure,
    action_compile_objc3c,
    action_package_runnable_toolchain,
    action_proof_objc3c,
)
from scripts.objc3c_workflow.actions.performance import (
    action_benchmark_comparative_baselines,
    action_benchmark_compiler_throughput,
    action_benchmark_performance,
    action_benchmark_runtime_inspector,
    action_benchmark_runtime_performance,
    action_build_performance_dashboard,
    action_check_performance_governance_schema_surface,
    action_check_performance_governance_surface,
    action_publish_performance_report,
    action_validate_compiler_throughput,
    action_validate_performance_foundation,
    action_validate_performance_governance,
    action_validate_performance_governance_end_to_end,
    action_validate_performance_governance_integration,
    action_validate_runnable_compiler_throughput,
    action_validate_runnable_performance,
    action_validate_runnable_runtime_performance,
    action_validate_runtime_performance,
)
from scripts.objc3c_workflow.actions.runtime_tests import (
    action_proof_runtime_architecture,
    action_test_runtime_acceptance,
    action_test_runtime_acceptance_block_arc,
    action_test_runtime_acceptance_concurrency,
    action_test_runtime_acceptance_cross_module,
    action_test_runtime_acceptance_diagnostics,
    action_test_runtime_acceptance_fast,
    action_validate_block_arc_conformance,
    action_validate_concurrency_conformance,
    action_validate_error_conformance,
    action_validate_interop_conformance,
    action_validate_metaprogramming_conformance,
    action_validate_object_model_conformance,
    action_validate_release_candidate_conformance,
    action_validate_runnable_block_arc,
    action_validate_runnable_bootstrap,
    action_validate_runnable_concurrency,
    action_validate_runnable_error,
    action_validate_runnable_interop,
    action_validate_runnable_metaprogramming,
    action_validate_runnable_object_model,
    action_validate_runnable_release_candidate,
    action_validate_runnable_storage_reflection,
    action_validate_runtime_architecture,
    action_validate_storage_reflection_conformance,
)
from scripts.objc3c_workflow.actions.release_governance import (
    action_build_distribution_credibility_dashboard,
    action_build_package_channels,
    action_build_platform_support_matrix,
    action_build_public_conformance_scorecard,
    action_build_release_manifest,
    action_build_security_posture,
    action_build_update_manifest,
    action_check_distribution_credibility_surface,
    action_check_packaging_channels_surface,
    action_check_public_conformance_reporting_surface,
    action_check_release_foundation_surface,
    action_check_release_operations_surface,
    action_check_security_hardening_surface,
    action_publish_distribution_credibility,
    action_publish_public_conformance_report,
    action_publish_release_operations,
    action_publish_release_provenance,
    action_publish_security_advisories,
    action_validate_distribution_credibility,
    action_validate_distribution_credibility_end_to_end,
    action_validate_packaging_channels,
    action_validate_packaging_channels_end_to_end,
    action_validate_platform_hardening,
    action_validate_platform_hardening_end_to_end,
    action_validate_public_conformance_reporting,
    action_validate_public_conformance_reporting_end_to_end,
    action_validate_public_conformance_reporting_integration,
    action_validate_release_foundation,
    action_validate_release_operations,
    action_validate_release_operations_end_to_end,
    action_validate_security_hardening,
    action_validate_security_hardening_end_to_end,
)
from scripts.objc3c_workflow.actions.schema_surfaces import (
    action_check_distribution_credibility_schema_surface,
    action_check_packaging_channels_schema_surface,
    action_check_public_conformance_schema_surface,
    action_check_release_foundation_schema_surface,
    action_check_release_operations_schema_surface,
    action_check_security_hardening_schema_surface,
)
from scripts.objc3c_workflow.actions.stress import (
    action_check_stress_surface,
    action_test_fuzz_safety,
    action_test_lowering_runtime_stress,
    action_test_mixed_module_differential,
    action_test_stress_crash_triage,
    action_test_stress_minimization,
    action_validate_stress,
    action_validate_stress_end_to_end,
    action_validate_stress_integration,
)
from scripts.objc3c_workflow.actions.validation_timing import (
    action_inspect_validation_timing,
    collect_child_timing,
    load_latest_report_payload,
    load_surface_from_report,
    validation_budget_violations,
    validation_speed_budget_mode,
)
from scripts.objc3c_workflow.commands import extract_output_line, pwsh_file, run, workflow_command
from scripts.objc3c_workflow.environment import (
    PWSH,
    WORKFLOW_COMMAND_TEXT,
    WORKFLOW_MODULE,
    WORKFLOW_RUNNER_MODE,
    WORKFLOW_RUNNER_SURFACE,
)
from scripts.objc3c_workflow.npm_surface import describe_package_script_payload
from scripts.objc3c_workflow.registry import ACTION_SPECS
from scripts.objc3c_workflow.reports import emit_json

COMPILE_WRAPPER_SELF_AUDIT_PY = (
    ROOT / "scripts" / "check_objc3c_compile_wrapper_self_audit.py"
)
SMOKE_PS1 = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_PS1 = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
RECOVERY_PS1 = ROOT / "scripts" / "check_objc3c_native_recovery_contract.ps1"
MATRIX_PS1 = ROOT / "scripts" / "run_objc3c_native_fixture_matrix.ps1"
NEGATIVE_EXPECTATIONS_PS1 = ROOT / "scripts" / "check_objc3c_negative_fixture_expectations.ps1"
REPO_SUPERCLEAN_SURFACE_PY = ROOT / "scripts" / "check_repo_superclean_surface.py"
SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
SHOWCASE_RUNTIME_PS1 = ROOT / "scripts" / "check_showcase_runtime.ps1"
SHOWCASE_INTEGRATION_PY = ROOT / "scripts" / "check_showcase_integration.py"
RUNNABLE_SHOWCASE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_showcase_end_to_end.py"
GETTING_STARTED_INTEGRATION_PY = ROOT / "scripts" / "check_getting_started_integration.py"
DEVELOPER_TOOLING_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_developer_tooling_integration.py"
RUNNABLE_DEVELOPER_TOOLING_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_developer_tooling_end_to_end.py"
EDITOR_TOOLING_SURFACE_PY = ROOT / "scripts" / "build_objc3c_editor_tooling_surface.py"
FORMAT_OBJC3C_SOURCE_PY = ROOT / "scripts" / "format_objc3c_source.py"
BONUS_EXPERIENCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_bonus_experience_integration.py"
CONFORMANCE_CORPUS_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_conformance_corpus_integration.py"
RUNNABLE_CONFORMANCE_CORPUS_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_conformance_corpus_end_to_end.py"
STDLIB_SURFACE_PY = ROOT / "scripts" / "check_stdlib_surface.py"
MATERIALIZE_STDLIB_PY = ROOT / "scripts" / "materialize_objc3c_stdlib_workspace.py"
STDLIB_FOUNDATION_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_foundation_integration.py"
STDLIB_ADVANCED_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_advanced_integration.py"
STDLIB_PROGRAM_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_program_integration.py"
RUNNABLE_STDLIB_FOUNDATION_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_foundation_end_to_end.py"
RUNNABLE_STDLIB_ADVANCED_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_advanced_end_to_end.py"
RUNNABLE_STDLIB_PROGRAM_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_program_end_to_end.py"
PROJECT_TEMPLATE_MATERIALIZER_PY = ROOT / "scripts" / "materialize_objc3c_project_template.py"
CANONICAL_APPLICATION_WORKSPACE_MATERIALIZER_PY = ROOT / "scripts" / "materialize_objc3c_canonical_application_workspace.py"
APPLICATION_ARCHITECTURE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_application_architecture_integration.py"
RUNNABLE_APPLICATION_ARCHITECTURE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_application_architecture_end_to_end.py"
PACKAGE_LOCK_PY = ROOT / "scripts" / "build_objc3c_package_lock.py"
PACKAGE_AUTHORING_WORKFLOW_PY = ROOT / "scripts" / "check_objc3c_package_authoring_workflow.py"
PACKAGE_MIRROR_REPRODUCIBILITY_PY = ROOT / "scripts" / "check_objc3c_package_registry_mirror_reproducibility.py"
PACKAGE_ECOSYSTEM_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_package_ecosystem_integration.py"
RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_package_ecosystem_end_to_end.py"
LONG_HORIZON_OPERATIONS_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_long_horizon_operations_integration.py"
LONG_HORIZON_OPERATIONS_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_long_horizon_operations_metadata.py"
ADOPTION_LEGIBILITY_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_adoption_legibility_integration.py"
ADOPTION_LEGIBILITY_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_adoption_legibility_metadata.py"
GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_governance_sustainability_integration.py"
GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_governance_sustainability_metadata.py"
PLANNING_ISSUE_PUBLISHER_PY = ROOT / "scripts" / "publish_objc3c_planning_issues.py"
PLANNING_PUBLICATION_AUDIT_PY = ROOT / "scripts" / "audit_objc3c_planning_publication.py"
LLVM_CAPABILITIES_PROBE_PY = ROOT / "scripts" / "probe_objc3c_llvm_capabilities.py"
DEPENDENCY_BOUNDARIES_PY = ROOT / "scripts" / "check_objc3c_dependency_boundaries.py"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"
SOURCE_HYGIENE_AUTHENTICITY_PY = ROOT / "scripts" / "check_source_hygiene_authenticity.py"
SOURCE_HYGIENE_HARD_CUTOVER_PY = ROOT / "scripts" / "check_source_hygiene_hard_cutover.py"
RUNNABLE_BONUS_EXPERIENCE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_bonus_experience_end_to_end.py"
SPEC_LINT_PY = ROOT / "scripts" / "spec_lint.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
BEHAVIOR_MATRIX_PY = ROOT / "scripts" / "check_objc3c_behavior_matrix.py"
RUNTIME_ACCEPTANCE_PY = ROOT / "scripts" / "check_objc3c_runtime_acceptance.py"
FRONTEND_C_API_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_DEVELOPER_TOOLING_SOURCE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PLAYGROUND_SOURCE = DEFAULT_DEVELOPER_TOOLING_SOURCE
PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
PLAYGROUND_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "playground"
PLAYGROUND_REPORT_ROOT = ROOT / "tmp" / "reports" / "playground"
PLAYGROUND_WORKSPACE_CONTRACT_ID = "objc3c.playground.workspace.v1"
REPO_SUPERCLEAN_SOURCE_OF_TRUTH = ROOT / "tmp" / "artifacts" / "objc3c-native" / "repo_superclean_source_of_truth.json"
SHOWCASE_PORTFOLIO_JSON = ROOT / "showcase" / "portfolio.json"
SHOWCASE_TUTORIAL_WALKTHROUGH_JSON = ROOT / "showcase" / "tutorial_walkthrough.json"
def run_steps(actions: Sequence[str]) -> int:
    for action in actions:
        rc = execute_registered_action(action, [])
        if rc != 0:
            return rc
    return 0


def action_build_default(_: list[str]) -> int:
    return run_steps(["build-native-binaries"])


def action_check_dependency_boundaries(_: list[str]) -> int:
    return run([sys.executable, str(DEPENDENCY_BOUNDARIES_PY), "--strict"])


def action_check_llvm_capabilities(_: list[str]) -> int:
    return run(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            "tmp/artifacts/objc3c-native/llvm_capabilities/summary.json",
        ]
    )


def action_check_release_evidence(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_EVIDENCE_PY)])


def action_check_source_hygiene_authenticity(_: list[str]) -> int:
    return run([sys.executable, str(SOURCE_HYGIENE_AUTHENTICITY_PY)])


def action_check_source_hygiene_hard_cutover(_: list[str]) -> int:
    return run([sys.executable, str(SOURCE_HYGIENE_HARD_CUTOVER_PY)])


def action_check_task_hygiene(_: list[str]) -> int:
    return run([sys.executable, str(TASK_HYGIENE_PY)])


def action_check_repo_superclean_surface(_: list[str]) -> int:
    return run([sys.executable, str(REPO_SUPERCLEAN_SURFACE_PY)])


def action_check_showcase_surface(rest: list[str]) -> int:
    return run([sys.executable, str(SHOWCASE_SURFACE_PY), *rest])


def action_check_stdlib_surface(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_SURFACE_PY)])


def action_materialize_stdlib_workspace(rest: list[str]) -> int:
    return run([sys.executable, str(MATERIALIZE_STDLIB_PY), *rest])


def action_materialize_canonical_application_workspace(rest: list[str]) -> int:
    return run([sys.executable, str(CANONICAL_APPLICATION_WORKSPACE_MATERIALIZER_PY), *rest])


def action_validate_showcase_runtime(rest: list[str]) -> int:
    return pwsh_file(SHOWCASE_RUNTIME_PS1, *rest)


def action_validate_showcase(_: list[str]) -> int:
    return run([sys.executable, str(SHOWCASE_INTEGRATION_PY)])


def action_validate_stdlib_foundation(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_FOUNDATION_INTEGRATION_PY)])


def action_validate_stdlib_advanced(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_ADVANCED_INTEGRATION_PY)])


def action_validate_stdlib_program(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_PROGRAM_INTEGRATION_PY)])


def action_validate_application_architecture(_: list[str]) -> int:
    return run([sys.executable, str(APPLICATION_ARCHITECTURE_INTEGRATION_PY)])


def action_validate_runnable_application_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_APPLICATION_ARCHITECTURE_E2E_PY)])


def action_build_package_lock(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_LOCK_PY)])


def action_validate_package_authoring(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_AUTHORING_WORKFLOW_PY)])


def action_validate_package_mirror(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_MIRROR_REPRODUCIBILITY_PY)])


def action_validate_package_ecosystem(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_ECOSYSTEM_INTEGRATION_PY)])


def action_validate_runnable_package_ecosystem(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY)])


def action_validate_long_horizon_operations(_: list[str]) -> int:
    return run([sys.executable, str(LONG_HORIZON_OPERATIONS_INTEGRATION_PY)])


def action_publish_long_horizon_operations(_: list[str]) -> int:
    return run([sys.executable, str(LONG_HORIZON_OPERATIONS_PUBLICATION_PY)])


def action_validate_adoption_legibility(_: list[str]) -> int:
    return run([sys.executable, str(ADOPTION_LEGIBILITY_INTEGRATION_PY)])


def action_publish_adoption_legibility(_: list[str]) -> int:
    return run([sys.executable, str(ADOPTION_LEGIBILITY_PUBLICATION_PY)])


def action_validate_governance_sustainability(_: list[str]) -> int:
    return run([sys.executable, str(GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY)])


def action_publish_governance_sustainability(_: list[str]) -> int:
    return run([sys.executable, str(GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY)])


def action_publish_planning_issues(rest: list[str]) -> int:
    return run([sys.executable, str(PLANNING_ISSUE_PUBLISHER_PY), *rest])


def action_check_planning_publication_drift(rest: list[str]) -> int:
    return run([sys.executable, str(PLANNING_PUBLICATION_AUDIT_PY), "--check", *rest])


def action_validate_runnable_stdlib_advanced(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_ADVANCED_E2E_PY)])


def action_validate_runnable_stdlib_program(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_PROGRAM_E2E_PY)])


def action_validate_runnable_showcase(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_SHOWCASE_E2E_PY)])


def action_validate_getting_started(_: list[str]) -> int:
    return run([sys.executable, str(GETTING_STARTED_INTEGRATION_PY)])


def action_validate_repo_superclean(_: list[str]) -> int:
    return run_composite_validation(
        "validate-repo-superclean",
        [
            ("build-native-contracts", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(BUILD_PS1), "-ExecutionMode", "contracts-binary"]),
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            ("source-hygiene", [sys.executable, str(SOURCE_HYGIENE_AUTHENTICITY_PY)]),
        ],
    )


def _parse_developer_tooling_invocation(rest: list[str]) -> tuple[str, list[str]]:
    if rest and not rest[0].startswith("--"):
        source_text = rest[0]
        passthrough = rest[1:]
    else:
        source_text = str(DEFAULT_DEVELOPER_TOOLING_SOURCE.relative_to(ROOT).as_posix())
        passthrough = rest
    for forbidden in (
        "--summary-out",
        "--dump-summary-json",
        "--dump-observability-json",
        "--dump-playground-repro-json",
        "--dump-runtime-inspector-json",
        "--dump-stage-trace-json",
    ):
        if forbidden in passthrough:
            raise ValueError(f"{forbidden} is managed by the public developer-tooling action")
    return source_text, passthrough


def _parse_playground_invocation(rest: list[str]) -> tuple[str, list[str]]:
    if rest and not rest[0].startswith("--"):
        source_text = rest[0]
        passthrough = rest[1:]
    else:
        source_text = str(DEFAULT_PLAYGROUND_SOURCE.relative_to(ROOT).as_posix())
        passthrough = rest
    for forbidden in (
        "--out-dir",
        "--emit-prefix",
        "--summary-out",
        "--dump-summary-json",
        "--dump-observability-json",
        "--dump-playground-repro-json",
        "--dump-runtime-inspector-json",
        "--dump-stage-trace-json",
    ):
        if forbidden in passthrough:
            raise ValueError(f"{forbidden} is managed by the public playground action")
    return source_text, passthrough


def _resolve_source_path(source_text: str) -> tuple[Path, str]:
    candidate = Path(source_text)
    resolved = candidate if candidate.is_absolute() else ROOT / candidate
    resolved = resolved.resolve()
    if not resolved.is_file():
        raise FileNotFoundError(f"playground source not found: {source_text}")
    try:
        display = resolved.relative_to(ROOT).as_posix()
    except ValueError:
        display = resolved.as_posix()
    return resolved, display


def _slugify_playground_workspace(source_display: str) -> str:
    safe_stem = re.sub(r"[^a-z0-9]+", "-", Path(source_display).stem.lower()).strip("-")
    if not safe_stem:
        safe_stem = "source"
    digest = hashlib.sha256(source_display.encode("utf-8")).hexdigest()[:12]
    return f"{safe_stem}-{digest}"


def _ensure_frontend_runner_ready() -> int:
    if not (ROOT / "native" / "objc3c" / "src" / "main.cpp").is_file() and FRONTEND_C_API_RUNNER_EXE.is_file():
        return 0
    return execute_registered_action("build-native-binaries", [])


def _run_playground_workspace(
    rest: list[str],
    *,
    emit_payload: bool,
) -> int:
    try:
        source_text, passthrough = _parse_playground_invocation(rest)
        _, source_display = _resolve_source_path(source_text)
    except (ValueError, FileNotFoundError) as exc:
        print(str(exc), file=sys.stderr)
        return 2

    rc = _ensure_frontend_runner_ready()
    if rc != 0:
        return rc

    workspace_id = _slugify_playground_workspace(source_display)
    workspace_root = PLAYGROUND_ARTIFACT_ROOT / workspace_id
    artifact_root = workspace_root / "build"
    report_root = PLAYGROUND_REPORT_ROOT / workspace_id
    workspace_manifest_path = workspace_root / "workspace.json"
    summary_path = report_root / "compile-summary.json"
    dump_path = report_root / "playground-repro.json"

    artifact_root.mkdir(parents=True, exist_ok=True)
    report_root.mkdir(parents=True, exist_ok=True)

    artifact_root_rel = artifact_root.relative_to(ROOT).as_posix()
    summary_path_rel = summary_path.relative_to(ROOT).as_posix()

    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    result = subprocess.run(
        [
            str(FRONTEND_C_API_RUNNER_EXE),
            source_display,
            "--out-dir",
            artifact_root_rel,
            "--emit-prefix",
            "module",
            "--summary-out",
            summary_path_rel,
            "--dump-playground-repro-json",
            *passthrough,
        ],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=env,
    )
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        if result.stdout:
            sys.stdout.write(result.stdout)
        return result.returncode

    try:
        playground_payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        print(f"playground-workspace: invalid JSON from frontend runner: {exc}", file=sys.stderr)
        return 1

    dump_path.write_text(json.dumps(playground_payload, indent=2) + "\n", encoding="utf-8")

    editor_result = subprocess.run(
        [sys.executable, str(EDITOR_TOOLING_SURFACE_PY), source_display],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=env,
    )
    if editor_result.stdout:
        sys.stdout.write(editor_result.stdout)
    if editor_result.stderr:
        sys.stderr.write(editor_result.stderr)
    if editor_result.returncode != 0:
        return editor_result.returncode

    editor_surface_path_text = extract_output_line(editor_result.stdout, "dump_path:")
    capabilities_path_text = extract_output_line(editor_result.stdout, "capabilities_path:")
    navigation_path_text = extract_output_line(editor_result.stdout, "navigation_path:")
    formatter_path_text = extract_output_line(editor_result.stdout, "formatter_path:")
    debug_path_text = extract_output_line(editor_result.stdout, "debug_path:")
    if not editor_surface_path_text:
        print("playground-workspace: editor tooling action did not publish dump_path", file=sys.stderr)
        return 1
    editor_surface_path = ROOT / editor_surface_path_text
    if not editor_surface_path.is_file():
        print(f"playground-workspace: missing editor tooling surface {editor_surface_path_text}", file=sys.stderr)
        return 1
    editor_surface_payload = load_json(editor_surface_path)
    formatter_payload = editor_surface_payload.get("formatter", {})
    debug_payload = editor_surface_payload.get("debug", {})
    workspace_drill_commands = {
        "inspect_editor_tooling": f"{WORKFLOW_COMMAND_TEXT} inspect-editor-tooling {source_display}",
        "format_preview": f"{WORKFLOW_COMMAND_TEXT} format-objc3c {source_display}",
        "object_symbol_inventory": str(debug_payload.get("object_symbol_inventory_command", "")),
    }

    workspace_payload = {
        "contract_id": PLAYGROUND_WORKSPACE_CONTRACT_ID,
        "schema_version": 1,
        "workspace_id": workspace_id,
        "source_path": source_display,
        "workspace_root": workspace_root.relative_to(ROOT).as_posix(),
        "artifact_root": artifact_root.relative_to(ROOT).as_posix(),
        "report_root": report_root.relative_to(ROOT).as_posix(),
        "emit_prefix": "module",
        "summary_path": summary_path.relative_to(ROOT).as_posix(),
        "playground_payload_path": dump_path.relative_to(ROOT).as_posix(),
        "playground_payload_contract_id": playground_payload.get("contract_id"),
        "public_actions": [
            "materialize-playground-workspace",
            "compile-objc3c",
            "inspect-playground-repro",
            "inspect-compile-observability",
            "inspect-editor-tooling",
            "format-objc3c",
            "trace-compile-stages",
            "validate-developer-tooling",
        ],
        "compile_profile": playground_payload.get("compile_profile", {}),
        "artifact_paths": playground_payload.get("artifact_paths", {}),
        "showcase_examples": playground_payload.get("showcase_examples", []),
        "repro_command": playground_payload.get("dump_commands", {}).get("repro_runner", ""),
        "editor_tooling": {
            "editor_surface_path": editor_surface_path_text,
            "language_server_capabilities_path": capabilities_path_text,
            "navigation_path": navigation_path_text,
            "formatter_path": formatter_path_text,
            "debug_path": debug_path_text,
            "formatted_output_path": formatter_payload.get("formatted_output_path"),
            "format_preview_supported": formatter_payload.get("supported"),
            "debugger_model": debug_payload.get("debugger_model", ""),
            "declaration_breakpoint_anchor_count": debug_payload.get("declaration_breakpoint_anchor_count", 0),
            "statement_level_stepping": debug_payload.get("statement_level_stepping"),
        },
        "workspace_drill_commands": workspace_drill_commands,
    }
    workspace_manifest_path.write_text(
        json.dumps(workspace_payload, indent=2) + "\n",
        encoding="utf-8",
    )

    if emit_payload:
        sys.stdout.write(json.dumps(playground_payload, indent=2) + "\n")
    print(f"workspace_path: {workspace_manifest_path.relative_to(ROOT).as_posix()}")
    print(f"summary_path: {summary_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    print(f"artifact_root: {artifact_root.relative_to(ROOT).as_posix()}")
    return 0


def _write_json_capture(path: Path, stdout: str) -> int:
    try:
        payload = json.loads(stdout)
    except json.JSONDecodeError as exc:
        print(f"developer-tooling-dump: invalid JSON from frontend runner: {exc}", file=sys.stderr)
        return 1
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return 0


def _run_developer_tooling_dump(action_name: str,
                                dump_flag: str,
                                dump_filename: str,
                                rest: list[str]) -> int:
    try:
        source_text, passthrough = _parse_developer_tooling_invocation(rest)
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return 2
    rc = _ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    summary_path = PUBLIC_WORKFLOW_REPORT_ROOT / f"{action_name}-summary.json"
    dump_path = PUBLIC_WORKFLOW_REPORT_ROOT / dump_filename
    command = [
        str(FRONTEND_C_API_RUNNER_EXE),
        source_text,
        "--summary-out",
        str(summary_path),
        dump_flag,
        *passthrough,
    ]
    result = run_capture(command)
    if result.returncode != 0:
        return result.returncode
    rc = _write_json_capture(dump_path, result.stdout)
    if rc != 0:
        return rc
    print(f"summary_path: {summary_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return 0


def action_inspect_compile_observability(rest: list[str]) -> int:
    return _run_developer_tooling_dump(
        "inspect-compile-observability",
        "--dump-observability-json",
        "compile-observability.json",
        rest,
    )


def action_inspect_runtime_inspector(rest: list[str]) -> int:
    return _run_developer_tooling_dump(
        "inspect-runtime-inspector",
        "--dump-runtime-inspector-json",
        "runtime-inspector.json",
        rest,
    )


def action_inspect_editor_tooling(rest: list[str]) -> int:
    rc = _ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    return run([sys.executable, str(EDITOR_TOOLING_SURFACE_PY), *rest])


def action_format_objc3c(rest: list[str]) -> int:
    return run([sys.executable, str(FORMAT_OBJC3C_SOURCE_PY), *rest])


def action_inspect_capability_explorer(rest: list[str]) -> int:
    dump_path = PUBLIC_WORKFLOW_REPORT_ROOT / "capability-explorer.json"
    dump_path.parent.mkdir(parents=True, exist_ok=True)
    rc = run(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            str(dump_path),
            *rest,
        ]
    )
    if rc == 0:
        print(f"summary_path: {dump_path.relative_to(ROOT).as_posix()}")
        print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return rc


def action_inspect_playground_repro(rest: list[str]) -> int:
    return _run_playground_workspace(rest, emit_payload=True)


def action_materialize_playground_workspace(rest: list[str]) -> int:
    return _run_playground_workspace(rest, emit_payload=False)


def action_trace_compile_stages(rest: list[str]) -> int:
    return _run_developer_tooling_dump(
        "trace-compile-stages",
        "--dump-stage-trace-json",
        "compile-stage-trace.json",
        rest,
    )


def action_validate_developer_tooling(_: list[str]) -> int:
    return run([sys.executable, str(DEVELOPER_TOOLING_INTEGRATION_PY)])


def action_validate_runnable_developer_tooling(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_DEVELOPER_TOOLING_E2E_PY)])


def action_validate_bonus_experiences(_: list[str]) -> int:
    return run([sys.executable, str(BONUS_EXPERIENCE_INTEGRATION_PY)])


def action_validate_conformance_corpus(_: list[str]) -> int:
    return run([sys.executable, str(CONFORMANCE_CORPUS_INTEGRATION_PY)])


def action_validate_runnable_conformance_corpus(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_CONFORMANCE_CORPUS_E2E_PY)])


def action_inspect_bonus_tool_integration(_: list[str]) -> int:
    native_main = ROOT / "native" / "objc3c" / "src" / "main.cpp"
    if native_main.is_file():
        rc = execute_registered_action("build-native-contracts", [])
        if rc != 0:
            return rc
    if not REPO_SUPERCLEAN_SOURCE_OF_TRUTH.is_file():
        print(
            f"missing source-of-truth artifact: {REPO_SUPERCLEAN_SOURCE_OF_TRUTH.relative_to(ROOT).as_posix()}",
            file=sys.stderr,
        )
        return 1
    source_of_truth = json.loads(REPO_SUPERCLEAN_SOURCE_OF_TRUTH.read_text(encoding="utf-8"))
    portfolio = json.loads(SHOWCASE_PORTFOLIO_JSON.read_text(encoding="utf-8"))
    walkthrough = json.loads(SHOWCASE_TUTORIAL_WALKTHROUGH_JSON.read_text(encoding="utf-8"))
    integration_surface = source_of_truth.get("bonus_tool_integration_surface")
    if not isinstance(integration_surface, dict):
        print("repo superclean artifact missing bonus_tool_integration_surface", file=sys.stderr)
        return 1
    dump_path = PUBLIC_WORKFLOW_REPORT_ROOT / "bonus-tool-integration.json"
    dump_path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "contract_id": "objc3c.bonus.tool.integration.surface.v1",
        "schema_version": 1,
        "source_of_truth_artifact": REPO_SUPERCLEAN_SOURCE_OF_TRUTH.relative_to(ROOT).as_posix(),
        "integration_surface": integration_surface,
        "bonus_experience_surfaces": source_of_truth.get("bonus_experience_surfaces", {}),
        "showcase_portfolio_contract_id": portfolio.get("contract_id"),
        "guided_walkthrough_contract_id": walkthrough.get("contract_id"),
    }
    dump_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {dump_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return 0


def action_materialize_project_template(rest: list[str]) -> int:
    return run([sys.executable, str(PROJECT_TEMPLATE_MATERIALIZER_PY), *rest])


def action_validate_runnable_bonus_experiences(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BONUS_EXPERIENCE_E2E_PY)])


def action_validate_runnable_stdlib_foundation(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_FOUNDATION_E2E_PY)])


def action_lint_spec(_: list[str]) -> int:
    return run([sys.executable, str(SPEC_LINT_PY)])


def action_lint(_: list[str]) -> int:
    return run_steps(["check-source-hygiene-hard-cutover", "check-task-hygiene", "build-site", "check-markdown"])


def action_test_default(_: list[str]) -> int:
    return run_steps(["test-smoke"])


def to_repo_relative(raw_path: str) -> str:
    candidate = Path(raw_path.strip())
    try:
        if candidate.is_absolute():
            return str(candidate.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return raw_path.strip()
    return raw_path.strip().replace("\\", "/")

def write_composite_validation_report(
    action: str,
    steps: list[dict[str, object]],
    *,
    status: str,
) -> Path:
    PUBLIC_WORKFLOW_REPORT_ROOT.mkdir(parents=True, exist_ok=True)
    report_path = PUBLIC_WORKFLOW_REPORT_ROOT / f"{action}.json"
    child_timing = collect_child_timing(steps)
    budgets = child_timing.get("budgets", [])
    budget_violations = (
        validation_budget_violations(budgets)
        if isinstance(budgets, list)
        else []
    )
    effective_status = (
        "FAIL"
        if status == "PASS" and budget_violations
        else status
    )
    payload = {
        "action": action,
        "status": effective_status,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "timing": {
            "step_count": len(steps),
            "total_step_duration_seconds": round(
                sum(float(step.get("duration_seconds", 0.0)) for step in steps),
                6,
            ),
            "slowest_steps": sorted(
                [
                    {
                        "action": str(step.get("action", "")),
                        "duration_seconds": float(step.get("duration_seconds", 0.0)),
                        "exit_code": int(step.get("exit_code", 0)),
                    }
                    for step in steps
                ],
                key=lambda entry: entry["duration_seconds"],
                reverse=True,
            )[:10],
            "estimated_no_skip_seconds": child_timing["estimated_no_skip_seconds"],
        },
        "child_timing": child_timing,
        "budget_policy": {
            "contract_id": "objc3c.validation.speed.budget.policy.v1",
            "mode": (
                "fail"
                if validation_speed_budget_mode() == "fail"
                else "warning-only"
            ),
            "violations": budget_violations,
            "fail_closed": True,
        },
        "claim_boundary": {
            "contract_id": "objc3c.runtime.execution.claim.boundary.v1",
            "reports_are_authoritative_only_when_child_steps_are_compile-coupled": True,
            "authoritative_child_surfaces": [
                "scripts/check_objc3c_runtime_acceptance.py",
                "scripts/check_objc3c_execution_replay_proof.ps1",
                "scripts/check_objc3c_native_execution_smoke.ps1",
            ],
            "non_authoritative_inputs": [
                "integrated report paths by themselves",
                "sidecar-only summaries with no matching emitted object/probe path",
                "synthetic or hand-authored llvm ir used without coupled compile output",
            ],
        },
        "steps": steps,
    }
    runtime_state_publication_surface = load_surface_from_report(
        steps, "runtime_state_publication_surface"
    )
    if runtime_state_publication_surface is not None:
        payload["runtime_state_publication_surface"] = runtime_state_publication_surface
    runtime_error_execution_cleanup_source_surface = load_surface_from_report(
        steps, "runtime_error_execution_cleanup_source_surface"
    )
    if runtime_error_execution_cleanup_source_surface is not None:
        payload["runtime_error_execution_cleanup_source_surface"] = (
            runtime_error_execution_cleanup_source_surface
        )
    runtime_catch_filter_finalization_source_surface = load_surface_from_report(
        steps, "runtime_catch_filter_finalization_source_surface"
    )
    if runtime_catch_filter_finalization_source_surface is not None:
        payload["runtime_catch_filter_finalization_source_surface"] = (
            runtime_catch_filter_finalization_source_surface
        )
    runtime_error_propagation_cleanup_semantics_surface = load_surface_from_report(
        steps, "runtime_error_propagation_cleanup_semantics_surface"
    )
    if runtime_error_propagation_cleanup_semantics_surface is not None:
        payload["runtime_error_propagation_cleanup_semantics_surface"] = (
            runtime_error_propagation_cleanup_semantics_surface
        )
    runtime_bridging_filter_unwind_diagnostics_surface = load_surface_from_report(
        steps, "runtime_bridging_filter_unwind_diagnostics_surface"
    )
    if runtime_bridging_filter_unwind_diagnostics_surface is not None:
        payload["runtime_bridging_filter_unwind_diagnostics_surface"] = (
            runtime_bridging_filter_unwind_diagnostics_surface
        )
    runtime_error_lowering_unwind_bridge_helper_surface = load_surface_from_report(
        steps, "runtime_error_lowering_unwind_bridge_helper_surface"
    )
    if runtime_error_lowering_unwind_bridge_helper_surface is not None:
        payload["runtime_error_lowering_unwind_bridge_helper_surface"] = (
            runtime_error_lowering_unwind_bridge_helper_surface
        )
    runtime_error_runtime_abi_cleanup_surface = load_surface_from_report(
        steps, "runtime_error_runtime_abi_cleanup_surface"
    )
    if runtime_error_runtime_abi_cleanup_surface is not None:
        payload["runtime_error_runtime_abi_cleanup_surface"] = (
            runtime_error_runtime_abi_cleanup_surface
        )
    runtime_error_propagation_catch_cleanup_runtime_implementation_surface = (
        load_surface_from_report(
            steps, "runtime_error_propagation_catch_cleanup_runtime_implementation_surface"
        )
    )
    if runtime_error_propagation_catch_cleanup_runtime_implementation_surface is not None:
        payload["runtime_error_propagation_catch_cleanup_runtime_implementation_surface"] = (
            runtime_error_propagation_catch_cleanup_runtime_implementation_surface
        )
    acceptance_suite_surface = load_surface_from_report(steps, "acceptance_suite_surface")
    if acceptance_suite_surface is not None:
        payload["acceptance_suite_surface"] = acceptance_suite_surface
    runtime_installation_abi_surface = load_surface_from_report(
        steps, "runtime_installation_abi_surface"
    )
    if runtime_installation_abi_surface is not None:
        payload["runtime_installation_abi_surface"] = runtime_installation_abi_surface
    runtime_loader_lifecycle_surface = load_surface_from_report(
        steps, "runtime_loader_lifecycle_surface"
    )
    if runtime_loader_lifecycle_surface is not None:
        payload["runtime_loader_lifecycle_surface"] = runtime_loader_lifecycle_surface
    runtime_object_model_realization_source_surface = load_surface_from_report(
        steps, "runtime_object_model_realization_source_surface"
    )
    if runtime_object_model_realization_source_surface is not None:
        payload["runtime_object_model_realization_source_surface"] = (
            runtime_object_model_realization_source_surface
        )
    runtime_block_arc_unified_source_surface = load_surface_from_report(
        steps, "runtime_block_arc_unified_source_surface"
    )
    if runtime_block_arc_unified_source_surface is not None:
        payload["runtime_block_arc_unified_source_surface"] = (
            runtime_block_arc_unified_source_surface
        )
    runtime_ownership_transfer_capture_family_source_surface = load_surface_from_report(
        steps, "runtime_ownership_transfer_capture_family_source_surface"
    )
    if runtime_ownership_transfer_capture_family_source_surface is not None:
        payload["runtime_ownership_transfer_capture_family_source_surface"] = (
            runtime_ownership_transfer_capture_family_source_surface
        )
    runtime_block_arc_lowering_helper_surface = load_surface_from_report(
        steps, "runtime_block_arc_lowering_helper_surface"
    )
    if runtime_block_arc_lowering_helper_surface is not None:
        payload["runtime_block_arc_lowering_helper_surface"] = (
            runtime_block_arc_lowering_helper_surface
        )
    runtime_block_arc_runtime_abi_surface = load_surface_from_report(
        steps, "runtime_block_arc_runtime_abi_surface"
    )
    if runtime_block_arc_runtime_abi_surface is not None:
        payload["runtime_block_arc_runtime_abi_surface"] = (
            runtime_block_arc_runtime_abi_surface
        )
    runtime_property_ivar_storage_accessor_source_surface = load_surface_from_report(
        steps, "runtime_property_ivar_storage_accessor_source_surface"
    )
    if runtime_property_ivar_storage_accessor_source_surface is not None:
        payload["runtime_property_ivar_storage_accessor_source_surface"] = (
            runtime_property_ivar_storage_accessor_source_surface
        )
    storage_accessor_runtime_abi_surface = load_surface_from_report(
        steps, "storage_accessor_runtime_abi_surface"
    )
    if storage_accessor_runtime_abi_surface is not None:
        payload["storage_accessor_runtime_abi_surface"] = (
            storage_accessor_runtime_abi_surface
        )
    runtime_property_ivar_accessor_reflection_implementation_surface = (
        load_surface_from_report(
            steps,
            "runtime_property_ivar_accessor_reflection_implementation_surface",
        )
    )
    if runtime_property_ivar_accessor_reflection_implementation_surface is not None:
        payload["runtime_property_ivar_accessor_reflection_implementation_surface"] = (
            runtime_property_ivar_accessor_reflection_implementation_surface
        )
    runtime_claimable_surface_residual_non_claimable_gaps_source_surface = load_surface_from_report(
        steps, "runtime_claimable_surface_residual_non_claimable_gaps_source_surface"
    )
    if runtime_claimable_surface_residual_non_claimable_gaps_source_surface is not None:
        payload["runtime_claimable_surface_residual_non_claimable_gaps_source_surface"] = (
            runtime_claimable_surface_residual_non_claimable_gaps_source_surface
        )
    runtime_strict_profile_feature_claim_source_surface = load_surface_from_report(
        steps, "runtime_strict_profile_feature_claim_source_surface"
    )
    if runtime_strict_profile_feature_claim_source_surface is not None:
        payload["runtime_strict_profile_feature_claim_source_surface"] = (
            runtime_strict_profile_feature_claim_source_surface
        )
    runtime_claimability_semantics_release_policy_surface = load_surface_from_report(
        steps, "runtime_claimability_semantics_release_policy_surface"
    )
    if runtime_claimability_semantics_release_policy_surface is not None:
        payload["runtime_claimability_semantics_release_policy_surface"] = (
            runtime_claimability_semantics_release_policy_surface
        )
    runtime_strict_profile_claim_implementation_surface = load_surface_from_report(
        steps, "runtime_strict_profile_claim_implementation_surface"
    )
    if runtime_strict_profile_claim_implementation_surface is not None:
        payload["runtime_strict_profile_claim_implementation_surface"] = (
            runtime_strict_profile_claim_implementation_surface
        )
    runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface = load_surface_from_report(
        steps, "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface"
    )
    if runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface is not None:
        payload["runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface"] = (
            runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface
        )
    runtime_claim_publication_dashboard_schema_surface = load_surface_from_report(
        steps, "runtime_claim_publication_dashboard_schema_surface"
    )
    if runtime_claim_publication_dashboard_schema_surface is not None:
        payload["runtime_claim_publication_dashboard_schema_surface"] = (
            runtime_claim_publication_dashboard_schema_surface
        )
    runtime_final_claim_publication_deprecated_path_shutdown_surface = load_surface_from_report(
        steps, "runtime_final_claim_publication_deprecated_path_shutdown_surface"
    )
    if runtime_final_claim_publication_deprecated_path_shutdown_surface is not None:
        payload["runtime_final_claim_publication_deprecated_path_shutdown_surface"] = (
            runtime_final_claim_publication_deprecated_path_shutdown_surface
        )
    runtime_release_candidate_claim_abi_surface = load_surface_from_report(
        steps, "runtime_release_candidate_claim_abi_surface"
    )
    if runtime_release_candidate_claim_abi_surface is not None:
        payload["runtime_release_candidate_claim_abi_surface"] = (
            runtime_release_candidate_claim_abi_surface
        )
    runtime_final_release_evidence_descaffolding_implementation_surface = load_surface_from_report(
        steps, "runtime_final_release_evidence_descaffolding_implementation_surface"
    )
    if runtime_final_release_evidence_descaffolding_implementation_surface is not None:
        payload["runtime_final_release_evidence_descaffolding_implementation_surface"] = (
            runtime_final_release_evidence_descaffolding_implementation_surface
        )
    runtime_metaprogramming_source_surface = load_surface_from_report(
        steps, "runtime_metaprogramming_source_surface"
    )
    if runtime_metaprogramming_source_surface is not None:
        payload["runtime_metaprogramming_source_surface"] = (
            runtime_metaprogramming_source_surface
        )
    runtime_metaprogramming_package_provenance_source_surface = load_surface_from_report(
        steps, "runtime_metaprogramming_package_provenance_source_surface"
    )
    if runtime_metaprogramming_package_provenance_source_surface is not None:
        payload["runtime_metaprogramming_package_provenance_source_surface"] = (
            runtime_metaprogramming_package_provenance_source_surface
        )
    runtime_metaprogramming_semantics_surface = load_surface_from_report(
        steps, "runtime_metaprogramming_semantics_surface"
    )
    if runtime_metaprogramming_semantics_surface is not None:
        payload["runtime_metaprogramming_semantics_surface"] = (
            runtime_metaprogramming_semantics_surface
        )
    runtime_metaprogramming_lowering_host_cache_surface = load_surface_from_report(
        steps, "runtime_metaprogramming_lowering_host_cache_surface"
    )
    if runtime_metaprogramming_lowering_host_cache_surface is not None:
        payload["runtime_metaprogramming_lowering_host_cache_surface"] = (
            runtime_metaprogramming_lowering_host_cache_surface
        )
    runtime_cross_module_metaprogramming_artifact_preservation_surface = (
        load_surface_from_report(
            steps, "runtime_cross_module_metaprogramming_artifact_preservation_surface"
        )
    )
    if runtime_cross_module_metaprogramming_artifact_preservation_surface is not None:
        payload["runtime_cross_module_metaprogramming_artifact_preservation_surface"] = (
            runtime_cross_module_metaprogramming_artifact_preservation_surface
        )
    runtime_metaprogramming_runtime_abi_cache_surface = load_surface_from_report(
        steps, "runtime_metaprogramming_runtime_abi_cache_surface"
    )
    if runtime_metaprogramming_runtime_abi_cache_surface is not None:
        payload["runtime_metaprogramming_runtime_abi_cache_surface"] = (
            runtime_metaprogramming_runtime_abi_cache_surface
        )
    runtime_metaprogramming_cache_runtime_integration_implementation_surface = (
        load_surface_from_report(
            steps,
            "runtime_metaprogramming_cache_runtime_integration_implementation_surface",
        )
    )
    if runtime_metaprogramming_cache_runtime_integration_implementation_surface is not None:
        payload["runtime_metaprogramming_cache_runtime_integration_implementation_surface"] = (
            runtime_metaprogramming_cache_runtime_integration_implementation_surface
        )
    for surface_key in (
        "runtime_cross_module_package_interop_source_surface",
        "runtime_textual_binary_interface_parity_source_surface",
        "runtime_mixed_image_compatibility_interop_semantics_surface",
        "runtime_package_loading_module_identity_semantics_surface",
        "runtime_c_cpp_swift_bridge_compatibility_semantics_surface",
        "runtime_import_version_feature_claim_diagnostics_surface",
        "runtime_packaging_bridge_loader_artifact_surface",
        "runtime_mixed_image_package_lowering_bridge_emission_surface",
        "runtime_cross_language_replay_import_surface_preservation_surface",
        "runtime_package_loader_bridge_abi_surface",
        "runtime_package_loading_interop_implementation_surface",
    ):
        surface = load_surface_from_report(steps, surface_key)
        if surface is not None:
            payload[surface_key] = surface
    runtime_property_atomicity_synthesis_reflection_source_surface = load_surface_from_report(
        steps, "runtime_property_atomicity_synthesis_reflection_source_surface"
    )
    if runtime_property_atomicity_synthesis_reflection_source_surface is not None:
        payload["runtime_property_atomicity_synthesis_reflection_source_surface"] = (
            runtime_property_atomicity_synthesis_reflection_source_surface
        )
    runtime_realization_lowering_reflection_artifact_surface = load_surface_from_report(
        steps, "runtime_realization_lowering_reflection_artifact_surface"
    )
    if runtime_realization_lowering_reflection_artifact_surface is not None:
        payload["runtime_realization_lowering_reflection_artifact_surface"] = (
            runtime_realization_lowering_reflection_artifact_surface
        )
    runtime_dispatch_table_reflection_record_lowering_surface = load_surface_from_report(
        steps, "runtime_dispatch_table_reflection_record_lowering_surface"
    )
    if runtime_dispatch_table_reflection_record_lowering_surface is not None:
        payload["runtime_dispatch_table_reflection_record_lowering_surface"] = (
            runtime_dispatch_table_reflection_record_lowering_surface
        )
    runtime_cross_module_realized_metadata_replay_preservation_surface = load_surface_from_report(
        steps, "runtime_cross_module_realized_metadata_replay_preservation_surface"
    )
    if runtime_cross_module_realized_metadata_replay_preservation_surface is not None:
        payload["runtime_cross_module_realized_metadata_replay_preservation_surface"] = (
            runtime_cross_module_realized_metadata_replay_preservation_surface
        )
    runtime_object_model_abi_query_surface = load_surface_from_report(
        steps, "runtime_object_model_abi_query_surface"
    )
    if runtime_object_model_abi_query_surface is not None:
        payload["runtime_object_model_abi_query_surface"] = (
            runtime_object_model_abi_query_surface
        )
    runtime_realization_lookup_reflection_implementation_surface = load_surface_from_report(
        steps, "runtime_realization_lookup_reflection_implementation_surface"
    )
    if runtime_realization_lookup_reflection_implementation_surface is not None:
        payload["runtime_realization_lookup_reflection_implementation_surface"] = (
            runtime_realization_lookup_reflection_implementation_surface
        )
    runtime_reflection_query_surface = load_surface_from_report(
        steps, "runtime_reflection_query_surface"
    )
    if runtime_reflection_query_surface is not None:
        payload["runtime_reflection_query_surface"] = runtime_reflection_query_surface
    runtime_realization_lookup_semantics_surface = load_surface_from_report(
        steps, "runtime_realization_lookup_semantics_surface"
    )
    if runtime_realization_lookup_semantics_surface is not None:
        payload["runtime_realization_lookup_semantics_surface"] = (
            runtime_realization_lookup_semantics_surface
        )
    runtime_class_metaclass_protocol_realization_surface = load_surface_from_report(
        steps, "runtime_class_metaclass_protocol_realization_surface"
    )
    if runtime_class_metaclass_protocol_realization_surface is not None:
        payload["runtime_class_metaclass_protocol_realization_surface"] = (
            runtime_class_metaclass_protocol_realization_surface
        )
    runtime_category_attachment_merged_dispatch_surface = load_surface_from_report(
        steps, "runtime_category_attachment_merged_dispatch_surface"
    )
    if runtime_category_attachment_merged_dispatch_surface is not None:
        payload["runtime_category_attachment_merged_dispatch_surface"] = (
            runtime_category_attachment_merged_dispatch_surface
        )
    runtime_reflection_visibility_coherence_diagnostics_surface = load_surface_from_report(
        steps, "runtime_reflection_visibility_coherence_diagnostics_surface"
    )
    if runtime_reflection_visibility_coherence_diagnostics_surface is not None:
        payload["runtime_reflection_visibility_coherence_diagnostics_surface"] = (
            runtime_reflection_visibility_coherence_diagnostics_surface
        )
    report_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return report_path


def run_composite_step(action: str, command: Sequence[str]) -> dict[str, object]:
    started_at = perf_counter()
    if (
        action.startswith("test-runtime-acceptance")
        and os.environ.get("OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN") == "1"
    ):
        return {
            "action": action,
            "command": [str(token) for token in command],
            "exit_code": 0,
            "report_paths": ["tmp/reports/runtime/acceptance/summary.json"],
            "report_reused": True,
            "report_reuse_source": "OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN",
            "duration_seconds": round(perf_counter() - started_at, 6),
        }
    normalized = [str(token) for token in command]
    if (
        len(normalized) >= 3
        and Path(normalized[0]).resolve() == Path(sys.executable).resolve()
        and Path(normalized[1]).resolve() == Path(__file__).resolve()
    ):
        nested_action = normalized[2]
        nested_rest = normalized[3:]
        exit_code = execute_registered_action(nested_action, nested_rest)
        return {
            "action": action,
            "command": normalized,
            "exit_code": exit_code,
            "report_paths": [],
            "executed_in_process": True,
            "duration_seconds": round(perf_counter() - started_at, 6),
        }
    result = run_capture(command)
    return {
        "action": action,
        "command": normalized,
        "exit_code": result.returncode,
        "report_paths": extract_report_paths(result.stdout),
        "duration_seconds": round(perf_counter() - started_at, 6),
    }


def run_composite_validation(action: str, steps: list[tuple[str, Sequence[str]]]) -> int:
    results: list[dict[str, object]] = []
    workflow_started_at = perf_counter()
    for index, (step_action, command) in enumerate(steps, start=1):
        previous = str(results[-1]["action"]) if results else "none"
        print(
            f"public-workflow-progress: [{index}/{len(steps)}] START action={step_action} "
            f"elapsed={perf_counter() - workflow_started_at:.3f}s last={previous}",
            flush=True,
        )
        step = run_composite_step(step_action, command)
        results.append(step)
        print(
            f"public-workflow-progress: [{index}/{len(steps)}] DONE action={step_action} "
            f"duration={float(step.get('duration_seconds', 0.0)):.3f}s "
            f"elapsed={perf_counter() - workflow_started_at:.3f}s exit={step['exit_code']}",
            flush=True,
        )
        if step["exit_code"] != 0:
            report_path = write_composite_validation_report(action, results, status="FAIL")
            print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
            return int(step["exit_code"])
    report_path = write_composite_validation_report(action, results, status="PASS")
    print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
    report_payload = load_latest_report_payload(report_path)
    if isinstance(report_payload, dict) and report_payload.get("status") != "PASS":
        return 1
    return 0


def action_test_behavior_matrix(_: list[str]) -> int:
    return run([sys.executable, str(BEHAVIOR_MATRIX_PY)])


def action_test_smoke(_: list[str]) -> int:
    return run_composite_validation(
        "test-smoke",
        [
            ("test-behavior-matrix", [sys.executable, str(BEHAVIOR_MATRIX_PY)]),
            ("test-runtime-acceptance-fast", [sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "fast"]),
            ("test-execution-replay-focused", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1), "-Limit", "1"]),
        ],
    )


def action_test_ci(_: list[str]) -> int:
    return run_composite_validation(
        "test-ci",
        [
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            ("validate-developer-tooling", [sys.executable, str(DEVELOPER_TOOLING_INTEGRATION_PY)]),
            ("validate-bonus-experiences", [sys.executable, str(BONUS_EXPERIENCE_INTEGRATION_PY)]),
            ("validate-stdlib-foundation", [sys.executable, str(STDLIB_FOUNDATION_INTEGRATION_PY)]),
            ("validate-stdlib-advanced", [sys.executable, str(STDLIB_ADVANCED_INTEGRATION_PY)]),
            ("validate-stdlib-program", [sys.executable, str(STDLIB_PROGRAM_INTEGRATION_PY)]),
            ("validate-performance-governance", workflow_command("validate-performance-governance")),
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1)]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
        ],
    )


def action_test_recovery(rest: list[str]) -> int:
    return pwsh_file(RECOVERY_PS1, *rest)


def action_test_execution_smoke(rest: list[str]) -> int:
    return pwsh_file(SMOKE_PS1, *rest)


def action_test_execution_replay(rest: list[str]) -> int:
    return pwsh_file(REPLAY_PS1, *rest)


def action_test_execution_replay_focused(_: list[str]) -> int:
    return pwsh_file(REPLAY_PS1, "-Limit", "1")


def action_test_compile_wrapper_self_audit(_: list[str]) -> int:
    return run([sys.executable, str(COMPILE_WRAPPER_SELF_AUDIT_PY)])


def action_test_fixture_matrix(rest: list[str]) -> int:
    return pwsh_file(MATRIX_PS1, *rest)


def action_test_negative_expectations(rest: list[str]) -> int:
    return pwsh_file(NEGATIVE_EXPECTATIONS_PS1, *rest)


def action_test_full(_: list[str]) -> int:
    return run_composite_validation(
        "test-full",
        [
            ("test-behavior-matrix", [sys.executable, str(BEHAVIOR_MATRIX_PY)]),
            ("test-compile-wrapper-self-audit", [sys.executable, str(COMPILE_WRAPPER_SELF_AUDIT_PY)]),
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1), "-Limit", "24"]),
            ("test-runtime-acceptance-fast", [sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "fast"]),
            ("test-execution-replay-focused", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1), "-Limit", "1"]),
        ],
    )


def action_test_nightly(_: list[str]) -> int:
    return run_composite_validation(
        "test-nightly",
        [
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1)]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
            ("validate-conformance-corpus", [sys.executable, str(CONFORMANCE_CORPUS_INTEGRATION_PY)]),
            ("validate-stress", workflow_command("validate-stress")),
            ("validate-external-validation", workflow_command("validate-external-validation")),
            ("validate-public-conformance-reporting", workflow_command("validate-public-conformance-reporting")),
            ("validate-performance-governance", workflow_command("validate-performance-governance")),
            ("validate-release-foundation", workflow_command("validate-release-foundation")),
            ("validate-packaging-channels", workflow_command("validate-packaging-channels")),
            ("validate-release-operations", workflow_command("validate-release-operations")),
            ("validate-distribution-credibility", workflow_command("validate-distribution-credibility")),
            ("test-recovery", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(RECOVERY_PS1)]),
            ("test-fixture-matrix", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(MATRIX_PS1)]),
            ("test-negative-expectations", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(NEGATIVE_EXPECTATIONS_PS1)]),
        ],
    )


ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-default": action_build_default,
    "build-native-binaries": action_build_native_binaries,
    "build-native-contracts": action_build_native_contracts,
    "build-native-full": action_build_native_full,
    "build-native-reconfigure": action_build_native_reconfigure,
    "build-site": action_build_site,
    "check-site": action_check_site,
    "build-native-docs": action_build_native_docs,
    "check-native-docs": action_check_native_docs,
    "build-public-command-surface": action_build_public_command_surface,
    "check-public-command-surface": action_check_public_command_surface,
    "build-public-command-contract": action_build_public_command_contract,
    "check-public-command-contract": action_check_public_command_contract,
    "check-public-command-budget": action_check_public_command_budget,
    "check-documentation-surface": action_check_documentation_surface,
    "check-markdown": action_check_markdown,
    "format-markdown": action_format_markdown,
    "lint-markdown": action_lint_markdown,
    "lint": action_lint,
    "check-dependency-boundaries": action_check_dependency_boundaries,
    "check-llvm-capabilities": action_check_llvm_capabilities,
    "check-release-evidence": action_check_release_evidence,
    "check-source-hygiene-authenticity": action_check_source_hygiene_authenticity,
    "check-source-hygiene-hard-cutover": action_check_source_hygiene_hard_cutover,
    "check-task-hygiene": action_check_task_hygiene,
    "check-showcase-surface": action_check_showcase_surface,
    "check-stdlib-surface": action_check_stdlib_surface,
    "validate-showcase-runtime": action_validate_showcase_runtime,
    "validate-showcase": action_validate_showcase,
    "validate-runnable-showcase": action_validate_runnable_showcase,
    "validate-getting-started": action_validate_getting_started,
    "check-repo-superclean-surface": action_check_repo_superclean_surface,
    "validate-documentation-surface": action_validate_documentation_surface,
    "validate-repo-superclean": action_validate_repo_superclean,
    "compile-objc3c": action_compile_objc3c,
    "materialize-playground-workspace": action_materialize_playground_workspace,
    "materialize-stdlib-workspace": action_materialize_stdlib_workspace,
    "validate-stdlib-foundation": action_validate_stdlib_foundation,
    "validate-stdlib-advanced": action_validate_stdlib_advanced,
    "validate-stdlib-program": action_validate_stdlib_program,
    "validate-runnable-stdlib-advanced": action_validate_runnable_stdlib_advanced,
    "validate-runnable-stdlib-foundation": action_validate_runnable_stdlib_foundation,
    "validate-runnable-stdlib-program": action_validate_runnable_stdlib_program,
    "inspect-capability-explorer": action_inspect_capability_explorer,
    "inspect-playground-repro": action_inspect_playground_repro,
    "inspect-compile-observability": action_inspect_compile_observability,
    "inspect-runtime-inspector": action_inspect_runtime_inspector,
    "inspect-editor-tooling": action_inspect_editor_tooling,
    "format-objc3c": action_format_objc3c,
    "benchmark-runtime-inspector": action_benchmark_runtime_inspector,
    "benchmark-performance": action_benchmark_performance,
    "benchmark-runtime-performance": action_benchmark_runtime_performance,
    "benchmark-compiler-throughput": action_benchmark_compiler_throughput,
    "validate-compiler-throughput": action_validate_compiler_throughput,
    "validate-runnable-compiler-throughput": action_validate_runnable_compiler_throughput,
    "validate-runtime-performance": action_validate_runtime_performance,
    "validate-runnable-runtime-performance": action_validate_runnable_runtime_performance,
    "benchmark-comparative-baselines": action_benchmark_comparative_baselines,
    "validate-runnable-performance": action_validate_runnable_performance,
    "validate-performance-foundation": action_validate_performance_foundation,
    "validate-conformance-corpus": action_validate_conformance_corpus,
    "validate-runnable-conformance-corpus": action_validate_runnable_conformance_corpus,
    "check-stress-surface": action_check_stress_surface,
    "test-fuzz-safety": action_test_fuzz_safety,
    "test-lowering-runtime-stress": action_test_lowering_runtime_stress,
    "test-mixed-module-differential": action_test_mixed_module_differential,
    "test-stress-minimization": action_test_stress_minimization,
    "test-stress-crash-triage": action_test_stress_crash_triage,
    "validate-stress": action_validate_stress,
    "validate-stress-integration": action_validate_stress_integration,
    "validate-stress-end-to-end": action_validate_stress_end_to_end,
    "check-external-validation-surface": action_check_external_validation_surface,
    "test-external-validation-replay": action_test_external_validation_replay,
    "publish-external-repro-corpus": action_publish_external_repro_corpus,
    "validate-external-validation": action_validate_external_validation,
    "validate-external-validation-integration": action_validate_external_validation_integration,
    "check-public-conformance-reporting-surface": action_check_public_conformance_reporting_surface,
    "check-public-conformance-schema-surface": action_check_public_conformance_schema_surface,
    "build-public-conformance-scorecard": action_build_public_conformance_scorecard,
    "publish-public-conformance-report": action_publish_public_conformance_report,
    "validate-public-conformance-reporting": action_validate_public_conformance_reporting,
    "validate-public-conformance-reporting-integration": action_validate_public_conformance_reporting_integration,
    "validate-public-conformance-reporting-end-to-end": action_validate_public_conformance_reporting_end_to_end,
    "check-performance-governance-surface": action_check_performance_governance_surface,
    "check-performance-governance-schema-surface": action_check_performance_governance_schema_surface,
    "build-performance-dashboard": action_build_performance_dashboard,
    "publish-performance-report": action_publish_performance_report,
    "validate-performance-governance": action_validate_performance_governance,
    "validate-performance-governance-integration": action_validate_performance_governance_integration,
    "validate-performance-governance-end-to-end": action_validate_performance_governance_end_to_end,
    "check-release-foundation-surface": action_check_release_foundation_surface,
    "check-release-foundation-schema-surface": action_check_release_foundation_schema_surface,
    "build-release-manifest": action_build_release_manifest,
    "publish-release-provenance": action_publish_release_provenance,
    "validate-release-foundation": action_validate_release_foundation,
    "check-packaging-channels-surface": action_check_packaging_channels_surface,
    "check-packaging-channels-schema-surface": action_check_packaging_channels_schema_surface,
    "build-package-channels": action_build_package_channels,
    "build-platform-support-matrix": action_build_platform_support_matrix,
    "validate-packaging-channels": action_validate_packaging_channels,
    "validate-packaging-channels-end-to-end": action_validate_packaging_channels_end_to_end,
    "validate-platform-hardening": action_validate_platform_hardening,
    "validate-platform-hardening-end-to-end": action_validate_platform_hardening_end_to_end,
    "check-release-operations-surface": action_check_release_operations_surface,
    "check-release-operations-schema-surface": action_check_release_operations_schema_surface,
    "build-update-manifest": action_build_update_manifest,
    "publish-release-operations": action_publish_release_operations,
    "validate-release-operations": action_validate_release_operations,
    "validate-release-operations-end-to-end": action_validate_release_operations_end_to_end,
    "check-distribution-credibility-surface": action_check_distribution_credibility_surface,
    "check-distribution-credibility-schema-surface": action_check_distribution_credibility_schema_surface,
    "build-distribution-credibility-dashboard": action_build_distribution_credibility_dashboard,
    "publish-distribution-credibility": action_publish_distribution_credibility,
    "validate-distribution-credibility": action_validate_distribution_credibility,
    "validate-distribution-credibility-end-to-end": action_validate_distribution_credibility_end_to_end,
    "check-security-hardening-surface": action_check_security_hardening_surface,
    "check-security-hardening-schema-surface": action_check_security_hardening_schema_surface,
    "build-security-posture": action_build_security_posture,
    "publish-security-advisories": action_publish_security_advisories,
    "validate-security-hardening": action_validate_security_hardening,
    "validate-security-hardening-end-to-end": action_validate_security_hardening_end_to_end,
    "inspect-bonus-tool-integration": action_inspect_bonus_tool_integration,
    "inspect-validation-timing": action_inspect_validation_timing,
    "materialize-project-template": action_materialize_project_template,
    "materialize-canonical-application-workspace": action_materialize_canonical_application_workspace,
    "trace-compile-stages": action_trace_compile_stages,
    "validate-developer-tooling": action_validate_developer_tooling,
    "validate-runnable-developer-tooling": action_validate_runnable_developer_tooling,
    "validate-bonus-experiences": action_validate_bonus_experiences,
    "validate-runnable-bonus-experiences": action_validate_runnable_bonus_experiences,
    "validate-application-architecture": action_validate_application_architecture,
    "validate-runnable-application-architecture": action_validate_runnable_application_architecture,
    "build-package-lock": action_build_package_lock,
    "validate-package-authoring": action_validate_package_authoring,
    "validate-package-mirror": action_validate_package_mirror,
    "validate-package-ecosystem": action_validate_package_ecosystem,
    "validate-runnable-package-ecosystem": action_validate_runnable_package_ecosystem,
    "validate-long-horizon-operations": action_validate_long_horizon_operations,
    "publish-long-horizon-operations": action_publish_long_horizon_operations,
    "validate-adoption-legibility": action_validate_adoption_legibility,
    "publish-adoption-legibility": action_publish_adoption_legibility,
    "validate-governance-sustainability": action_validate_governance_sustainability,
    "publish-governance-sustainability": action_publish_governance_sustainability,
    "publish-planning-issues": action_publish_planning_issues,
    "check-planning-publication-drift": action_check_planning_publication_drift,
    "lint-spec": action_lint_spec,
    "test-default": action_test_default,
    "test-behavior-matrix": action_test_behavior_matrix,
    "test-smoke": action_test_smoke,
    "test-ci": action_test_ci,
    "test-recovery": action_test_recovery,
    "test-compile-wrapper-self-audit": action_test_compile_wrapper_self_audit,
    "test-execution-smoke": action_test_execution_smoke,
    "test-execution-replay": action_test_execution_replay,
    "test-execution-replay-focused": action_test_execution_replay_focused,
    "test-runtime-acceptance": action_test_runtime_acceptance,
    "test-runtime-acceptance-fast": action_test_runtime_acceptance_fast,
    "test-runtime-acceptance-diagnostics": action_test_runtime_acceptance_diagnostics,
    "test-runtime-acceptance-cross-module": action_test_runtime_acceptance_cross_module,
    "test-runtime-acceptance-block-arc": action_test_runtime_acceptance_block_arc,
    "test-runtime-acceptance-concurrency": action_test_runtime_acceptance_concurrency,
    "proof-runtime-architecture": action_proof_runtime_architecture,
    "validate-runtime-architecture": action_validate_runtime_architecture,
    "validate-runnable-bootstrap": action_validate_runnable_bootstrap,
    "validate-block-arc-conformance": action_validate_block_arc_conformance,
    "validate-runnable-block-arc": action_validate_runnable_block_arc,
    "validate-concurrency-conformance": action_validate_concurrency_conformance,
    "validate-runnable-concurrency": action_validate_runnable_concurrency,
    "validate-object-model-conformance": action_validate_object_model_conformance,
    "validate-storage-reflection-conformance": action_validate_storage_reflection_conformance,
    "validate-runnable-object-model": action_validate_runnable_object_model,
    "validate-runnable-storage-reflection": action_validate_runnable_storage_reflection,
    "validate-error-conformance": action_validate_error_conformance,
    "validate-runnable-error": action_validate_runnable_error,
    "validate-interop-conformance": action_validate_interop_conformance,
    "validate-runnable-interop": action_validate_runnable_interop,
    "validate-metaprogramming-conformance": action_validate_metaprogramming_conformance,
    "validate-runnable-metaprogramming": action_validate_runnable_metaprogramming,
    "validate-release-candidate-conformance": action_validate_release_candidate_conformance,
    "validate-runnable-release-candidate": action_validate_runnable_release_candidate,
    "test-fixture-matrix": action_test_fixture_matrix,
    "test-negative-expectations": action_test_negative_expectations,
    "test-full": action_test_full,
    "test-nightly": action_test_nightly,
    "package-runnable-toolchain": action_package_runnable_toolchain,
    "proof-objc3c": action_proof_objc3c,
}



def enrich_action_payload(spec: ActionSpec) -> dict[str, object]:
    payload = asdict(spec)
    payload["mode"] = WORKFLOW_RUNNER_MODE
    payload["runner_path"] = WORKFLOW_RUNNER_SURFACE
    payload["category"] = spec.action.split("-", 1)[0]
    return payload


def list_actions_payload() -> dict[str, object]:
    return {
        "mode": WORKFLOW_RUNNER_MODE,
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "action_count": len(ACTION_SPECS),
        "public_action_count": len(ACTION_SPECS),
        "internal_action_count": 0,
        "public_script_count": 1,
        "actions": [enrich_action_payload(spec) for spec in ACTION_SPECS.values()],
    }


def describe_action_payload(action: str) -> dict[str, object]:
    return enrich_action_payload(ACTION_SPECS[action])



def execute_registered_action(action: str, rest: list[str]) -> int:
    spec = ACTION_SPECS.get(action)
    handler = ACTION_HANDLERS.get(action)
    if spec is None or handler is None:
        print(f"unknown action: {action}", file=sys.stderr)
        return 2
    if rest and not spec.pass_through_args:
        print(f"action does not accept extra arguments: {action}", file=sys.stderr)
        return 2
    return handler(rest)


def main(argv: Sequence[str]) -> int:
    if not argv:
        print(
            f"usage: {WORKFLOW_COMMAND_TEXT} <action> [args...]\n"
            f"       {WORKFLOW_COMMAND_TEXT} --list-json\n"
            f"       {WORKFLOW_COMMAND_TEXT} --describe <action>\n"
            f"       {WORKFLOW_COMMAND_TEXT} --describe-script <package-script>",
            file=sys.stderr,
        )
        return 2

    action, *rest = argv
    if action == "--list-json":
        return emit_json(list_actions_payload())
    if action == "--describe":
        if len(rest) != 1:
            print(f"usage: {WORKFLOW_COMMAND_TEXT} --describe <action>", file=sys.stderr)
            return 2
        describe_action = rest[0]
        if describe_action not in ACTION_SPECS:
            print(f"unknown action: {describe_action}", file=sys.stderr)
            return 2
        return emit_json(describe_action_payload(describe_action))
    if action == "--describe-script":
        if len(rest) != 1:
            print(f"usage: {WORKFLOW_COMMAND_TEXT} --describe-script <package-script>", file=sys.stderr)
            return 2
        describe_script = rest[0]
        if describe_script != "objc3c":
            print(f"unknown package script: {describe_script}", file=sys.stderr)
            return 2
        return emit_json(describe_package_script_payload(describe_script))
    return execute_registered_action(action, rest)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
