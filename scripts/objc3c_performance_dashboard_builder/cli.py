from __future__ import annotations

import sys

from objc3c_performance_dashboard_builder.input_loading import load_dashboard_inputs
from objc3c_performance_dashboard_builder.rendering import publish_dashboard_summary
from objc3c_performance_dashboard_builder.summary import build_dashboard_summary


def main() -> int:
    try:
        inputs = load_dashboard_inputs()
    except RuntimeError as exc:
        print(f"objc3c-performance-dashboard: FAIL\n- {exc}", file=sys.stderr)
        return 1

    payload = build_dashboard_summary(inputs)
    summary_path = publish_dashboard_summary(payload)
    print(f"summary_path: {summary_path}")
    print("objc3c-performance-dashboard: OK")
    return 0
