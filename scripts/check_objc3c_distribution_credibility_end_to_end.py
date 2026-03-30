#!/usr/bin/env python3
"""Validate objc3c distribution-credibility artifacts end to end."""

from __future__ import annotations

import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
TRUST_REPORT_JSON = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "report" / "objc3c-distribution-trust-report.json"
TRUST_REPORT_MD = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "report" / "objc3c-distribution-trust-report.md"
DASHBOARD_JSON = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "dashboard" / "distribution-credibility-dashboard.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "end-to-end-summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    result = subprocess.run(list(command), cwd=ROOT, text=True, capture_output=True, check=False, env=env)
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    result = run_capture([sys.executable, str(RUNNER), "validate-distribution-credibility"])
    if result.returncode != 0:
        raise RuntimeError("validate-distribution-credibility failed")

    trust_report = load_json(TRUST_REPORT_JSON)
    dashboard = load_json(DASHBOARD_JSON)
    expect(TRUST_REPORT_MD.is_file(), "missing markdown trust report")
    expect(trust_report.get("status") == "PASS", "trust report JSON did not pass")
    expect(dashboard.get("status") == "PASS", "dashboard JSON did not pass")
    expect(trust_report.get("trust_state") in {"ready", "degraded", "blocked"}, "trust state drifted")
    evidence_paths = trust_report.get("evidence_paths", [])
    expect(isinstance(evidence_paths, list) and len(evidence_paths) >= 5, "evidence paths drifted")
    expect(
        any("release-operations" in str(path) for path in evidence_paths)
        and any("package-channels" in str(path) for path in evidence_paths),
        "trust report evidence no longer references release-operations and package-channels outputs",
    )

    summary = {
        "contract_id": "objc3c.distribution.credibility.end-to-end.summary.v1",
        "status": "PASS",
        "dashboard_json": repo_rel(DASHBOARD_JSON),
        "trust_report_json": repo_rel(TRUST_REPORT_JSON),
        "trust_report_markdown": repo_rel(TRUST_REPORT_MD),
        "trust_state": trust_report.get("trust_state"),
        "evidence_path_count": len(evidence_paths),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-distribution-credibility-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
