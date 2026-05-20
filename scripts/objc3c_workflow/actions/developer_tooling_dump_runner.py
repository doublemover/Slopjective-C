"""Frontend runner capture helpers for developer-tooling dump actions."""

from __future__ import annotations

import json
import sys
from pathlib import Path

from objc3c_tooling.subprocesses import run_capture

from ..environment import ROOT
from .developer_tooling_dump_contracts import (
    DeveloperToolingDumpContract,
    SUMMARY_OUT_FLAG,
)
from .developer_tooling_dump_inputs import parse_developer_tooling_invocation
from .developer_tooling_paths import FRONTEND_C_API_RUNNER_EXE, PUBLIC_WORKFLOW_REPORT_ROOT
from .developer_tooling_playground import ensure_frontend_runner_ready


def write_json_capture(path: Path, stdout: str) -> int:
    try:
        payload = json.loads(stdout)
    except json.JSONDecodeError as exc:
        print(
            f"developer-tooling-dump: invalid JSON from frontend runner: {exc}",
            file=sys.stderr,
        )
        return 1
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return 0


def dump_summary_path(contract: DeveloperToolingDumpContract) -> Path:
    return PUBLIC_WORKFLOW_REPORT_ROOT / f"{contract.action}-summary.json"


def dump_payload_path(contract: DeveloperToolingDumpContract) -> Path:
    return PUBLIC_WORKFLOW_REPORT_ROOT / contract.dump_filename


def dump_artifact_root(contract: DeveloperToolingDumpContract) -> Path:
    return ROOT / "tmp" / "artifacts" / "objc3c-public-workflow" / contract.action


def repo_relative_text(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def frontend_dump_command(
    contract: DeveloperToolingDumpContract,
    source_text: str,
    artifact_root: Path,
    summary_path: Path,
    passthrough: list[str],
) -> list[str]:
    return [
        str(FRONTEND_C_API_RUNNER_EXE),
        source_text,
        "--out-dir",
        repo_relative_text(artifact_root),
        "--emit-prefix",
        "module",
        SUMMARY_OUT_FLAG,
        str(summary_path),
        contract.dump_flag,
        *passthrough,
    ]


def run_developer_tooling_dump(
    contract: DeveloperToolingDumpContract,
    rest: list[str],
) -> int:
    try:
        source_text, passthrough = parse_developer_tooling_invocation(rest)
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return 2
    rc = ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    summary_path = dump_summary_path(contract)
    dump_path = dump_payload_path(contract)
    artifact_root = dump_artifact_root(contract)
    artifact_root.mkdir(parents=True, exist_ok=True)
    command = frontend_dump_command(
        contract,
        source_text,
        artifact_root,
        summary_path,
        passthrough,
    )
    result = run_capture(command)
    if result.returncode != 0:
        return result.returncode
    rc = write_json_capture(dump_path, result.stdout)
    if rc != 0:
        return rc
    print(f"summary_path: {summary_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return 0
