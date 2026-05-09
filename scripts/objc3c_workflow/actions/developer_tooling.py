"""Developer tooling, playground, and bonus-experience workflow actions."""

from __future__ import annotations

import json
import sys
from pathlib import Path

from objc3c_tooling.subprocesses import run_capture

from ..commands import run
from ..environment import ROOT
from .developer_tooling_paths import (
    BONUS_EXPERIENCE_INTEGRATION_PY,
    DEFAULT_DEVELOPER_TOOLING_SOURCE,
    DEVELOPER_TOOLING_INTEGRATION_PY,
    FRONTEND_C_API_RUNNER_EXE,
    PROJECT_TEMPLATE_MATERIALIZER_PY,
    PUBLIC_WORKFLOW_REPORT_ROOT,
    REPO_SUPERCLEAN_SOURCE_OF_TRUTH,
    RUNNABLE_BONUS_EXPERIENCE_E2E_PY,
    RUNNABLE_DEVELOPER_TOOLING_E2E_PY,
    SHOWCASE_PORTFOLIO_JSON,
    SHOWCASE_TUTORIAL_WALKTHROUGH_JSON,
)
from .developer_tooling_llvm import (
    action_check_hosted_llvm_capabilities,
    action_check_llvm_capabilities,
    action_inspect_capability_explorer,
    action_test_capability_routed_source_parity,
)
from .developer_tooling_playground import (
    action_format_objc3c,
    action_inspect_editor_tooling,
    action_inspect_playground_repro,
    action_materialize_playground_workspace,
    ensure_frontend_runner_ready,
)


def _execute_registered_action(action: str, rest: list[str]) -> int:
    from scripts.objc3c_workflow.action_dispatch import execute_registered_action

    return execute_registered_action(action, rest)


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


def _write_json_capture(path: Path, stdout: str) -> int:
    try:
        payload = json.loads(stdout)
    except json.JSONDecodeError as exc:
        print(f"developer-tooling-dump: invalid JSON from frontend runner: {exc}", file=sys.stderr)
        return 1
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return 0


def _run_developer_tooling_dump(
    action_name: str,
    dump_flag: str,
    dump_filename: str,
    rest: list[str],
) -> int:
    try:
        source_text, passthrough = _parse_developer_tooling_invocation(rest)
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return 2
    rc = ensure_frontend_runner_ready()
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


def action_inspect_bonus_tool_integration(_: list[str]) -> int:
    native_main = ROOT / "native" / "objc3c" / "src" / "main.cpp"
    if native_main.is_file():
        rc = _execute_registered_action("build-native-contracts", [])
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
