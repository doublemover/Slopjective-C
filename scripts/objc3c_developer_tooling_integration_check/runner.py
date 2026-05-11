"""Developer-tooling integration validation entrypoint."""

from __future__ import annotations

import sys

from .assertions import expect
from .report_assertions import assert_loaded_reports
from .reports import assert_report_paths_exist, load_reports
from .steps import run_developer_tooling_steps
from .summary import write_summary


def main() -> int:
    steps = run_developer_tooling_steps()
    failures: list[str] = []
    for step in steps:
        expect(step["exit_code"] == 0, f"{step['name']} failed", failures)

    assert_report_paths_exist(failures)
    reports = load_reports()
    assert_loaded_reports(reports, failures)
    write_summary(steps, failures)

    if failures:
        print("developer-tooling-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("developer-tooling-integration: PASS")
    return 0
