from __future__ import annotations

import shutil
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import ROOT, repo_rel
from objc3c_tooling.subprocesses import python_script_command, run_capture

PUBLICATION_SUMMARY_CONTRACT_ID = "objc3c.distribution.credibility.public.summary.v1"
TRUST_REPORT_CONTRACT_ID = "objc3c.distribution.trust.report.v1"


@dataclass(frozen=True)
class DistributionTrustReportPaths:
    source_check: Path
    schema_check: Path
    dashboard_build: Path
    source_summary: Path
    schema_summary: Path
    dashboard_summary: Path
    public_summary: Path
    published_dashboard: Path
    published_report_json: Path
    published_report_markdown: Path

    @classmethod
    def for_root(cls, root: Path = ROOT) -> "DistributionTrustReportPaths":
        report_root = root / "tmp" / "reports" / "distribution-credibility"
        artifact_root = root / "tmp" / "artifacts" / "distribution-credibility"
        return cls(
            source_check=root / "scripts" / "check_distribution_credibility_source_surface.py",
            schema_check=root / "scripts" / "check_distribution_credibility_schema_surface.py",
            dashboard_build=root / "scripts" / "build_objc3c_distribution_credibility_dashboard.py",
            source_summary=report_root / "source-surface-summary.json",
            schema_summary=report_root / "schema-surface-summary.json",
            dashboard_summary=report_root / "dashboard-summary.json",
            public_summary=report_root / "publication-summary.json",
            published_dashboard=artifact_root / "dashboard" / "distribution-credibility-dashboard.json",
            published_report_json=artifact_root / "report" / "objc3c-distribution-trust-report.json",
            published_report_markdown=artifact_root / "report" / "objc3c-distribution-trust-report.md",
        )


def ensure_success(path: Path, script: Path) -> dict[str, Any]:
    if not path.is_file():
        result = run_capture(python_script_command(script))
        if result.returncode != 0:
            raise RuntimeError(f"failed to build required report via {repo_rel(script)}")
    payload = load_json(path)
    if payload.get("status") != "PASS":
        raise RuntimeError(f"required report did not pass: {repo_rel(path)}")
    return payload


def headline_for_state(trust_state: str) -> str:
    if trust_state == "ready":
        return "Objective-C 3 distribution trust signals are release-ready on the current live package surfaces."
    if trust_state == "degraded":
        return "Objective-C 3 distribution trust signals are publishable with caution on the current live package surfaces."
    return "Objective-C 3 distribution trust signals are blocked until release drill or recovery regressions are resolved."


def evidence_paths(
    *,
    paths: DistributionTrustReportPaths,
    dashboard: dict[str, Any],
) -> list[str]:
    upstream_reports = dashboard.get("upstream_reports", {})
    if not isinstance(upstream_reports, dict):
        raise RuntimeError("dashboard upstream_reports drifted")
    return [
        repo_rel(paths.source_summary),
        repo_rel(paths.schema_summary),
        repo_rel(paths.dashboard_summary),
        *[str(path) for path in upstream_reports.values()],
    ]


def trust_report_payload(
    *,
    paths: DistributionTrustReportPaths,
    dashboard: dict[str, Any],
    evidence: list[str],
    generated_at_utc: datetime,
) -> dict[str, Any]:
    trust_state = str(dashboard["trust_state"])
    operator_actions = dashboard.get("operator_actions")
    trust_signals = dashboard.get("trust_signals")
    required_drill_steps = dashboard.get("required_drill_steps")
    if trust_state not in {"ready", "degraded", "blocked"}:
        raise RuntimeError(f"dashboard trust state drifted: {trust_state}")
    if not isinstance(operator_actions, list) or not operator_actions:
        raise RuntimeError("dashboard operator actions drifted")
    if not isinstance(trust_signals, list) or len(trust_signals) < 4:
        raise RuntimeError("dashboard trust signals drifted")
    if not isinstance(required_drill_steps, list) or len(required_drill_steps) < 5:
        raise RuntimeError("dashboard release drill steps drifted")

    return {
        "contract_id": TRUST_REPORT_CONTRACT_ID,
        "generated_at_utc": generated_at_utc.isoformat(),
        "status": "PASS",
        "trust_state": trust_state,
        "headline": headline_for_state(trust_state),
        "release_version": dashboard["release_version"],
        "warning_count": dashboard["warning_count"],
        "dashboard_path": repo_rel(paths.published_dashboard),
        "evidence_paths": evidence,
        "trust_signals": trust_signals,
        "required_drill_steps": required_drill_steps,
        "operator_actions": operator_actions,
        "markdown_path": repo_rel(paths.published_report_markdown),
    }


def markdown_report(report: dict[str, Any]) -> str:
    lines = [
        "# Objective-C 3 Distribution Trust Report",
        "",
        f"- Trust state: `{report['trust_state']}`",
        f"- Release version: `{report['release_version']}`",
        f"- Compatibility warnings tracked: `{report['warning_count']}`",
        "",
        str(report["headline"]),
        "",
        "## Trust Signals",
        "",
    ]
    lines.extend(
        f"- `{signal['signal_id']}`: `{signal['status']}` from `{signal['source_path']}`"
        for signal in report["trust_signals"]
    )
    lines.extend(["", "## Release Drill", ""])
    lines.extend(f"- `{step}`" for step in report["required_drill_steps"])
    lines.extend(["", "## Operator Actions", ""])
    lines.extend(f"- {line}" for line in report["operator_actions"])
    lines.extend(["", "## Evidence", ""])
    lines.extend(f"- `{path}`" for path in report["evidence_paths"])
    return "\n".join(lines) + "\n"


def publish_distribution_trust_report(paths: DistributionTrustReportPaths) -> dict[str, Any]:
    ensure_success(paths.source_summary, paths.source_check)
    ensure_success(paths.schema_summary, paths.schema_check)
    dashboard = ensure_success(paths.dashboard_summary, paths.dashboard_build)

    generated_at_utc = datetime.now(timezone.utc)
    evidence = evidence_paths(paths=paths, dashboard=dashboard)
    report = trust_report_payload(paths=paths, dashboard=dashboard, evidence=evidence, generated_at_utc=generated_at_utc)

    paths.published_dashboard.parent.mkdir(parents=True, exist_ok=True)
    paths.published_report_json.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(paths.dashboard_summary, paths.published_dashboard)
    write_json_file(paths.published_report_json, report)
    paths.published_report_markdown.write_text(markdown_report(report), encoding="utf-8")

    summary = {
        "contract_id": PUBLICATION_SUMMARY_CONTRACT_ID,
        "generated_at_utc": generated_at_utc.isoformat(),
        "status": "PASS",
        "source_surface_summary_path": repo_rel(paths.source_summary),
        "schema_surface_summary_path": repo_rel(paths.schema_summary),
        "dashboard_summary_path": repo_rel(paths.dashboard_summary),
        "published_dashboard": repo_rel(paths.published_dashboard),
        "trust_report_json": repo_rel(paths.published_report_json),
        "trust_report_markdown": repo_rel(paths.published_report_markdown),
        "headline": report["headline"],
        "trust_state": report["trust_state"],
        "evidence_paths": evidence,
    }
    paths.public_summary.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(paths.public_summary, summary)
    return summary


def main() -> int:
    paths = DistributionTrustReportPaths.for_root()
    try:
        summary = publish_distribution_trust_report(paths)
    except RuntimeError as exc:
        print(f"objc3c-distribution-trust-report: FAIL\n- {exc}", file=sys.stderr)
        return 1

    print(f"summary_path: {repo_rel(paths.public_summary)}")
    print(f"published_dashboard: {summary['published_dashboard']}")
    print(f"published_report_json: {summary['trust_report_json']}")
    print(f"published_report_markdown: {summary['trust_report_markdown']}")
    print("objc3c-distribution-trust-report: OK")
    return 0
