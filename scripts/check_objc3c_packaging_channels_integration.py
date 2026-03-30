#!/usr/bin/env python3
"""Validate the integrated objc3c packaging-channels workflow report."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "workflow_surface.json"
PUBLIC_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-packaging-channels.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "integration-summary.json"
EXPECTED_STEP_ORDER = [
    "validate-release-foundation",
    "check-packaging-channels-surface",
    "check-packaging-channels-schema-surface",
    "build-package-channels",
]


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def main() -> int:
    workflow_surface = load_json(WORKFLOW_SURFACE)
    report = load_json(PUBLIC_REPORT)
    if report.get("action") != workflow_surface.get("validate_action"):
        raise RuntimeError("public packaging-channels report drifted from workflow_surface validate_action")
    steps = report.get("steps")
    if not isinstance(steps, list):
        raise RuntimeError("public packaging-channels report did not publish steps")
    step_actions = [step.get("action") for step in steps if isinstance(step, dict)]
    if step_actions != EXPECTED_STEP_ORDER:
        raise RuntimeError(f"packaging-channels step order drifted: {step_actions!r}")

    summary = {
        "contract_id": "objc3c.packaging.channels.integration.summary.v1",
        "status": "PASS",
        "public_workflow_report": repo_rel(PUBLIC_REPORT),
        "step_order": step_actions,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-packaging-channels-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
