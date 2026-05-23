"""Process execution helpers for playground workflow actions."""

from __future__ import annotations

import os
import subprocess
import sys

from ..action_execution_dispatch import execute_registered_action
from ..commands import run
from ..environment import ROOT
from .developer_tooling_paths import (
    CHECK_DEVELOPER_TOOLING_DIAGNOSTIC_QUALITY_PY,
    CHECK_DEVELOPER_TOOLING_EDITOR_SOURCE_TRUTH_PY,
    EDITOR_TOOLING_SURFACE_PY,
    FORMAT_OBJC3C_SOURCE_PY,
    LANGUAGE_SERVICE_SURFACE_PY,
    FRONTEND_C_API_RUNNER_EXE,
    REWRITE_OBJC3C_SOURCE_PY,
)


def playground_subprocess_env() -> dict[str, str]:
    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    return env


def ensure_frontend_runner_ready() -> int:
    native_main = ROOT / "native" / "objc3c" / "src" / "main.cpp"
    if not native_main.is_file() and FRONTEND_C_API_RUNNER_EXE.is_file():
        return 0
    return execute_registered_action("build-native-binaries", [])


def run_frontend_playground(
    source_display: str,
    artifact_root_rel: str,
    summary_path_rel: str,
    passthrough: list[str],
    *,
    env: dict[str, str],
) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
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


def run_editor_tooling(
    source_display: str,
    *,
    env: dict[str, str],
) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(EDITOR_TOOLING_SURFACE_PY), source_display],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=env,
    )


def action_inspect_editor_tooling(rest: list[str]) -> int:
    rc = ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    return run([sys.executable, str(EDITOR_TOOLING_SURFACE_PY), *rest])


def action_inspect_artifact(rest: list[str]) -> int:
    rc = ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    return run([sys.executable, str(EDITOR_TOOLING_SURFACE_PY), "--artifact-inspector-only", *rest])


def action_inspect_source_graph(rest: list[str]) -> int:
    rc = ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    return run([sys.executable, str(EDITOR_TOOLING_SURFACE_PY), "--source-graph-only", *rest])


def action_inspect_language_service(rest: list[str]) -> int:
    rc = ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    return run([sys.executable, str(LANGUAGE_SERVICE_SURFACE_PY), *rest])


def action_format_objc3c(rest: list[str]) -> int:
    return run([sys.executable, str(FORMAT_OBJC3C_SOURCE_PY), *rest])


def action_rewrite_objc3c_source(rest: list[str]) -> int:
    return run([sys.executable, str(REWRITE_OBJC3C_SOURCE_PY), *rest])


def action_check_developer_diagnostic_quality(_: list[str]) -> int:
    return run([sys.executable, str(CHECK_DEVELOPER_TOOLING_DIAGNOSTIC_QUALITY_PY)])


def action_check_developer_tooling_editor_source_truth(_: list[str]) -> int:
    return run([sys.executable, str(CHECK_DEVELOPER_TOOLING_EDITOR_SOURCE_TRUTH_PY)])
