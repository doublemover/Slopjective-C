from __future__ import annotations

import sys

from objc3c_performance_report.input_loading import load_performance_report_inputs
from objc3c_performance_report.model import build_performance_report_model
from objc3c_performance_report.paths import PerformanceReportPaths
from objc3c_performance_report.publication import publish_performance_report


def main() -> int:
    paths = PerformanceReportPaths.for_root()
    try:
        inputs = load_performance_report_inputs(paths)
        model = build_performance_report_model(paths, inputs.dashboard_summary)
        published = publish_performance_report(paths=paths, model=model)
    except RuntimeError as exc:
        print(f"objc3c-performance-report: FAIL\n- {exc}", file=sys.stderr)
        return 1

    print(f"summary_path: {published.summary_path}")
    print(f"published_dashboard: {published.dashboard_path}")
    print(f"published_badge: {published.badge_path}")
    print(f"published_report_markdown: {published.report_markdown_path}")
    print("objc3c-performance-report: OK")
    return 0
