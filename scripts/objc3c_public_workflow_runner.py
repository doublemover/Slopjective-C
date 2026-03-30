#!/usr/bin/env python3
"""Unified public workflow runner for the live objc3c command surface."""

from __future__ import annotations

import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
from dataclasses import asdict, dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Callable, Sequence

ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
NPX = shutil.which("npx.cmd") or shutil.which("npx") or "npx"

BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
SMOKE_PS1 = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_PS1 = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
RECOVERY_PS1 = ROOT / "scripts" / "check_objc3c_native_recovery_contract.ps1"
MATRIX_PS1 = ROOT / "scripts" / "run_objc3c_native_fixture_matrix.ps1"
NEGATIVE_EXPECTATIONS_PS1 = ROOT / "scripts" / "check_objc3c_negative_fixture_expectations.ps1"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
PROOF_PS1 = ROOT / "scripts" / "run_objc3c_native_compile_proof.ps1"
SITE_PY = ROOT / "scripts" / "build_site_index.py"
NATIVE_DOCS_PY = ROOT / "scripts" / "build_objc3c_native_docs.py"
PUBLIC_COMMAND_SURFACE_PY = ROOT / "scripts" / "render_objc3c_public_command_surface.py"
DOCUMENTATION_SURFACE_PY = ROOT / "scripts" / "check_documentation_surface.py"
REPO_SUPERCLEAN_SURFACE_PY = ROOT / "scripts" / "check_repo_superclean_surface.py"
SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
SHOWCASE_RUNTIME_PS1 = ROOT / "scripts" / "check_showcase_runtime.ps1"
SHOWCASE_INTEGRATION_PY = ROOT / "scripts" / "check_showcase_integration.py"
RUNNABLE_SHOWCASE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_showcase_end_to_end.py"
GETTING_STARTED_INTEGRATION_PY = ROOT / "scripts" / "check_getting_started_integration.py"
DEVELOPER_TOOLING_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_developer_tooling_integration.py"
BONUS_EXPERIENCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_bonus_experience_integration.py"
RUNTIME_INSPECTOR_BENCHMARK_PY = ROOT / "scripts" / "benchmark_objc3c_runtime_inspector.py"
PERFORMANCE_BENCHMARK_PY = ROOT / "scripts" / "benchmark_objc3c_performance.py"
COMPARATIVE_BASELINES_PY = ROOT / "scripts" / "run_objc3c_comparative_baselines.py"
RUNNABLE_PERFORMANCE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_performance_end_to_end.py"
PERFORMANCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_performance_integration.py"
STDLIB_SURFACE_PY = ROOT / "scripts" / "check_stdlib_surface.py"
MATERIALIZE_STDLIB_PY = ROOT / "scripts" / "materialize_objc3c_stdlib_workspace.py"
STDLIB_FOUNDATION_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_foundation_integration.py"
STDLIB_ADVANCED_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_advanced_integration.py"
STDLIB_PROGRAM_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_program_integration.py"
RUNNABLE_STDLIB_FOUNDATION_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_foundation_end_to_end.py"
RUNNABLE_STDLIB_ADVANCED_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_advanced_end_to_end.py"
RUNNABLE_STDLIB_PROGRAM_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_program_end_to_end.py"
PROJECT_TEMPLATE_MATERIALIZER_PY = ROOT / "scripts" / "materialize_objc3c_project_template.py"
LLVM_CAPABILITIES_PROBE_PY = ROOT / "scripts" / "probe_objc3c_llvm_capabilities.py"
RUNNABLE_BONUS_EXPERIENCE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_bonus_experience_end_to_end.py"
SPEC_LINT_PY = ROOT / "scripts" / "spec_lint.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
RUNTIME_ACCEPTANCE_PY = ROOT / "scripts" / "check_objc3c_runtime_acceptance.py"
RUNTIME_ARCHITECTURE_PROOF_PACKET_PY = ROOT / "scripts" / "check_objc3c_runtime_architecture_proof_packet.py"
RUNTIME_ARCHITECTURE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_runtime_architecture_integration.py"
RUNNABLE_BOOTSTRAP_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_bootstrap_end_to_end.py"
RUNNABLE_BLOCK_ARC_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_block_arc_conformance.py"
RUNNABLE_BLOCK_ARC_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_block_arc_end_to_end.py"
RUNNABLE_CONCURRENCY_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_concurrency_conformance.py"
RUNNABLE_CONCURRENCY_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_concurrency_end_to_end.py"
RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_object_model_conformance.py"
RUNNABLE_OBJECT_MODEL_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_object_model_end_to_end.py"
RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_conformance.py"
RUNNABLE_STORAGE_REFLECTION_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_end_to_end.py"
RUNNABLE_ERROR_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_error_conformance.py"
RUNNABLE_ERROR_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_error_end_to_end.py"
RUNNABLE_INTEROP_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_interop_conformance.py"
RUNNABLE_INTEROP_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_interop_end_to_end.py"
RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_conformance.py"
RUNNABLE_METAPROGRAMMING_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_end_to_end.py"
RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_release_candidate_conformance.py"
RUNNABLE_RELEASE_CANDIDATE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_release_candidate_end_to_end.py"
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


@dataclass(frozen=True)
class ActionSpec:
    action: str
    summary: str
    backend: str
    public_scripts: tuple[str, ...]
    validation_tier: str = ""
    guarantee_owner: str = ""
    pass_through_args: bool = False


ActionHandler = Callable[[list[str]], int]


def run(command: Sequence[str]) -> int:
    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    return subprocess.run(list(command), cwd=ROOT, check=False, env=env).returncode


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=env,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def pwsh_file(script: Path, *args: str) -> int:
    return run([PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(script), *args])


def run_steps(actions: Sequence[str]) -> int:
    for action in actions:
        rc = execute_registered_action(action, [])
        if rc != 0:
            return rc
    return 0


def action_build_default(_: list[str]) -> int:
    return run_steps(["build-native-binaries"])


def action_build_native_binaries(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only")


def action_build_native_contracts(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "contracts-binary")


def action_build_native_full(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "full")


def action_build_native_reconfigure(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only", "-ForceReconfigure")


def action_build_site(_: list[str]) -> int:
    rc = run([sys.executable, str(SITE_PY)])
    if rc != 0:
        return rc
    return run([NPX, "prettier", "--write", "site/index.md"])


def action_check_site(_: list[str]) -> int:
    return run([sys.executable, str(SITE_PY), "--check"])


def action_build_native_docs(_: list[str]) -> int:
    return run([sys.executable, str(NATIVE_DOCS_PY)])


def action_check_native_docs(_: list[str]) -> int:
    return run([sys.executable, str(NATIVE_DOCS_PY), "--check"])


def action_build_public_command_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_SURFACE_PY)])


def action_check_public_command_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_SURFACE_PY), "--check"])


def action_check_documentation_surface(_: list[str]) -> int:
    return run([sys.executable, str(DOCUMENTATION_SURFACE_PY)])


def action_check_repo_superclean_surface(_: list[str]) -> int:
    return run([sys.executable, str(REPO_SUPERCLEAN_SURFACE_PY)])


def action_check_showcase_surface(rest: list[str]) -> int:
    return run([sys.executable, str(SHOWCASE_SURFACE_PY), *rest])


def action_check_stdlib_surface(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_SURFACE_PY)])


def action_materialize_stdlib_workspace(rest: list[str]) -> int:
    return run([sys.executable, str(MATERIALIZE_STDLIB_PY), *rest])


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


def action_validate_runnable_stdlib_advanced(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_ADVANCED_E2E_PY)])


def action_validate_runnable_stdlib_program(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_PROGRAM_E2E_PY)])


def action_validate_runnable_showcase(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_SHOWCASE_E2E_PY)])


def action_validate_getting_started(_: list[str]) -> int:
    return run([sys.executable, str(GETTING_STARTED_INTEGRATION_PY)])


def action_validate_documentation_surface(_: list[str]) -> int:
    commands = [
        [sys.executable, str(SITE_PY)],
        [sys.executable, str(NATIVE_DOCS_PY)],
        [sys.executable, str(PUBLIC_COMMAND_SURFACE_PY)],
        [sys.executable, str(SITE_PY), "--check"],
        [sys.executable, str(NATIVE_DOCS_PY), "--check"],
        [sys.executable, str(PUBLIC_COMMAND_SURFACE_PY), "--check"],
        [sys.executable, str(DOCUMENTATION_SURFACE_PY)],
    ]
    for command in commands:
        rc = run(command)
        if rc != 0:
            return rc
    return 0


def action_validate_repo_superclean(_: list[str]) -> int:
    return run_composite_validation(
        "validate-repo-superclean",
        [
            ("build-native-contracts", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(BUILD_PS1), "-ExecutionMode", "contracts-binary"]),
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
        ],
    )


def action_compile_objc3c(rest: list[str]) -> int:
    return pwsh_file(COMPILE_PS1, *rest)


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

    rc = execute_registered_action("build-native-binaries", [])
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
            "trace-compile-stages",
        ],
        "compile_profile": playground_payload.get("compile_profile", {}),
        "artifact_paths": playground_payload.get("artifact_paths", {}),
        "showcase_examples": playground_payload.get("showcase_examples", []),
        "repro_command": playground_payload.get("dump_commands", {}).get("repro_runner", ""),
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
    rc = execute_registered_action("build-native-binaries", [])
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


def action_validate_bonus_experiences(_: list[str]) -> int:
    return run([sys.executable, str(BONUS_EXPERIENCE_INTEGRATION_PY)])


def action_benchmark_runtime_inspector(rest: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_INSPECTOR_BENCHMARK_PY), *rest])


def action_benchmark_performance(rest: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_BENCHMARK_PY), *rest])


def action_benchmark_comparative_baselines(rest: list[str]) -> int:
    return run([sys.executable, str(COMPARATIVE_BASELINES_PY), *rest])


def action_validate_runnable_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PERFORMANCE_E2E_PY)])


def action_validate_performance_foundation(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_INTEGRATION_PY)])


def action_inspect_bonus_tool_integration(_: list[str]) -> int:
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


def action_test_default(_: list[str]) -> int:
    return run_steps(["test-fast"])


def to_repo_relative(raw_path: str) -> str:
    candidate = Path(raw_path.strip())
    try:
        if candidate.is_absolute():
            return str(candidate.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return raw_path.strip()
    return raw_path.strip().replace("\\", "/")


def extract_report_paths(stdout: str) -> list[str]:
    report_paths: list[str] = []
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith("summary_path:"):
            report_paths.append(to_repo_relative(line.split(":", 1)[1]))
            continue
        if line.startswith("dump_path:"):
            report_paths.append(to_repo_relative(line.split(":", 1)[1]))
            continue
        if line.startswith("workspace_path:"):
            report_paths.append(to_repo_relative(line.split(":", 1)[1]))
            continue
        if line.startswith("template_path:"):
            report_paths.append(to_repo_relative(line.split(":", 1)[1]))
            continue
        if line.startswith("harness_path:"):
            report_paths.append(to_repo_relative(line.split(":", 1)[1]))
            continue
        match = re.search(r"runtime-acceptance:\s+PASS\s+\((.+)\)", line)
        if match:
            report_paths.append(to_repo_relative(match.group(1)))
            continue
        if line.startswith("out_dir:"):
            report_paths.append(to_repo_relative(line.split(":", 1)[1]))
    return report_paths


def load_surface_from_report(
    steps: Sequence[dict[str, object]], surface_key: str
) -> dict[str, object] | None:
    for step in steps:
        report_paths = step.get("report_paths", [])
        if not isinstance(report_paths, list):
            continue
        for raw_path in report_paths:
            if not isinstance(raw_path, str):
                continue
            candidate = ROOT / raw_path
            if not candidate.is_file():
                continue
            try:
                payload = json.loads(candidate.read_text(encoding="utf-8"))
            except json.JSONDecodeError:
                continue
            surface = payload.get(surface_key)
            if isinstance(surface, dict):
                return surface
    return None


def write_composite_validation_report(
    action: str,
    steps: list[dict[str, object]],
    *,
    status: str,
) -> Path:
    PUBLIC_WORKFLOW_REPORT_ROOT.mkdir(parents=True, exist_ok=True)
    report_path = PUBLIC_WORKFLOW_REPORT_ROOT / f"{action}.json"
    payload = {
        "action": action,
        "status": status,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "runner_path": "scripts/objc3c_public_workflow_runner.py",
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
    if (
        action == "test-runtime-acceptance"
        and os.environ.get("OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN") == "1"
    ):
        return {
            "action": action,
            "command": [str(token) for token in command],
            "exit_code": 0,
            "report_paths": ["tmp/reports/runtime/acceptance/summary.json"],
            "report_reused": True,
        }
    result = run_capture(command)
    return {
        "action": action,
        "command": [str(token) for token in command],
        "exit_code": result.returncode,
        "report_paths": extract_report_paths(result.stdout),
    }


def run_composite_validation(action: str, steps: list[tuple[str, Sequence[str]]]) -> int:
    results: list[dict[str, object]] = []
    for step_action, command in steps:
        step = run_composite_step(step_action, command)
        results.append(step)
        if step["exit_code"] != 0:
            report_path = write_composite_validation_report(action, results, status="FAIL")
            print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
            return int(step["exit_code"])
    report_path = write_composite_validation_report(action, results, status="PASS")
    print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
    return 0


def action_test_fast(_: list[str]) -> int:
    return run_composite_validation(
        "test-fast",
        [
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1), "-Limit", "12"]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
        ],
    )


def action_test_smoke(_: list[str]) -> int:
    return run_steps(["test-execution-smoke"])


def action_test_ci(_: list[str]) -> int:
    return run_composite_validation(
        "test-ci",
        [
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            ("validate-developer-tooling", [sys.executable, str(DEVELOPER_TOOLING_INTEGRATION_PY)]),
            ("validate-bonus-experiences", [sys.executable, str(BONUS_EXPERIENCE_INTEGRATION_PY)]),
            ("validate-stdlib-foundation", [sys.executable, str(STDLIB_FOUNDATION_INTEGRATION_PY)]),
            ("validate-stdlib-advanced", [sys.executable, str(STDLIB_ADVANCED_INTEGRATION_PY)]),
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


def action_test_runtime_acceptance(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY)])


def action_proof_runtime_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ARCHITECTURE_PROOF_PACKET_PY)])


def action_validate_runtime_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ARCHITECTURE_INTEGRATION_PY)])


def action_validate_runnable_bootstrap(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BOOTSTRAP_E2E_PY)])


def action_validate_block_arc_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BLOCK_ARC_CONFORMANCE_PY)])


def action_validate_runnable_block_arc(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BLOCK_ARC_E2E_PY)])


def action_validate_concurrency_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_CONCURRENCY_CONFORMANCE_PY)])


def action_validate_runnable_concurrency(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_CONCURRENCY_E2E_PY)])


def action_validate_object_model_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY)])


def action_validate_runnable_object_model(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_OBJECT_MODEL_E2E_PY)])


def action_validate_storage_reflection_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY)])


def action_validate_runnable_storage_reflection(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STORAGE_REFLECTION_E2E_PY)])


def action_validate_error_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_ERROR_CONFORMANCE_PY)])


def action_validate_runnable_error(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_ERROR_E2E_PY)])


def action_validate_interop_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_INTEROP_CONFORMANCE_PY)])


def action_validate_runnable_interop(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_INTEROP_E2E_PY)])


def action_validate_metaprogramming_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY)])


def action_validate_runnable_metaprogramming(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_METAPROGRAMMING_E2E_PY)])


def action_validate_release_candidate_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY)])


def action_validate_runnable_release_candidate(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_RELEASE_CANDIDATE_E2E_PY)])


def action_test_fixture_matrix(rest: list[str]) -> int:
    return pwsh_file(MATRIX_PS1, *rest)


def action_test_negative_expectations(rest: list[str]) -> int:
    return pwsh_file(NEGATIVE_EXPECTATIONS_PS1, *rest)


def action_test_full(_: list[str]) -> int:
    return run_composite_validation(
        "test-full",
        [
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1)]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
        ],
    )


def action_test_nightly(_: list[str]) -> int:
    return run_composite_validation(
        "test-nightly",
        [
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1)]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
            ("test-recovery", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(RECOVERY_PS1)]),
            ("test-fixture-matrix", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(MATRIX_PS1)]),
            ("test-negative-expectations", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(NEGATIVE_EXPECTATIONS_PS1)]),
        ],
    )


def action_package_runnable_toolchain(_: list[str]) -> int:
    return pwsh_file(PACKAGE_PS1)


def action_proof_objc3c(_: list[str]) -> int:
    return pwsh_file(PROOF_PS1)


ACTION_SPECS: dict[str, ActionSpec] = {
    "build-default": ActionSpec("build-default", "default public build entrypoint", "runner-internal", ("build",)),
    "build-native-binaries": ActionSpec("build-native-binaries", "build native binaries", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native",)),
    "build-native-contracts": ActionSpec("build-native-contracts", "build native contracts/binaries contract-artifact surface", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native:contracts",)),
    "build-native-full": ActionSpec("build-native-full", "run full native build", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native:full",)),
    "build-native-reconfigure": ActionSpec("build-native-reconfigure", "force native reconfigure build", "pwsh:scripts/build_objc3c_native.ps1", ("build:objc3c-native:reconfigure",)),
    "build-site": ActionSpec("build-site", "build published site output and format it", "python:scripts/build_site_index.py + npx prettier", ("build:site",)),
    "check-site": ActionSpec("check-site", "check generated site output for drift", "python:scripts/build_site_index.py --check", ("check:site",), validation_tier="docs", guarantee_owner="published site index generation stays in sync with site/src inputs"),
    "build-native-docs": ActionSpec("build-native-docs", "build the generated native implementation docs", "python:scripts/build_objc3c_native_docs.py", ("build:docs:native",)),
    "check-native-docs": ActionSpec("check-native-docs", "check generated native implementation docs for drift", "python:scripts/build_objc3c_native_docs.py --check", ("check:docs:native",), validation_tier="docs", guarantee_owner="generated native implementation documentation stays in sync with docs/objc3c-native/src inputs"),
    "build-public-command-surface": ActionSpec("build-public-command-surface", "build the generated public command-surface appendix", "python:scripts/render_objc3c_public_command_surface.py", ("build:docs:commands",)),
    "check-public-command-surface": ActionSpec("check-public-command-surface", "check the generated public command-surface appendix for drift", "python:scripts/render_objc3c_public_command_surface.py --check", ("check:docs:commands",), validation_tier="docs", guarantee_owner="operator-facing machine appendix stays in sync with the live workflow runner and package scripts"),
    "check-documentation-surface": ActionSpec("check-documentation-surface", "check the reader-facing documentation structure and machine-appendix boundary", "python:scripts/check_documentation_surface.py", ("check:docs:surface",), validation_tier="docs", guarantee_owner="reader-facing onboarding, site structure, and machine-appendix boundary stay accessible and explicit"),
    "check-showcase-surface": ActionSpec("check-showcase-surface", "check the live showcase portfolio and compile its example sources through the public compiler path", "python:scripts/check_showcase_surface.py", ("check:showcase:surface",), validation_tier="repo", guarantee_owner="showcase examples stay compile-coupled, checked in, and tied to the public compiler path", pass_through_args=True),
    "check-stdlib-surface": ActionSpec("check-stdlib-surface", "check the checked-in stdlib boundary contracts, canonical module inventory, package alias mapping, and lowering/import artifact contract", "python:scripts/check_stdlib_surface.py", ("check:stdlib:surface",), validation_tier="repo", guarantee_owner="stdlib roots, canonical module inventory, package alias mapping, and lowering/import artifact contract stay checked in and coherent"),
    "validate-showcase-runtime": ActionSpec("validate-showcase-runtime", "compile, link, and run the checked-in showcase examples through the live runtime launch contract", "pwsh:scripts/check_showcase_runtime.ps1", (), validation_tier="repo", guarantee_owner="showcase examples stay runnable through the real runtime archive and launch-contract wiring", pass_through_args=True),
    "validate-showcase": ActionSpec("validate-showcase", "run the integrated showcase compile and runtime validation flow", "python:scripts/check_showcase_integration.py", ("test:showcase",), validation_tier="repo", guarantee_owner="showcase examples stay compiled, runnable, and wired into the normal repo validation path"),
    "validate-runnable-showcase": ActionSpec("validate-runnable-showcase", "run packaged showcase compile link and execution validation from the staged runnable toolchain bundle", "python:scripts/check_objc3c_runnable_showcase_end_to_end.py", ("test:showcase:e2e",), validation_tier="full", guarantee_owner="showcase examples stay publishable and runnable from the staged runnable toolchain bundle"),
    "validate-getting-started": ActionSpec("validate-getting-started", "run the integrated getting-started tutorial and onboarding validation flow", "python:scripts/check_getting_started_integration.py", ("test:getting-started",), validation_tier="repo", guarantee_owner="getting-started tutorials stay compile-coupled, runnable, and wired into the normal repo validation path"),
    "check-repo-superclean-surface": ActionSpec("check-repo-superclean-surface", "check the build-emitted repo superclean source-of-truth artifact", "python:scripts/check_repo_superclean_surface.py", ("check:repo:surface",), validation_tier="repo", guarantee_owner="native build emits the canonical repo-cleanup roots, outputs, and command names as one source-of-truth artifact"),
    "validate-documentation-surface": ActionSpec("validate-documentation-surface", "run the full documentation build and reader-surface validation flow", "runner-internal + generated documentation checks", ("test:docs",), validation_tier="docs", guarantee_owner="site output, native docs, command appendix, and reader-facing onboarding remain buildable, in sync, and explicit"),
    "validate-repo-superclean": ActionSpec("validate-repo-superclean", "build the canonical repo surface and run the integrated hygiene/docs/superclean checks", "runner-internal + native build contracts + task hygiene gate", ("test:repo",), validation_tier="repo", guarantee_owner="repo roots, checked-in docs, generated outputs, and machine-owned boundaries remain canonical and enforced"),
    "compile-objc3c": ActionSpec("compile-objc3c", "compile one Objective-C 3 fixture through the native compiler", "pwsh:scripts/objc3c_native_compile.ps1", ("compile:objc3c",), pass_through_args=True),
    "materialize-playground-workspace": ActionSpec("materialize-playground-workspace", "compile one source through the live frontend runner and materialize a machine-owned playground workspace contract under tmp", "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe", ("build:objc3c:playground",), validation_tier="repo", guarantee_owner="playground workspaces stay machine-owned, compile-coupled, and rooted in tmp outputs instead of shared proof-only buckets", pass_through_args=True),
    "materialize-stdlib-workspace": ActionSpec("materialize-stdlib-workspace", "copy the checked-in stdlib workspace and lowering/import contracts into a machine-owned artifact root under tmp", "python:scripts/materialize_objc3c_stdlib_workspace.py", ("build:objc3c:stdlib",), validation_tier="repo", guarantee_owner="stdlib workspace materializations stay machine-owned and derived from the checked-in stdlib root plus lowering/import contract surface", pass_through_args=True),
    "validate-stdlib-foundation": ActionSpec("validate-stdlib-foundation", "run the integrated stdlib boundary and smoke validation flow", "python:scripts/check_objc3c_stdlib_foundation_integration.py", ("test:stdlib",), validation_tier="repo", guarantee_owner="stdlib boundary contracts, lowering/import artifact expectations, workspace materialization, and smoke compilation stay executable on the live public workflow"),
    "validate-stdlib-advanced": ActionSpec("validate-stdlib-advanced", "run the integrated advanced stdlib helper validation flow", "python:scripts/check_objc3c_stdlib_advanced_integration.py", ("test:stdlib:advanced",), validation_tier="repo", guarantee_owner="advanced stdlib helper module contracts, profile gates, and shared smoke compilation stay executable on the live public workflow"),
    "validate-stdlib-program": ActionSpec("validate-stdlib-program", "run the integrated stdlib program docs, showcase, tutorial, and capability-adoption validation flow", "python:scripts/check_objc3c_stdlib_program_integration.py", ("test:stdlib:program",), validation_tier="repo", guarantee_owner="stdlib publish/adoption docs, capability demos, tutorial routing, and stdlib smoke integration stay executable on the live public workflow"),
    "validate-runnable-stdlib-advanced": ActionSpec("validate-runnable-stdlib-advanced", "validate runnable advanced stdlib helper packaging and smoke compilation end to end from the package root", "python:scripts/check_objc3c_runnable_stdlib_advanced_end_to_end.py", ("test:stdlib:advanced:e2e",), validation_tier="full", guarantee_owner="packaged advanced stdlib helper contracts, profile gates, and subset smoke compilation stay reproducible from the staged runnable toolchain bundle"),
    "validate-runnable-stdlib-foundation": ActionSpec("validate-runnable-stdlib-foundation", "validate runnable stdlib foundation packaging and smoke compilation end to end from the package root", "python:scripts/check_objc3c_runnable_stdlib_foundation_end_to_end.py", ("test:stdlib:e2e",), validation_tier="full", guarantee_owner="packaged stdlib boundary contracts, lowering/import artifact metadata, module smoke compilation, and runtime-archive linkage stay reproducible from the staged runnable toolchain bundle"),
    "validate-runnable-stdlib-program": ActionSpec("validate-runnable-stdlib-program", "validate the staged runnable stdlib program docs/example package surface end to end", "python:scripts/check_objc3c_runnable_stdlib_program_end_to_end.py", ("test:stdlib:program:e2e",), validation_tier="full", guarantee_owner="packaged stdlib program docs, showcase examples, and publish-input metadata stay reproducible from the staged runnable toolchain bundle"),
    "inspect-capability-explorer": ActionSpec("inspect-capability-explorer", "probe LLVM and backend-routing capability state through the live capability explorer surface", "python:scripts/probe_objc3c_llvm_capabilities.py", ("inspect:objc3c:capabilities",), validation_tier="repo", guarantee_owner="capability explorer payloads stay tied to the live LLVM probe and backend-routing contracts", pass_through_args=True),
    "inspect-playground-repro": ActionSpec("inspect-playground-repro", "compile one source through the frontend C API runner and dump the playground and repro object", "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe", ("inspect:objc3c:playground",), validation_tier="repo", guarantee_owner="playground and repro payloads stay tied to the real frontend runner summary, emitted artifacts, and executable replay command", pass_through_args=True),
    "inspect-compile-observability": ActionSpec("inspect-compile-observability", "compile one source through the frontend C API runner and dump the structured observability object", "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe", ("inspect:objc3c:observability",), validation_tier="repo", guarantee_owner="developer-facing compile observability stays tied to the real frontend runner summary and emitted artifacts", pass_through_args=True),
    "inspect-runtime-inspector": ActionSpec("inspect-runtime-inspector", "compile one source through the frontend C API runner and dump the runtime inspector object", "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe", ("inspect:objc3c:runtime",), validation_tier="repo", guarantee_owner="developer-facing runtime inspection stays tied to the real emitted object artifact and runtime ABI boundary models", pass_through_args=True),
    "benchmark-runtime-inspector": ActionSpec("benchmark-runtime-inspector", "measure the live runtime-inspector and capability-explorer workflow and write a reproducible benchmark report", "python:scripts/benchmark_objc3c_runtime_inspector.py", ("inspect:objc3c:benchmark",), validation_tier="repo", guarantee_owner="runtime inspector timing and capability comparisons stay tied to executable public actions and real emitted artifacts", pass_through_args=True),
    "benchmark-performance": ActionSpec("benchmark-performance", "measure the checked-in objc3 showcase workloads and write reproducible compile/runtime telemetry packets", "python:scripts/benchmark_objc3c_performance.py", ("inspect:objc3c:performance",), validation_tier="repo", guarantee_owner="objc3 benchmark telemetry stays tied to checked-in showcase workloads and raw sample packets", pass_through_args=True),
    "benchmark-comparative-baselines": ActionSpec("benchmark-comparative-baselines", "measure the checked-in ObjC2 Swift and C++ baseline workloads and write reproducible comparison telemetry packets", "python:scripts/run_objc3c_comparative_baselines.py", ("inspect:objc3c:comparative-baselines",), validation_tier="repo", guarantee_owner="comparative baseline telemetry stays tied to checked-in language fixtures and recorded availability states", pass_through_args=True),
    "validate-runnable-performance": ActionSpec("validate-runnable-performance", "validate the staged runnable toolchain performance surface end to end from the package root", "python:scripts/check_objc3c_runnable_performance_end_to_end.py", ("test:objc3c:runnable-performance",), validation_tier="full", guarantee_owner="packaged benchmark fixtures, schemas, and benchmark command surfaces stay reproducible from the staged runnable toolchain bundle"),
    "validate-performance-foundation": ActionSpec("validate-performance-foundation", "run the integrated benchmark and comparative baseline validation flow", "python:scripts/check_objc3c_performance_integration.py", ("test:objc3c:performance",), validation_tier="repo", guarantee_owner="benchmark foundations stay executable across live objc3 workloads, comparative baselines, and the staged runnable bundle"),
    "inspect-bonus-tool-integration": ActionSpec("inspect-bonus-tool-integration", "emit the live bonus-tool integration surface from the build-owned source-of-truth artifact and checked-in showcase/tutorial contracts", "runner-internal + tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json", ("inspect:objc3c:bonus-tools",), validation_tier="repo", guarantee_owner="bonus-tool integration stays rooted in the build-owned source-of-truth artifact and checked-in showcase/tutorial contracts"),
    "materialize-project-template": ActionSpec("materialize-project-template", "materialize a machine-owned project template from the checked-in showcase portfolio and drive the live bonus-tool demo harness against it", "python:scripts/materialize_objc3c_project_template.py", ("build:objc3c:template",), validation_tier="repo", guarantee_owner="starter-template and demo-harness outputs stay derived from checked-in showcase sources and executable public actions", pass_through_args=True),
    "trace-compile-stages": ActionSpec("trace-compile-stages", "compile one source through the frontend C API runner and dump the stage trace object", "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe", ("trace:objc3c:stages",), validation_tier="repo", guarantee_owner="developer-facing compile stage traces stay tied to the real frontend runner stage summaries and process exit semantics", pass_through_args=True),
    "validate-developer-tooling": ActionSpec("validate-developer-tooling", "run the integrated developer-tooling inspect and trace validation flow", "python:scripts/check_objc3c_developer_tooling_integration.py", ("test:objc3c:developer-tooling",), validation_tier="repo", guarantee_owner="developer-facing inspect and trace commands stay executable, artifact-backed, and tied to the live frontend runner"),
    "validate-bonus-experiences": ActionSpec("validate-bonus-experiences", "run the integrated bonus-experience validation flow across the live showcase tutorial and template surfaces", "python:scripts/check_objc3c_bonus_experience_integration.py", ("test:bonus-experiences",), validation_tier="repo", guarantee_owner="bonus-experience workflows stay executable, template-derived, and tied to the live showcase tutorial and developer-tooling surfaces"),
    "validate-runnable-bonus-experiences": ActionSpec("validate-runnable-bonus-experiences", "validate template-derived bonus experiences end to end from the staged runnable toolchain bundle", "python:scripts/check_objc3c_runnable_bonus_experience_end_to_end.py", ("test:bonus-experiences:e2e",), validation_tier="full", guarantee_owner="staged runnable toolchain bundles preserve bonus-experience template compilation runtime execution and capability-probe semantics"),
    "lint-spec": ActionSpec("lint-spec", "run spec lint", "python:scripts/spec_lint.py", ("lint:spec",)),
    "test-default": ActionSpec("test-default", "default public test entrypoint", "runner-internal", ("test",)),
    "test-fast": ActionSpec("test-fast", "fast public validation entrypoint", "runner-internal + targeted smoke slice", ("test:fast",), validation_tier="fast", guarantee_owner="runtime acceptance, canonical replay, and a bounded smoke slice"),
    "test-smoke": ActionSpec("test-smoke", "developer smoke validation entrypoint", "runner-internal", ("test:smoke",), validation_tier="smoke", guarantee_owner="full execution smoke corpus"),
    "test-ci": ActionSpec("test-ci", "CI-oriented public validation entrypoint", "runner-internal + direct task hygiene", ("test:ci",), validation_tier="ci", guarantee_owner="task hygiene, developer-tooling integration, stdlib foundation and advanced validation, bonus-experience validation, runtime acceptance, canonical replay, and full execution smoke validation"),
    "test-recovery": ActionSpec("test-recovery", "native recovery contract suite", "pwsh:scripts/check_objc3c_native_recovery_contract.ps1", ("test:objc3c",), validation_tier="recovery", guarantee_owner="recovery compile success and deterministic recovery diagnostics", pass_through_args=True),
    "test-execution-smoke": ActionSpec("test-execution-smoke", "native execution smoke suite", "pwsh:scripts/check_objc3c_native_execution_smoke.ps1", ("test:objc3c:execution-smoke",), validation_tier="smoke", guarantee_owner="compile/link/run execution behavior", pass_through_args=True),
    "test-execution-replay": ActionSpec("test-execution-replay", "native execution replay proof suite", "pwsh:scripts/check_objc3c_execution_replay_proof.ps1", ("test:objc3c:execution-replay-proof",), validation_tier="full", guarantee_owner="replay and native-output truth", pass_through_args=True),
    "test-runtime-acceptance": ActionSpec("test-runtime-acceptance", "runtime acceptance suite", "python:scripts/check_objc3c_runtime_acceptance.py", ("test:objc3c:runtime-acceptance",), validation_tier="fast", guarantee_owner="runtime acceptance and ABI/accessor proof"),
    "proof-runtime-architecture": ActionSpec("proof-runtime-architecture", "emit the integrated runtime architecture proof packet", "python:scripts/check_objc3c_runtime_architecture_proof_packet.py", ("proof:objc3c:runtime-architecture",)),
    "validate-runtime-architecture": ActionSpec("validate-runtime-architecture", "validate runtime architecture across the full public workflow and proof packet", "python:scripts/check_objc3c_runtime_architecture_integration.py", ("test:objc3c:runtime-architecture",), validation_tier="full", guarantee_owner="full public workflow and runtime architecture proof packet alignment"),
    "validate-runnable-bootstrap": ActionSpec("validate-runnable-bootstrap", "validate the staged runnable toolchain end to end from the package root", "python:scripts/check_objc3c_runnable_bootstrap_end_to_end.py", ("test:objc3c:runnable-bootstrap",), validation_tier="full", guarantee_owner="packaged compile, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-block-arc-conformance": ActionSpec("validate-block-arc-conformance", "validate runnable block/ARC conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_block_arc_conformance.py", ("test:objc3c:block-arc-conformance",), validation_tier="full", guarantee_owner="integrated block/ARC conformance over the live runtime architecture workflow"),
    "validate-runnable-block-arc": ActionSpec("validate-runnable-block-arc", "validate runnable block/ARC execution end to end from the package root", "python:scripts/check_objc3c_runnable_block_arc_end_to_end.py", ("test:objc3c:runnable-block-arc",), validation_tier="full", guarantee_owner="packaged compile, block/ARC probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-concurrency-conformance": ActionSpec("validate-concurrency-conformance", "validate runnable concurrency conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_concurrency_conformance.py", ("test:objc3c:concurrency-conformance",), validation_tier="full", guarantee_owner="integrated async/task/executor/actor conformance over the live runtime architecture workflow"),
    "validate-runnable-concurrency": ActionSpec("validate-runnable-concurrency", "validate runnable concurrency execution end to end from the package root", "python:scripts/check_objc3c_runnable_concurrency_end_to_end.py", ("test:objc3c:runnable-concurrency",), validation_tier="full", guarantee_owner="packaged compile, concurrency probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-object-model-conformance": ActionSpec("validate-object-model-conformance", "validate runnable object-model conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_object_model_conformance.py", ("test:objc3c:object-model-conformance",), validation_tier="full", guarantee_owner="integrated object-model conformance over the live runtime architecture workflow"),
    "validate-storage-reflection-conformance": ActionSpec("validate-storage-reflection-conformance", "validate runnable storage/reflection conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_storage_reflection_conformance.py", ("test:objc3c:storage-reflection-conformance",), validation_tier="full", guarantee_owner="integrated storage/accessor/reflection conformance over the live runtime architecture workflow"),
    "validate-runnable-object-model": ActionSpec("validate-runnable-object-model", "validate runnable object-model execution end to end from the package root", "python:scripts/check_objc3c_runnable_object_model_end_to_end.py", ("test:objc3c:runnable-object-model",), validation_tier="full", guarantee_owner="packaged compile, object-model probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-storage-reflection": ActionSpec("validate-runnable-storage-reflection", "validate runnable storage/reflection execution end to end from the package root", "python:scripts/check_objc3c_runnable_storage_reflection_end_to_end.py", ("test:objc3c:runnable-storage-reflection",), validation_tier="full", guarantee_owner="packaged compile, storage/reflection probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-error-conformance": ActionSpec("validate-error-conformance", "validate runnable error conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_error_conformance.py", ("test:objc3c:error-conformance",), validation_tier="full", guarantee_owner="integrated error conformance over the live runtime architecture workflow"),
    "validate-runnable-error": ActionSpec("validate-runnable-error", "validate runnable error execution end to end from the package root", "python:scripts/check_objc3c_runnable_error_end_to_end.py", ("test:objc3c:runnable-error",), validation_tier="full", guarantee_owner="packaged compile, error probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-interop-conformance": ActionSpec("validate-interop-conformance", "validate runnable mixed-module and interop conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_interop_conformance.py", ("test:objc3c:interop-conformance",), validation_tier="full", guarantee_owner="integrated mixed-module runtime packaging and interop conformance over the live runtime architecture workflow"),
    "validate-runnable-interop": ActionSpec("validate-runnable-interop", "validate runnable mixed-module and interop execution end to end from the package root", "python:scripts/check_objc3c_runnable_interop_end_to_end.py", ("test:objc3c:runnable-interop",), validation_tier="full", guarantee_owner="packaged compile, interop probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-metaprogramming-conformance": ActionSpec("validate-metaprogramming-conformance", "validate runnable metaprogramming conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_metaprogramming_conformance.py", ("test:objc3c:metaprogramming-conformance",), validation_tier="full", guarantee_owner="integrated metaprogramming conformance over the live runtime architecture workflow"),
    "validate-runnable-metaprogramming": ActionSpec("validate-runnable-metaprogramming", "validate runnable metaprogramming execution end to end from the package root", "python:scripts/check_objc3c_runnable_metaprogramming_end_to_end.py", ("test:objc3c:runnable-metaprogramming",), validation_tier="full", guarantee_owner="packaged compile, metaprogramming probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-release-candidate-conformance": ActionSpec("validate-release-candidate-conformance", "validate runnable release-candidate conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_release_candidate_conformance.py", ("test:objc3c:release-candidate-conformance",), validation_tier="full", guarantee_owner="integrated public-claims strict-profile and release-candidate conformance over the live runtime architecture workflow"),
    "validate-runnable-release-candidate": ActionSpec("validate-runnable-release-candidate", "validate runnable release-candidate packaging and validation end to end from the package root", "python:scripts/check_objc3c_runnable_release_candidate_end_to_end.py", ("test:objc3c:runnable-release-candidate",), validation_tier="full", guarantee_owner="packaged compile, release-candidate validation, runtime probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "test-fixture-matrix": ActionSpec("test-fixture-matrix", "broad positive recovery fixture matrix sweep", "pwsh:scripts/run_objc3c_native_fixture_matrix.ps1", ("test:objc3c:fixture-matrix",), validation_tier="nightly", guarantee_owner="broad positive corpus artifact sanity", pass_through_args=True),
    "test-negative-expectations": ActionSpec("test-negative-expectations", "static negative fixture expectation enforcement", "pwsh:scripts/check_objc3c_negative_fixture_expectations.ps1", ("test:objc3c:negative-expectations",), validation_tier="nightly", guarantee_owner="negative expectation header and token enforcement", pass_through_args=True),
    "test-full": ActionSpec("test-full", "full developer validation entrypoint", "runner-internal + direct PowerShell suites", ("test:objc3c:full",), validation_tier="full", guarantee_owner="smoke, runtime acceptance, and replay without full recovery fan-out"),
    "test-nightly": ActionSpec("test-nightly", "exhaustive validation entrypoint", "runner-internal + direct PowerShell suites", ("test:objc3c:nightly",), validation_tier="nightly", guarantee_owner="full validation plus recovery and broad corpus sweeps"),
    "package-runnable-toolchain": ActionSpec("package-runnable-toolchain", "package the runnable native toolchain", "pwsh:scripts/package_objc3c_runnable_toolchain.ps1", ("package:objc3c-native:runnable-toolchain",)),
    "proof-objc3c": ActionSpec("proof-objc3c", "run the native compile proof workflow", "pwsh:scripts/run_objc3c_native_compile_proof.ps1", ("proof:objc3c",)),
}


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
    "check-documentation-surface": action_check_documentation_surface,
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
    "benchmark-runtime-inspector": action_benchmark_runtime_inspector,
    "benchmark-performance": action_benchmark_performance,
    "benchmark-comparative-baselines": action_benchmark_comparative_baselines,
    "validate-runnable-performance": action_validate_runnable_performance,
    "validate-performance-foundation": action_validate_performance_foundation,
    "inspect-bonus-tool-integration": action_inspect_bonus_tool_integration,
    "materialize-project-template": action_materialize_project_template,
    "trace-compile-stages": action_trace_compile_stages,
    "validate-developer-tooling": action_validate_developer_tooling,
    "validate-bonus-experiences": action_validate_bonus_experiences,
    "validate-runnable-bonus-experiences": action_validate_runnable_bonus_experiences,
    "lint-spec": action_lint_spec,
    "test-default": action_test_default,
    "test-fast": action_test_fast,
    "test-smoke": action_test_smoke,
    "test-ci": action_test_ci,
    "test-recovery": action_test_recovery,
    "test-execution-smoke": action_test_execution_smoke,
    "test-execution-replay": action_test_execution_replay,
    "test-runtime-acceptance": action_test_runtime_acceptance,
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


def emit_json(payload: object) -> int:
    json.dump(payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


def list_actions_payload() -> dict[str, object]:
    return {
        "mode": "public_runner-parameterized-task-runner-v1",
        "runner_path": "scripts/objc3c_public_workflow_runner.py",
        "action_count": len(ACTION_SPECS),
        "actions": [asdict(spec) for spec in ACTION_SPECS.values()],
    }


def describe_action_payload(action: str) -> dict[str, object]:
    spec = ACTION_SPECS[action]
    payload = asdict(spec)
    payload["mode"] = "public_runner-parameterized-task-runner-v1"
    payload["runner_path"] = "scripts/objc3c_public_workflow_runner.py"
    return payload


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
            "usage: objc3c_public_workflow_runner.py <action> [args...]\n"
            "       objc3c_public_workflow_runner.py --list-json\n"
            "       objc3c_public_workflow_runner.py --describe <action>",
            file=sys.stderr,
        )
        return 2

    action, *rest = argv
    if action == "--list-json":
        return emit_json(list_actions_payload())
    if action == "--describe":
        if len(rest) != 1:
            print("usage: objc3c_public_workflow_runner.py --describe <action>", file=sys.stderr)
            return 2
        describe_action = rest[0]
        if describe_action not in ACTION_SPECS:
            print(f"unknown action: {describe_action}", file=sys.stderr)
            return 2
        return emit_json(describe_action_payload(describe_action))
    return execute_registered_action(action, rest)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
