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
    policy_paths: dict[str, str]
    upstream_reports: dict[str, str]
    owner_split: dict[str, list[str]]
    publication_contracts: dict[str, str]


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


def build_upstream_reports(dashboard: dict[str, Any]) -> dict[str, str]:
    return {
        str(key): str(value)
        for key, value in require_dashboard_upstream_reports(dashboard).items()
    }


def build_policy_paths(dashboard: dict[str, Any]) -> dict[str, str]:
    path_fields = {
        "budget_model": "budget_model_path",
        "claim_policy": "claim_policy_path",
        "breach_triage_policy": "breach_triage_policy_path",
        "lab_policy": "lab_policy_path",
        "source_surface": "source_surface_path",
        "workflow_surface": "workflow_surface_path",
    }
    return {
        key: str(dashboard[field_name])
        for key, field_name in path_fields.items()
        if isinstance(dashboard.get(field_name), str)
    }


def build_publication_contracts(dashboard: dict[str, Any]) -> dict[str, str]:
    contracts = {
        "dashboard_summary": str(dashboard["contract_id"]),
        "public_summary": SUMMARY_CONTRACT_ID,
    }
    policy_contracts = dashboard.get("policy_contracts", {})
    if isinstance(policy_contracts, dict):
        contracts.update({f"policy.{key}": str(value) for key, value in policy_contracts.items()})
    upstream_contracts = dashboard.get("upstream_report_contracts", {})
    if isinstance(upstream_contracts, dict):
        contracts.update({f"upstream.{key}": str(value) for key, value in upstream_contracts.items()})
    return contracts


def build_owner_split(dashboard: dict[str, Any]) -> dict[str, list[str]]:
    owner_split = dashboard.get("owner_split", {})
    if not isinstance(owner_split, dict):
        return {}
    normalized: dict[str, list[str]] = {}
    for owner_name, paths in owner_split.items():
        if isinstance(paths, list):
            normalized[str(owner_name)] = [str(path) for path in paths]
    return normalized


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
        policy_paths=build_policy_paths(dashboard),
        upstream_reports=build_upstream_reports(dashboard),
        owner_split=build_owner_split(dashboard),
        publication_contracts=build_publication_contracts(dashboard),
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
        "policy_paths": model.policy_paths,
        "upstream_reports": model.upstream_reports,
        "owner_split": model.owner_split,
        "publication_contracts": model.publication_contracts,
    }
