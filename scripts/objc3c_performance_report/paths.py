from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from objc3c_tooling.paths import ROOT


SUMMARY_CONTRACT_ID = "objc3c.performance.governance.public.summary.v1"


@dataclass(frozen=True)
class RequiredReportPath:
    summary_path: Path
    builder_script: Path


@dataclass(frozen=True)
class PerformanceReportPaths:
    root: Path
    source_check: Path
    schema_check: Path
    dashboard_build: Path
    source_summary: Path
    schema_summary: Path
    dashboard_summary: Path
    public_summary: Path
    artifact_root: Path
    published_dashboard: Path
    published_badge: Path
    published_report_markdown: Path

    @classmethod
    def for_root(cls, root: Path = ROOT) -> "PerformanceReportPaths":
        report_root = root / "tmp" / "reports" / "performance-governance"
        artifact_root = root / "tmp" / "artifacts" / "performance-governance"
        return cls(
            root=root,
            source_check=root / "scripts" / "check_performance_governance_source_surface.py",
            schema_check=root / "scripts" / "check_performance_governance_schema_surface.py",
            dashboard_build=root / "scripts" / "build_objc3c_performance_dashboard.py",
            source_summary=report_root / "source-surface-summary.json",
            schema_summary=report_root / "schema-surface-summary.json",
            dashboard_summary=report_root / "dashboard-summary.json",
            public_summary=report_root / "public-summary.json",
            artifact_root=artifact_root,
            published_dashboard=artifact_root / "dashboard" / "performance-dashboard.json",
            published_badge=artifact_root / "badge" / "performance-status-badge.json",
            published_report_markdown=artifact_root / "report" / "performance-report.md",
        )

    def required_reports(self) -> tuple[RequiredReportPath, RequiredReportPath, RequiredReportPath]:
        return (
            RequiredReportPath(self.source_summary, self.source_check),
            RequiredReportPath(self.schema_summary, self.schema_check),
            RequiredReportPath(self.dashboard_summary, self.dashboard_build),
        )
