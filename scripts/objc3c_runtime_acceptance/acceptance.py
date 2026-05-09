"""CLI orchestration for runtime acceptance."""

from __future__ import annotations

import argparse
import json
import sys
from datetime import datetime

from objc3c_runtime_acceptance.case_catalog import build_case_factories
from objc3c_runtime_acceptance.case_catalog import load_runtime_acceptance_domains
from objc3c_runtime_acceptance.execution import filter_case_factories
from objc3c_runtime_acceptance.execution import run_case_factories
from objc3c_runtime_acceptance.native_build import ensure_native_binaries
from objc3c_runtime_acceptance.native_build import find_clangxx
from objc3c_runtime_acceptance.progress import RuntimeAcceptanceProgress
from objc3c_runtime_acceptance.progress import repo_display_path
from objc3c_runtime_acceptance.progress import set_acceptance_progress
from objc3c_runtime_acceptance.reports import write_json_report
from objc3c_runtime_acceptance.runtime_contracts import REPORT_ROOT, TMP_ROOT
from objc3c_runtime_acceptance.summary import build_runtime_acceptance_summary
from objc3c_runtime_acceptance.suite_catalog import RUNTIME_ACCEPTANCE_SUITE_CASES
from objc3c_runtime_acceptance.suite_catalog import available_suite_payload


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run ObjC3 runtime acceptance suites."
    )
    parser.add_argument(
        "--suite",
        choices=sorted(RUNTIME_ACCEPTANCE_SUITE_CASES),
        default="full",
        help="named runtime acceptance suite to run",
    )
    parser.add_argument(
        "--case",
        action="append",
        default=[],
        dest="cases",
        help="run one case label; may be repeated and overrides --suite",
    )
    parser.add_argument(
        "--list-suites",
        action="store_true",
        help="print suite names and case labels without running acceptance",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    run_dir = TMP_ROOT / run_id
    report_path = REPORT_ROOT / "summary.json"
    progress_path = REPORT_ROOT / "progress.json"
    run_dir.mkdir(parents=True, exist_ok=True)
    report_path.parent.mkdir(parents=True, exist_ok=True)

    ensure_native_binaries()
    clangxx = find_clangxx()
    domains = load_runtime_acceptance_domains()
    case_factories = build_case_factories(
        domains,
        clangxx=clangxx,
        run_dir=run_dir,
    )

    if args.list_suites:
        print(
            json.dumps(
                available_suite_payload(case_factories),
                indent=2,
            )
        )
        return 0

    case_factories = filter_case_factories(
        case_factories,
        selected_suite=args.suite,
        selected_cases=list(args.cases),
    )

    acceptance_progress = RuntimeAcceptanceProgress(
        run_id=run_id,
        run_dir=run_dir,
        progress_path=progress_path,
        total_cases=len(case_factories),
    )
    set_acceptance_progress(acceptance_progress)
    print(
        f"runtime-acceptance-progress-log: {repo_display_path(progress_path)}",
        flush=True,
    )
    results = run_case_factories(
        case_factories,
        acceptance_progress=acceptance_progress,
    )

    summary = build_runtime_acceptance_summary(
        args=args,
        run_dir=run_dir,
        report_path=report_path,
        progress_path=progress_path,
        clangxx=clangxx,
        results=results,
        acceptance_progress=acceptance_progress,
        domains=domains,
        available_suites=RUNTIME_ACCEPTANCE_SUITE_CASES,
    )
    write_json_report(progress_path, acceptance_progress.final_summary())
    write_json_report(report_path, summary)
    set_acceptance_progress(None)
    print(f"runtime-acceptance: PASS ({report_path})")
    return 0


__all__ = [
    "RUNTIME_ACCEPTANCE_SUITE_CASES",
    "filter_case_factories",
    "main",
    "parse_args",
]
