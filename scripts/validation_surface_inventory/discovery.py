"""Source discovery for validation surface inventory inputs."""

from __future__ import annotations

import json
import subprocess
from pathlib import Path
from typing import Any, Sequence

from scripts.objc3c_workflow.public_command_api import public_workflow_list_payload
from objc3c_tooling.subprocesses import python_script_command

from .paths import ACCEPTANCE_HARNESS, CHECK_ROOTS, PACKAGE_JSON, ROOT


def run_json(command: list[str]) -> Any:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout or f"command failed: {' '.join(command)}")
    return json.loads(completed.stdout)


def all_check_py_files(check_roots: Sequence[Path] = CHECK_ROOTS) -> list[Path]:
    files: list[Path] = []
    for root in check_roots:
        if root.exists():
            files.extend(path for path in root.rglob("check_*.py") if path.is_file())
    return sorted(files)


def all_test_check_py_files(check_roots: Sequence[Path] = CHECK_ROOTS) -> list[Path]:
    files: list[Path] = []
    for root in check_roots:
        if root.exists():
            files.extend(path for path in root.rglob("test_check_*.py") if path.is_file())
    return sorted(files)


def all_validation_ps1_files(scripts_root: Path | None = None) -> list[Path]:
    root = scripts_root or ROOT / "scripts"
    if not root.exists():
        return []
    files = [path for path in root.rglob("check_*.ps1") if path.is_file()]
    files.extend(path for path in root.rglob("run_*.ps1") if path.is_file() and "fixture_matrix" in path.name)
    return sorted(set(files))


def package_scripts(package_json: Path = PACKAGE_JSON) -> dict[str, str]:
    payload = json.loads(package_json.read_text(encoding="utf-8"))
    return dict(payload.get("scripts", {}))


def acceptance_harness_catalog(acceptance_harness: Path = ACCEPTANCE_HARNESS) -> dict[str, Any]:
    return run_json(python_script_command(acceptance_harness, "--list-suites"))


def workflow_backend_text() -> str:
    workflow_payload = public_workflow_list_payload()
    return "\n".join(
        str(action.get("backend", "")) for action in workflow_payload.get("actions", []) if isinstance(action, dict)
    )
