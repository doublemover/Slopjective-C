#!/usr/bin/env python3
"""Apply the safe portion of the Objective-C 2, Swift, and C++ migration rewrite plan."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

from objc3c_tooling.paths import repo_rel, resolve_repo_path_inside
from objc3c_migration_analyzer import (
    analyze_migration_input,
    apply_rewrite_plan,
    build_rewrite_workflow_report,
    default_analysis_report_path,
    default_rewrite_report_path,
    write_analysis_report,
    write_rewrite_report,
)
from objc3c_migration_analyzer.analyzer import ARTIFACT_ROOT, write_rewritten_source


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", help="Migration input JSON contract")
    parser.add_argument("--output", help="Optional rewritten source output path")
    parser.add_argument("--analysis-report-out", help="Optional analysis report path")
    parser.add_argument("--report-out", help="Optional rewrite report path")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    input_path = resolve_repo_path_inside(args.input)
    analysis_report_path = (
        resolve_repo_path_inside(args.analysis_report_out)
        if args.analysis_report_out
        else default_analysis_report_path(input_path)
    )
    rewrite_report_path = (
        resolve_repo_path_inside(args.report_out)
        if args.report_out
        else default_rewrite_report_path(input_path)
    )
    analysis = analyze_migration_input(input_path)
    write_analysis_report(analysis, analysis_report_path)

    rewritten_text: str | None = None
    output_path: Path | None = None
    if analysis.ok:
        output_path = (
            resolve_repo_path_inside(args.output)
            if args.output
            else ARTIFACT_ROOT
            / analysis_report_path.parent.name
            / "rewritten.objc3"
        )
        rewritten_text = apply_rewrite_plan(analysis.source_text, analysis.payload["rewrite_plan"])
        write_rewritten_source(output_path, rewritten_text)

    report = build_rewrite_workflow_report(
        analysis,
        input_path=input_path,
        rewritten_output_path=output_path,
        rewritten_text=rewritten_text,
        analysis_report_path=analysis_report_path,
    )
    write_rewrite_report(report, rewrite_report_path)
    print(f"analysis_report_path: {repo_rel(analysis_report_path)}")
    print(f"rewrite_report_path: {repo_rel(rewrite_report_path)}")
    if output_path is not None:
        print(f"rewritten_output_path: {repo_rel(output_path)}")
    print(json.dumps(report, indent=2))
    return 0 if report["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
