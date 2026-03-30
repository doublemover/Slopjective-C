#!/usr/bin/env python3
"""Validate the checked-in objc3c packaging-channels source surface."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "source-surface-summary.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def resolve_repo_path(raw_path: str) -> Path:
    return ROOT / raw_path.replace("/", "\\")


def main() -> int:
    payload = load_json(SOURCE_SURFACE)
    required_keys = (
        "runbook",
        "channel_architecture",
        "supported_platforms",
        "installer_policy",
        "metadata_surface",
        "schema_surface",
    )
    for key in required_keys:
        raw_path = payload.get(key)
        if not isinstance(raw_path, str) or not raw_path:
            raise RuntimeError(f"source surface did not publish {key}")
        candidate = resolve_repo_path(raw_path)
        if not candidate.is_file():
            raise RuntimeError(f"missing source-surface path {raw_path}")

    checked_in_sources = payload.get("checked_in_sources")
    if not isinstance(checked_in_sources, list) or not checked_in_sources:
        raise RuntimeError("source surface did not publish checked_in_sources")
    for raw_path in checked_in_sources:
        if not isinstance(raw_path, str) or not raw_path:
            raise RuntimeError("checked_in_sources contained an invalid path")
        candidate = resolve_repo_path(raw_path)
        if not candidate.is_file():
            raise RuntimeError(f"missing checked-in packaging-channels source {raw_path}")

    output_roots = payload.get("machine_owned_output_roots")
    if not isinstance(output_roots, list) or not output_roots:
        raise RuntimeError("source surface did not publish machine_owned_output_roots")

    summary = {
        "contract_id": "objc3c.packaging.channels.source.surface.summary.v1",
        "status": "PASS",
        "source_surface": repo_rel(SOURCE_SURFACE),
        "checked_in_source_count": len(checked_in_sources),
        "machine_owned_output_roots": output_roots,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("packaging-channels-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
