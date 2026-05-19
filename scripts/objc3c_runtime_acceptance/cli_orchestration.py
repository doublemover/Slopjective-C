"""CLI orchestration for runtime acceptance."""

from __future__ import annotations

import json
import sys
from datetime import datetime

from objc3c_runtime_acceptance.cli_arguments import parse_args
from objc3c_runtime_acceptance.execution import run_case_factories
from objc3c_runtime_acceptance.native_binaries import resolve_native_binary_set
from objc3c_runtime_acceptance.progress_format import repo_display_path
from objc3c_runtime_acceptance.progress_state import RuntimeAcceptanceProgress
from objc3c_runtime_acceptance.progress_state import set_acceptance_progress
from objc3c_runtime_acceptance.report_assembly import assemble_runtime_acceptance_summary
from objc3c_runtime_acceptance.report_assembly import persist_runtime_acceptance_reports
from objc3c_runtime_acceptance.runtime_contract_paths import REPORT_ROOT, TMP_ROOT
from objc3c_runtime_acceptance.scenario_loading import load_runtime_acceptance_scenarios
from objc3c_runtime_acceptance.suite_catalog import RUNTIME_ACCEPTANCE_SUITE_CASES


def main(argv: list[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    run_dir = TMP_ROOT / run_id
    report_path = REPORT_ROOT / "summary.json"
    progress_path = REPORT_ROOT / "progress.json"
    run_dir.mkdir(parents=True, exist_ok=True)
    report_path.parent.mkdir(parents=True, exist_ok=True)

    native_binaries = resolve_native_binary_set()
    scenarios = load_runtime_acceptance_scenarios(
        clangxx=native_binaries.clangxx,
        run_dir=run_dir,
    )

    if args.list_suites:
        print(json.dumps(scenarios.available_suite_payload(), indent=2))
        return 0

    case_factories = scenarios.selected(
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

    summary = assemble_runtime_acceptance_summary(
        args=args,
        run_dir=run_dir,
        report_path=report_path,
        progress_path=progress_path,
        clangxx=native_binaries.clangxx,
        results=results,
        acceptance_progress=acceptance_progress,
        scenarios=scenarios,
        available_suites=RUNTIME_ACCEPTANCE_SUITE_CASES,
    )
    persist_runtime_acceptance_reports(
        progress_path=progress_path,
        report_path=report_path,
        acceptance_progress=acceptance_progress,
        summary=summary,
    )
    set_acceptance_progress(None)
    print(f"runtime-acceptance: PASS ({report_path})")
    return 0


__all__ = ["main"]
