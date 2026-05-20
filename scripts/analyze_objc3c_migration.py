#!/usr/bin/env python3
"""Analyze Objective-C 2, Swift, and C++ migration inputs."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import sys

from objc3c_tooling.paths import repo_rel, resolve_repo_path_inside
from objc3c_migration_analyzer import (
    analyze_migration_input,
    default_analysis_report_path,
    write_analysis_report,
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", help="Migration input JSON contract")
    parser.add_argument("--report-out", help="Optional analysis report path")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    input_path = resolve_repo_path_inside(args.input)
    report_path = (
        resolve_repo_path_inside(args.report_out)
        if args.report_out
        else default_analysis_report_path(input_path)
    )
    report = analyze_migration_input(input_path)
    write_analysis_report(report, report_path)
    print(f"analysis_report_path: {repo_rel(report_path)}")
    print(json.dumps(report.payload, indent=2))
    return 0 if report.ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
