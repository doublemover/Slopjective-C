from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
from typing import Any

from objc3c_tooling.paths import repo_rel

from objc3c_performance_report.paths import PerformanceReportPaths, SUMMARY_CONTRACT_ID
from objc3c_performance_report.validation import require_dashboard_upstream_reports


@dataclass(frozen=True)
class PerformanceReportModel:
    release_status: str
    claim_ready: bool
    blocking_breach_count: int
    warning_breach_count: int
    headline: str
    summary_lines: list[str]
    evidence_paths: list[str]


def release_headline(release_status: str) -> str:
    if release_status == "release-ready":
        return "Objective-C 3 performance evidence is release-ready on the current checked-in lab profile."
    if release_status == "caution":
        return "Objective-C 3 performance evidence is publishable with caution on the current checked-in lab profile."
    return "Objective-C 3 performance evidence is blocked until regressions or environment drift are resolved."


def build_summary_lines(
    *,
    release_status: str,
    claim_ready: bool,
    blocking_breach_count: int,
    warning_breach_count: int,
    dashboard: dict[str, Any],
) -> list[str]:
    environment_drift = dashboard["environment_drift"]
    drift_issues = environment_drift.get("issues", [])
    drift_summary = "; ".join(str(issue) for issue in drift_issues) if drift_issues else "none"
    return [
        f"Release status is {release_status} with {blocking_breach_count} blocking breaches and {warning_breach_count} warning-or-caution breaches.",
        f"Claim ready is {str(claim_ready).lower()}.",
        f"Environment drift issues: {drift_summary}.",
    ]


def build_evidence_paths(paths: PerformanceReportPaths, dashboard: dict[str, Any]) -> list[str]:
    upstream_reports = require_dashboard_upstream_reports(dashboard)
    return [
        repo_rel(paths.source_summary),
        repo_rel(paths.schema_summary),
        repo_rel(paths.dashboard_summary),
        *(str(path) for path in upstream_reports.values()),
    ]


def build_performance_report_model(paths: PerformanceReportPaths, dashboard: dict[str, Any]) -> PerformanceReportModel:
    release_status = str(dashboard["release_status"])
    claim_ready = bool(dashboard["claim_ready"])
    blocking_breach_count = int(dashboard["blocking_breach_count"])
    warning_breach_count = int(dashboard["warning_breach_count"])
    headline = release_headline(release_status)
    return PerformanceReportModel(
        release_status=release_status,
        claim_ready=claim_ready,
        blocking_breach_count=blocking_breach_count,
        warning_breach_count=warning_breach_count,
        headline=headline,
        summary_lines=build_summary_lines(
            release_status=release_status,
            claim_ready=claim_ready,
            blocking_breach_count=blocking_breach_count,
            warning_breach_count=warning_breach_count,
            dashboard=dashboard,
        ),
        evidence_paths=build_evidence_paths(paths, dashboard),
    )


def badge_payload(model: PerformanceReportModel) -> dict[str, Any]:
    return {
        "release_status": model.release_status,
        "claim_ready": model.claim_ready,
        "blocking_breach_count": model.blocking_breach_count,
        "warning_breach_count": model.warning_breach_count,
    }


def public_summary_payload(
    *,
    paths: PerformanceReportPaths,
    model: PerformanceReportModel,
    generated_at_utc: datetime | None = None,
) -> dict[str, Any]:
    generated_at = generated_at_utc or datetime.now(timezone.utc)
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": generated_at.isoformat(),
        "status": "PASS",
        "source_surface_summary_path": repo_rel(paths.source_summary),
        "schema_surface_summary_path": repo_rel(paths.schema_summary),
        "dashboard_summary_path": repo_rel(paths.dashboard_summary),
        "report_markdown_path": repo_rel(paths.published_report_markdown),
        "badge_path": repo_rel(paths.published_badge),
        "release_status": model.release_status,
        "claim_ready": model.claim_ready,
        "headline": model.headline,
        "summary_lines": model.summary_lines,
        "evidence_paths": model.evidence_paths,
    }
