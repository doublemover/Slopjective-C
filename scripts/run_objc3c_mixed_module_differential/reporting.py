"""Summary and artifact writing for mixed-module differential runs."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

from .commands import SURFACE_BUILDERS
from .config import SUMMARY_CONTRACT_ID
from .models import CaseResult, CaseSummary
from .tooling import repo_rel


def json_text(payload: dict[str, Any]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.write_text(json_text(payload), encoding="utf-8")


def build_surfaces(
    surface_contracts: dict[str, str],
    results: list[CaseResult],
) -> dict[str, dict[str, Any]]:
    surfaces: dict[str, dict[str, Any]] = {}
    for surface_key, contract_id in surface_contracts.items():
        surface = SURFACE_BUILDERS[surface_key](results)
        if surface.get("contract_id") != contract_id:
            raise RuntimeError(f"mixed-module differential surface {surface_key} drifted from contract {contract_id}")
        surfaces[surface_key] = surface
    return surfaces


def build_summary_payload(
    *,
    generated_at_utc: str,
    manifest_path: Path,
    run_root: Path,
    manifest: dict[str, Any],
    results: list[CaseResult],
    surfaces: dict[str, dict[str, Any]],
) -> dict[str, Any]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": generated_at_utc,
        "status": "PASS",
        "manifest_path": repo_rel(manifest_path),
        "run_root": repo_rel(run_root),
        "case_count": len(results),
        "surface_count": len(surfaces),
        "case_ids": manifest["case_ids"],
        "fixture_groups": manifest["fixture_groups"],
        "case_summaries": [CaseSummary.from_result(result).to_json() for result in results],
        "surfaces": surfaces,
    }


def render_console_summary(summary_out: Path) -> str:
    return f"summary_path: {repo_rel(summary_out)}\nobjc3c-mixed-module-differential: PASS\n"


def emit_result(payload: dict[str, Any], summary_out: Path, contract_mode: bool) -> None:
    if contract_mode:
        sys.stdout.write(json_text(payload))
    else:
        sys.stdout.write(render_console_summary(summary_out))


__all__ = [
    "build_summary_payload",
    "build_surfaces",
    "emit_result",
    "json_text",
    "render_console_summary",
    "write_json",
]
