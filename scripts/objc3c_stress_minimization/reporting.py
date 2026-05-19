"""Result payload construction and rendering for stress minimization."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from .models import MinCase
from .paths import SUMMARY_CONTRACT_ID


def json_text(payload: dict[str, Any]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.write_text(json_text(payload), encoding="utf-8")


def build_invocation_payload(compiler: Path, case: MinCase, timeout_sec: float) -> dict[str, Any]:
    return {
        "compiler": repo_rel(compiler),
        "source_path": repo_rel(case.source_path),
        "timeout_sec": timeout_sec,
    }


def build_failure_summary(case: MinCase, baseline: dict[str, Any]) -> dict[str, Any]:
    return {
        "case_id": case.case_id,
        "subsystem": case.subsystem,
        "returncode": baseline["returncode"],
        "diagnostic_lines": baseline["diagnostic_lines"],
        "signature_sha256": baseline["signature_sha256"],
    }


def build_reducer_plan(case: MinCase, attempts: list[dict[str, Any]]) -> dict[str, Any]:
    return {
        "case_id": case.case_id,
        "attempt_count": len(attempts),
        "accepted_reduction_count": sum(1 for attempt in attempts if attempt["accepted"]),
        "attempts": attempts,
    }


def build_reduced_summary(
    case: MinCase,
    original_source: str,
    reduced_source: str,
    baseline_signature: str,
) -> dict[str, Any]:
    return {
        "case_id": case.case_id,
        "original_bytes": len(original_source.encode("utf-8")),
        "reduced_bytes": len(reduced_source.encode("utf-8")),
        "original_line_count": len(original_source.splitlines()),
        "reduced_line_count": len(reduced_source.splitlines()),
        "signature_sha256": baseline_signature,
    }


def build_summary_payload(
    *,
    generated_at_utc: str,
    manifest_path: Path,
    artifact_surface_path: Path,
    failure_root: Path,
    minimized_root: Path,
    artifact_surface: dict[str, Any],
    case_summaries: list[dict[str, Any]],
) -> dict[str, Any]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": generated_at_utc,
        "status": "PASS",
        "manifest_path": repo_rel(manifest_path),
        "artifact_surface_path": repo_rel(artifact_surface_path),
        "failure_root": repo_rel(failure_root),
        "minimized_root": repo_rel(minimized_root),
        "case_count": len(case_summaries),
        "artifact_surface_summary_reports": artifact_surface["summary_reports"],
        "case_summaries": case_summaries,
    }


def render_console_summary(summary_out: Path) -> str:
    return f"summary_path: {repo_rel(summary_out)}\nobjc3c-stress-minimization: PASS\n"


def emit_result(payload: dict[str, Any], summary_out: Path, contract_mode: bool) -> None:
    if contract_mode:
        sys.stdout.write(json_text(payload))
    else:
        sys.stdout.write(render_console_summary(summary_out))


__all__ = [
    "build_failure_summary",
    "build_invocation_payload",
    "build_reduced_summary",
    "build_reducer_plan",
    "build_summary_payload",
    "emit_result",
    "json_text",
    "render_console_summary",
    "write_json",
]
