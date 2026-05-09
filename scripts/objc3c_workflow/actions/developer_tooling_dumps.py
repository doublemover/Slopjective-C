"""Developer-tooling frontend JSON dump actions."""

from __future__ import annotations

import json
import sys
from pathlib import Path

from objc3c_tooling.subprocesses import run_capture

from ..environment import ROOT
from .developer_tooling_paths import (
    DEFAULT_DEVELOPER_TOOLING_SOURCE,
    FRONTEND_C_API_RUNNER_EXE,
    PUBLIC_WORKFLOW_REPORT_ROOT,
)
from .developer_tooling_playground import ensure_frontend_runner_ready


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
        print(
            f"developer-tooling-dump: invalid JSON from frontend runner: {exc}",
            file=sys.stderr,
        )
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
