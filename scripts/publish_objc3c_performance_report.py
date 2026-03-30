#!/usr/bin/env python3
"""Publish the live performance governance report artifacts and summary."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
SOURCE_CHECK = ROOT / "scripts" / "check_performance_governance_source_surface.py"
SCHEMA_CHECK = ROOT / "scripts" / "check_performance_governance_schema_surface.py"
DASHBOARD_BUILD = ROOT / "scripts" / "build_objc3c_performance_dashboard.py"
SOURCE_SUMMARY = ROOT / "tmp" / "reports" / "performance-governance" / "source-surface-summary.json"
SCHEMA_SUMMARY = ROOT / "tmp" / "reports" / "performance-governance" / "schema-surface-summary.json"
DASHBOARD_SUMMARY = ROOT / "tmp" / "reports" / "performance-governance" / "dashboard-summary.json"
PUBLIC_SUMMARY = ROOT / "tmp" / "reports" / "performance-governance" / "public-summary.json"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "performance-governance"
PUBLISHED_DASHBOARD = ARTIFACT_ROOT / "dashboard" / "performance-dashboard.json"
PUBLISHED_BADGE = ARTIFACT_ROOT / "badge" / "performance-status-badge.json"
PUBLISHED_REPORT_MD = ARTIFACT_ROOT / "report" / "performance-report.md"
SUMMARY_CONTRACT_ID = "objc3c.performance.governance.public.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
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
        print(f"objc3c-performance-report: FAIL\n- {exc}", file=sys.stderr)
        return 1

    release_status = str(dashboard["release_status"])
    claim_ready = bool(dashboard["claim_ready"])
    blocking_breach_count = int(dashboard["blocking_breach_count"])
    warning_breach_count = int(dashboard["warning_breach_count"])
    if release_status == "release-ready":
        headline = "Objective-C 3 performance evidence is release-ready on the current checked-in lab profile."
    elif release_status == "caution":
        headline = "Objective-C 3 performance evidence is publishable with caution on the current checked-in lab profile."
    else:
        headline = "Objective-C 3 performance evidence is blocked until regressions or environment drift are resolved."

    summary_lines = [
        f"Release status is {release_status} with {blocking_breach_count} blocking breaches and {warning_breach_count} warning-or-caution breaches.",
        f"Claim ready is {str(claim_ready).lower()}.",
        (
            "Environment drift issues: "
            + (
                "; ".join(str(issue) for issue in dashboard["environment_drift"].get("issues", []))
                if dashboard["environment_drift"].get("issues")
                else "none"
            )
            + "."
        ),
    ]

    evidence_paths = [
        repo_rel(SOURCE_SUMMARY),
        repo_rel(SCHEMA_SUMMARY),
        repo_rel(DASHBOARD_SUMMARY),
        *dashboard["upstream_reports"].values(),
    ]

    PUBLISHED_DASHBOARD.parent.mkdir(parents=True, exist_ok=True)
    PUBLISHED_BADGE.parent.mkdir(parents=True, exist_ok=True)
    PUBLISHED_REPORT_MD.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(DASHBOARD_SUMMARY, PUBLISHED_DASHBOARD)
    PUBLISHED_BADGE.write_text(
        json.dumps(
            {
                "release_status": release_status,
                "claim_ready": claim_ready,
                "blocking_breach_count": blocking_breach_count,
                "warning_breach_count": warning_breach_count,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    markdown_lines = [
        "# Objective-C 3 Performance Report",
        "",
        f"- Release status: `{release_status}`",
        f"- Claim ready: `{str(claim_ready).lower()}`",
        f"- Blocking breaches: `{blocking_breach_count}`",
        f"- Warning or caution breaches: `{warning_breach_count}`",
        "",
        headline,
        "",
        "## Summary",
        "",
    ]
    markdown_lines.extend(f"- {line}" for line in summary_lines)
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
        "report_markdown_path": repo_rel(PUBLISHED_REPORT_MD),
        "badge_path": repo_rel(PUBLISHED_BADGE),
        "release_status": release_status,
        "claim_ready": claim_ready,
        "headline": headline,
        "summary_lines": summary_lines,
        "evidence_paths": evidence_paths,
    }
    PUBLIC_SUMMARY.parent.mkdir(parents=True, exist_ok=True)
    PUBLIC_SUMMARY.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(PUBLIC_SUMMARY)}")
    print(f"published_dashboard: {repo_rel(PUBLISHED_DASHBOARD)}")
    print(f"published_badge: {repo_rel(PUBLISHED_BADGE)}")
    print(f"published_report_markdown: {repo_rel(PUBLISHED_REPORT_MD)}")
    print("objc3c-performance-report: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
