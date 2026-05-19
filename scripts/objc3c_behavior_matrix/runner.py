"""Top-level behavior matrix orchestration."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.behavior_fixtures import BehaviorFixture, load_behavior_fixtures
from objc3c_tooling.paths import ROOT, repo_rel

from .case_execution import execute_fixture
from .cli import parse_args
from .errors import BehaviorMatrixFailure
from .fixture_loading import select_fixtures
from .native_execution import ensure_native_binaries
from .reporting import build_summary_payload, render_report


def resolve_report_path(report_out: Path) -> Path:
    return report_out if report_out.is_absolute() else ROOT / report_out


def run_matrix(args: object) -> tuple[dict[str, object], Path]:
    report_path = resolve_report_path(args.report_out)
    case_root = report_path.parent / "cases"

    results: list[dict[str, object]] = []
    failures: list[dict[str, object]] = []
    selected: list[BehaviorFixture] = []
    try:
        ensure_native_binaries()
        selected = select_fixtures(
            load_behavior_fixtures(),
            phases=args.phase,
            limit=args.limit,
        )
        for index, fixture in enumerate(selected, start=1):
            print(
                f"behavior-matrix-progress: [{index}/{len(selected)}] "
                f"START fixture={fixture.relative_source}",
                flush=True,
            )
            case_dir = case_root / f"{index:03d}-{fixture.owner_phase}-{fixture.behavior_family}-{fixture.source_path.stem}"
            try:
                result = execute_fixture(fixture, case_dir)
                results.append(result)
                print(
                    f"behavior-matrix-progress: [{index}/{len(selected)}] "
                    f"DONE fixture={fixture.relative_source} status=PASS",
                    flush=True,
                )
            except BehaviorMatrixFailure as exc:
                failure = {
                    "fixture": fixture.relative_source,
                    "metadata": fixture.relative_metadata,
                    "message": str(exc),
                }
                failures.append(failure)
                print(
                    f"behavior-matrix-progress: [{index}/{len(selected)}] "
                    f"DONE fixture={fixture.relative_source} status=FAIL",
                    flush=True,
                )
                break
    except BehaviorMatrixFailure as exc:
        failures.append({"fixture": "", "metadata": "", "message": str(exc)})

    payload = build_summary_payload(results=results, failures=failures, selected=selected)
    return payload, report_path


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    payload, report_path = run_matrix(args)
    render_report(report_path, payload)
    print(f"summary_path: {repo_rel(report_path)}")
    print(f"status: {payload['status']}")
    return 0 if not payload["failures"] else 1
