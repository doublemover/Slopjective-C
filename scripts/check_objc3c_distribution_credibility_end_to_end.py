#!/usr/bin/env python3
"""Validate objc3c distribution-credibility artifacts end to end."""

from __future__ import annotations

import json
import os
import re
import subprocess
import sys
from hashlib import sha256
from pathlib import Path
from typing import Any, Sequence

SCRIPT_DIR = Path(__file__).resolve().parent
ROOT = SCRIPT_DIR.parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import run_capture


TRUST_REPORT_JSON = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "report" / "objc3c-distribution-trust-report.json"
TRUST_REPORT_MD = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "report" / "objc3c-distribution-trust-report.md"
DASHBOARD_JSON = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "dashboard" / "distribution-credibility-dashboard.json"
ARTIFACT_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "artifact_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "end-to-end-summary.json"
SHA256_RE = re.compile(r"^[a-f0-9]{64}$")






def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_path(raw_path: str) -> Path:
    path = ROOT / raw_path
    resolved = path.resolve()
    root_resolved = ROOT.resolve()
    try:
        resolved.relative_to(root_resolved)
    except ValueError as exc:
        raise RuntimeError(f"evidence artifact path is outside repo: {raw_path}") from exc
    return resolved


def validate_evidence_artifact(artifact: Any) -> str:
    expect(isinstance(artifact, dict), "evidence artifact entry must be an object")
    raw_path = artifact.get("path")
    expect(isinstance(raw_path, str) and raw_path, "evidence artifact path drifted")
    expect(artifact.get("digest_algorithm") == "sha256", f"evidence artifact digest algorithm drifted for {raw_path}")
    expected_sha256 = artifact.get("sha256")
    expect(
        isinstance(expected_sha256, str) and SHA256_RE.fullmatch(expected_sha256) is not None,
        f"evidence artifact sha256 drifted for {raw_path}",
    )

    path = repo_path(raw_path)
    expect(path.is_file(), f"evidence artifact path is missing: {raw_path}")
    actual_sha256 = sha256(path.read_bytes()).hexdigest()
    expect(actual_sha256 == expected_sha256, f"evidence artifact digest mismatch for {raw_path}")

    payload = load_json(path)
    contract_id = payload.get("contract_id")
    status = payload.get("status")
    if isinstance(contract_id, str) and contract_id:
        expect(artifact.get("contract_id") == contract_id, f"evidence artifact contract_id drifted for {raw_path}")
    if isinstance(status, str) and status:
        expect(artifact.get("status") == status, f"evidence artifact status drifted for {raw_path}")
    return raw_path


def main() -> int:
    result = run_capture(public_workflow_command("validate-distribution-credibility"))
    if result.returncode != 0:
        raise RuntimeError("validate-distribution-credibility failed")

    trust_report = load_json(TRUST_REPORT_JSON)
    dashboard = load_json(DASHBOARD_JSON)
    artifact_surface = load_json(ARTIFACT_SURFACE)
    expect(TRUST_REPORT_MD.is_file(), "missing markdown trust report")
    expect(artifact_surface.get("trust_report_json") == repo_rel(TRUST_REPORT_JSON), "trust report JSON path drifted")
    expect(artifact_surface.get("trust_report_markdown") == repo_rel(TRUST_REPORT_MD), "trust report markdown path drifted")
    expect(artifact_surface.get("dashboard_artifact") == repo_rel(DASHBOARD_JSON), "dashboard artifact path drifted")
    expect(trust_report.get("status") == "PASS", "trust report JSON did not pass")
    expect(dashboard.get("status") == "PASS", "dashboard JSON did not pass")
    expect(trust_report.get("dashboard_path") == repo_rel(DASHBOARD_JSON), "trust report dashboard path drifted")
    expect(trust_report.get("trust_state") in {"ready", "degraded", "blocked"}, "trust state drifted")
    expect(trust_report.get("trust_signals") == dashboard.get("trust_signals"), "trust signal publication drifted")
    expect(trust_report.get("required_drill_steps") == dashboard.get("required_drill_steps"), "release drill publication drifted")
    expect(trust_report.get("operator_actions") == dashboard.get("operator_actions"), "operator action publication drifted")
    evidence_paths = trust_report.get("evidence_paths", [])
    expect(isinstance(evidence_paths, list) and len(evidence_paths) >= 5, "evidence paths drifted")
    evidence_artifacts = trust_report.get("evidence_artifacts", [])
    expect(isinstance(evidence_artifacts, list) and len(evidence_artifacts) >= len(evidence_paths), "evidence artifacts drifted")
    evidence_artifact_paths = [validate_evidence_artifact(artifact) for artifact in evidence_artifacts]
    expect(set(evidence_paths).issubset(set(evidence_artifact_paths)), "evidence artifact paths no longer cover evidence paths")
    expect(
        any("release-operations" in str(path) for path in evidence_artifact_paths)
        and any("package-channels" in str(path) for path in evidence_artifact_paths),
        "trust report evidence artifacts no longer reference release-operations and package-channels outputs",
    )

    summary = {
        "contract_id": "objc3c.distribution.credibility.end-to-end.summary.v1",
        "status": "PASS",
        "dashboard_json": repo_rel(DASHBOARD_JSON),
        "trust_report_json": repo_rel(TRUST_REPORT_JSON),
        "trust_report_markdown": repo_rel(TRUST_REPORT_MD),
        "trust_state": trust_report.get("trust_state"),
        "evidence_path_count": len(evidence_paths),
        "evidence_artifact_count": len(evidence_artifacts),
        "trust_signal_count": len(trust_report.get("trust_signals", [])),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-distribution-credibility-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
