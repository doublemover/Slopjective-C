from __future__ import annotations

import json
import shutil
from dataclasses import dataclass

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from objc3c_performance_report.model import (
    PerformanceReportModel,
    badge_payload,
    public_summary_payload,
)
from objc3c_performance_report.paths import PerformanceReportPaths
from objc3c_performance_report.rendering import render_markdown_report


@dataclass(frozen=True)
class PublishedPerformanceReport:
    summary_path: str
    dashboard_path: str
    badge_path: str
    report_markdown_path: str


def publish_performance_report(
    *,
    paths: PerformanceReportPaths,
    model: PerformanceReportModel,
) -> PublishedPerformanceReport:
    paths.published_dashboard.parent.mkdir(parents=True, exist_ok=True)
    paths.published_badge.parent.mkdir(parents=True, exist_ok=True)
    paths.published_report_markdown.parent.mkdir(parents=True, exist_ok=True)

    shutil.copyfile(paths.dashboard_summary, paths.published_dashboard)
    paths.published_badge.write_text(
        json.dumps(badge_payload(model), indent=2) + "\n",
        encoding="utf-8",
    )
    paths.published_report_markdown.write_text(render_markdown_report(model), encoding="utf-8")

    paths.public_summary.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(paths.public_summary, public_summary_payload(paths=paths, model=model))

    return PublishedPerformanceReport(
        summary_path=repo_rel(paths.public_summary),
        dashboard_path=repo_rel(paths.published_dashboard),
        badge_path=repo_rel(paths.published_badge),
        report_markdown_path=repo_rel(paths.published_report_markdown),
    )
