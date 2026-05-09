from __future__ import annotations

import sys

from objc3c_distribution_credibility_dashboard.input_loading import load_dashboard_inputs
from objc3c_distribution_credibility_dashboard.model import build_dashboard_model
from objc3c_distribution_credibility_dashboard.paths import DistributionCredibilityDashboardPaths
from objc3c_distribution_credibility_dashboard.publication import publish_dashboard_summary


def main() -> int:
    paths = DistributionCredibilityDashboardPaths.for_root()
    try:
        inputs = load_dashboard_inputs(paths)
        model = build_dashboard_model(paths, inputs)
        published = publish_dashboard_summary(paths=paths, model=model)
    except RuntimeError as exc:
        print(f"objc3c-distribution-credibility-dashboard: FAIL\n- {exc}", file=sys.stderr)
        return 1

    print(f"summary_path: {published.summary_path}")
    print("objc3c-distribution-credibility-dashboard: OK")
    return 0
