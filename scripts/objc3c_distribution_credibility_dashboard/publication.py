from __future__ import annotations

from dataclasses import dataclass

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from objc3c_distribution_credibility_dashboard.model import DistributionCredibilityDashboardModel
from objc3c_distribution_credibility_dashboard.paths import DistributionCredibilityDashboardPaths
from objc3c_distribution_credibility_dashboard.rendering import dashboard_summary_payload


@dataclass(frozen=True)
class PublishedDistributionCredibilityDashboard:
    summary_path: str


def publish_dashboard_summary(
    *,
    paths: DistributionCredibilityDashboardPaths,
    model: DistributionCredibilityDashboardModel,
) -> PublishedDistributionCredibilityDashboard:
    paths.output_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(paths.output_path, dashboard_summary_payload(model))
    return PublishedDistributionCredibilityDashboard(summary_path=repo_rel(paths.output_path))
