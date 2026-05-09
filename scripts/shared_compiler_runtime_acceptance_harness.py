#!/usr/bin/env python3
"""Shared executable acceptance harness for live objc3c runtime validation."""

from __future__ import annotations

import argparse
import json
import os
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import run_capture
from shared_compiler_runtime_acceptance_catalog import (
    COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
    COMPILE_PROVENANCE_CONTRACT_ID,
    HARNESS_REPORT_ROOT,
    HARNESS_SUMMARY_CONTRACT_ID,
    SUITE_MAP,
    SUITES,
    SuiteEntry,
    SurfaceRequirement,
    build_catalog_payload,
    build_harness_surface,
    check_catalog,
)
from shared_compiler_runtime_acceptance_reporting import summarize_report


ROOT = Path(__file__).resolve().parents[1]


def emit_json(payload: object) -> int:
    json.dump(payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


def load_report(report_path: Path) -> dict[str, Any]:
    if not report_path.is_file():
        raise RuntimeError(f"expected suite report was not published: {repo_rel(report_path)}")
    try:
        payload = json.loads(report_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"invalid suite report JSON at {repo_rel(report_path)}: {exc}") from exc
    if not isinstance(payload, dict):
        raise RuntimeError(f"suite report at {repo_rel(report_path)} did not contain a JSON object")
    return payload


def require_surface(report: dict[str, Any], requirement: SurfaceRequirement) -> dict[str, Any]:
    surface = report.get(requirement.key)
    if not isinstance(surface, dict):
        raise RuntimeError(f"suite report did not publish {requirement.key}")
    if surface.get("contract_id") != requirement.contract_id:
        raise RuntimeError(
            f"suite report published the wrong contract for {requirement.key}: "
            f"{surface.get('contract_id')!r}"
        )
    for field in requirement.required_fields:
        if field not in surface:
            raise RuntimeError(f"suite report surface {requirement.key} is missing required field {field}")
    return surface


def validate_suite_report(entry: SuiteEntry, report: dict[str, Any]) -> dict[str, dict[str, Any]]:
    if report.get("status") != "PASS":
        raise RuntimeError(f"suite report status was not PASS for {entry.suite_id}")

    surfaces = {
        requirement.key: require_surface(report, requirement)
        for requirement in entry.required_surfaces
    }
    acceptance_suite_surface = surfaces["acceptance_suite_surface"]
    if (
        acceptance_suite_surface.get("compile_output_provenance_contract_id")
        != COMPILE_PROVENANCE_CONTRACT_ID
    ):
        raise RuntimeError("acceptance_suite_surface drifted from the compile provenance contract")
    if (
        acceptance_suite_surface.get("compile_output_truthfulness_contract_id")
        != COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID
    ):
        raise RuntimeError("acceptance_suite_surface drifted from the compile truthfulness contract")

    if entry.suite_id == "runtime-acceptance":
        if report.get("case_count", 0) <= 0:
            raise RuntimeError("runtime acceptance report did not publish any cases")
        if acceptance_suite_surface.get("suite_path") != "scripts/check_objc3c_runtime_acceptance.py":
            raise RuntimeError("runtime acceptance suite surface drifted from the runtime acceptance suite path")
        if acceptance_suite_surface.get("report_path") != entry.report_path:
            raise RuntimeError("runtime acceptance suite surface drifted from the expected report path")
    else:
        if report.get("runner_path") != "scripts.objc3c_workflow":
            raise RuntimeError("composite suite report drifted from the public workflow runner path")
        steps = report.get("steps")
        if not isinstance(steps, list) or not steps:
            raise RuntimeError("composite suite report did not publish child steps")
        if acceptance_suite_surface.get("suite_path") != "scripts/check_objc3c_runtime_acceptance.py":
            raise RuntimeError("composite suite did not carry forward the runtime acceptance suite path")
        if acceptance_suite_surface.get("report_path") != "tmp/reports/runtime/acceptance/summary.json":
            raise RuntimeError("composite suite did not carry forward the runtime acceptance report path")

    return surfaces


def run_suite(entry: SuiteEntry) -> dict[str, Any]:
    report_path = ROOT / entry.report_path
    if os.environ.get("OBJC3C_SKIP_SUITE_RERUN") != "1":
        command = [str(token) for token in entry.command]
        result = run_capture(command)
        if result.returncode != 0:
            raise RuntimeError(
                f"suite execution failed for {entry.suite_id} with exit code {result.returncode}"
            )
    elif not report_path.is_file():
        raise RuntimeError(
            f"suite execution rerun was skipped but report is missing for {entry.suite_id}: {entry.report_path}"
        )
    report = load_report(report_path)
    surfaces = validate_suite_report(entry, report)
    return summarize_report(entry, report, surfaces)


def write_summary(summary_out: Path, payload: dict[str, Any]) -> None:
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def default_summary_path(run_all: bool, suite_id: str | None) -> Path:
    if run_all:
        return HARNESS_REPORT_ROOT / "all-suites.json"
    if suite_id is None:
        raise RuntimeError("suite_id must be provided for single-suite summary path resolution")
    return HARNESS_REPORT_ROOT / suite_id / "summary.json"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--list-suites", action="store_true")
    parser.add_argument("--show-suite")
    parser.add_argument("--check-catalog", action="store_true")
    parser.add_argument("--check-roots", action="store_true")
    parser.add_argument("--run-suite")
    parser.add_argument("--run-all", action="store_true")
    parser.add_argument("--summary-out", type=Path)
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    selected = list(SUITES)

    if args.show_suite:
        selected = [SUITE_MAP[args.show_suite]] if args.show_suite in SUITE_MAP else []
        if not selected:
            print(json.dumps({"ok": False, "error": f"unknown suite: {args.show_suite}"}, indent=2), file=sys.stderr)
            return 1

    if args.list_suites:
        return emit_json(build_catalog_payload(selected))

    if args.show_suite:
        return emit_json(build_catalog_payload(selected))

    if args.check_catalog or args.check_roots:
        payload = build_catalog_payload(selected)
        payload.update(check_catalog(selected))
        rendered = json.dumps(payload, indent=2)
        if args.summary_out is not None:
            write_summary(args.summary_out, payload)
        print(rendered)
        return 0 if payload["ok"] else 1

    if args.run_all and args.run_suite:
        print(json.dumps({"ok": False, "error": "choose either --run-suite or --run-all"}, indent=2), file=sys.stderr)
        return 2

    if args.run_suite:
        entry = SUITE_MAP.get(args.run_suite)
        if entry is None:
            print(json.dumps({"ok": False, "error": f"unknown suite: {args.run_suite}"}, indent=2), file=sys.stderr)
            return 1
        results = [run_suite(entry)]
        summary_out = args.summary_out or default_summary_path(False, entry.suite_id)
    elif args.run_all:
        results = [run_suite(entry) for entry in SUITES]
        summary_out = args.summary_out or default_summary_path(True, None)
    else:
        print(json.dumps({"ok": False, "error": "no mode selected"}, indent=2), file=sys.stderr)
        return 1

    payload = {
        "contract_id": HARNESS_SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "harness_surface": build_harness_surface(SUITES if args.run_all else [SUITE_MAP[results[0]["suite_id"]]]),
        "suite_count": len(results),
        "suites": results,
    }
    write_summary(summary_out, payload)
    print(f"summary_path: {repo_rel(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
