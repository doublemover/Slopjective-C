#!/usr/bin/env python3
"""Publish the live distribution-credibility report artifacts and summary."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
SOURCE_CHECK = ROOT / "scripts" / "check_distribution_credibility_source_surface.py"
SCHEMA_CHECK = ROOT / "scripts" / "check_distribution_credibility_schema_surface.py"
DASHBOARD_BUILD = ROOT / "scripts" / "build_objc3c_distribution_credibility_dashboard.py"
SOURCE_SUMMARY = ROOT / "tmp" / "reports" / "distribution-credibility" / "source-surface-summary.json"
SCHEMA_SUMMARY = ROOT / "tmp" / "reports" / "distribution-credibility" / "schema-surface-summary.json"
DASHBOARD_SUMMARY = ROOT / "tmp" / "reports" / "distribution-credibility" / "dashboard-summary.json"
PUBLIC_SUMMARY = ROOT / "tmp" / "reports" / "distribution-credibility" / "publication-summary.json"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "distribution-credibility"
PUBLISHED_DASHBOARD = ARTIFACT_ROOT / "dashboard" / "distribution-credibility-dashboard.json"
PUBLISHED_REPORT_JSON = ARTIFACT_ROOT / "report" / "objc3c-distribution-trust-report.json"
PUBLISHED_REPORT_MD = ARTIFACT_ROOT / "report" / "objc3c-distribution-trust-report.md"
SUMMARY_CONTRACT_ID = "objc3c.distribution.credibility.public.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(list(command), cwd=ROOT, check=False, text=True, capture_output=True)
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def ensure_success(path: Path, script: Path) -> dict[str, Any]:
    if not path.is_file():
        result = run_capture([sys.executable, str(script)])
        if result.returncode != 0:
            raise RuntimeError(f"failed to build required report via {repo_rel(script)}")
    payload = load_json(path)
    if payload.get("status") != "PASS":
        raise RuntimeError(f"required report did not pass: {repo_rel(path)}")
    return payload


def main() -> int:
    try:
        source_summary = ensure_success(SOURCE_SUMMARY, SOURCE_CHECK)
        schema_summary = ensure_success(SCHEMA_SUMMARY, SCHEMA_CHECK)
        dashboard = ensure_success(DASHBOARD_SUMMARY, DASHBOARD_BUILD)
    except RuntimeError as exc:
        print(f"objc3c-distribution-trust-report: FAIL\n- {exc}", file=sys.stderr)
        return 1

    trust_state = str(dashboard["trust_state"])
    if trust_state == "ready":
        headline = "Objective-C 3 distribution trust signals are release-ready on the current live package surfaces."
    elif trust_state == "degraded":
        headline = "Objective-C 3 distribution trust signals are publishable with caution on the current live package surfaces."
    else:
        headline = "Objective-C 3 distribution trust signals are blocked until release drill or recovery regressions are resolved."

    operator_actions = [
        "run the packaged install and rollback drill before widening release claims",
        "keep the update-manifest and compatibility publication tied to the same shipped artifacts",
        "retain the release-evidence index alongside the published release payload",
    ]
    evidence_paths = [
        repo_rel(SOURCE_SUMMARY),
        repo_rel(SCHEMA_SUMMARY),
        repo_rel(DASHBOARD_SUMMARY),
        *dashboard["upstream_reports"].values(),
    ]

    PUBLISHED_DASHBOARD.parent.mkdir(parents=True, exist_ok=True)
    PUBLISHED_REPORT_JSON.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(DASHBOARD_SUMMARY, PUBLISHED_DASHBOARD)

    report_payload = {
        "contract_id": "objc3c.distribution.trust.report.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "trust_state": trust_state,
        "headline": headline,
        "release_version": dashboard["release_version"],
        "warning_count": dashboard["warning_count"],
        "evidence_paths": evidence_paths,
        "operator_actions": operator_actions,
        "markdown_path": repo_rel(PUBLISHED_REPORT_MD),
    }
    PUBLISHED_REPORT_JSON.write_text(json.dumps(report_payload, indent=2) + "\n", encoding="utf-8")

    markdown_lines = [
        "# Objective-C 3 Distribution Trust Report",
        "",
        f"- Trust state: `{trust_state}`",
        f"- Release version: `{dashboard['release_version']}`",
        f"- Compatibility warnings tracked: `{dashboard['warning_count']}`",
        "",
        headline,
        "",
        "## Trust Signals",
        "",
    ]
    markdown_lines.extend(
        f"- `{signal['signal_id']}`: `{signal['status']}` from `{signal['source_path']}`"
        for signal in dashboard["trust_signals"]
    )
    markdown_lines.extend(["", "## Operator Actions", ""])
    markdown_lines.extend(f"- {line}" for line in operator_actions)
    markdown_lines.extend(["", "## Evidence", ""])
    markdown_lines.extend(f"- `{path}`" for path in evidence_paths)
    PUBLISHED_REPORT_MD.write_text("\n".join(markdown_lines) + "\n", encoding="utf-8")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "source_surface_summary_path": repo_rel(SOURCE_SUMMARY),
        "schema_surface_summary_path": repo_rel(SCHEMA_SUMMARY),
        "dashboard_summary_path": repo_rel(DASHBOARD_SUMMARY),
        "trust_report_json": repo_rel(PUBLISHED_REPORT_JSON),
        "trust_report_markdown": repo_rel(PUBLISHED_REPORT_MD),
        "headline": headline,
        "trust_state": trust_state,
        "evidence_paths": evidence_paths,
    }
    PUBLIC_SUMMARY.parent.mkdir(parents=True, exist_ok=True)
    PUBLIC_SUMMARY.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(PUBLIC_SUMMARY)}")
    print(f"published_dashboard: {repo_rel(PUBLISHED_DASHBOARD)}")
    print(f"published_report_json: {repo_rel(PUBLISHED_REPORT_JSON)}")
    print(f"published_report_markdown: {repo_rel(PUBLISHED_REPORT_MD)}")
    print("objc3c-distribution-trust-report: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
