#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-error-conformance/summary.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_error_runtime_closure.md"
OUT_DIR = ROOT / "tmp/reports/error-runtime-closure/live-throw-cleanup-runtime"
JSON_OUT = OUT_DIR / "live_throw_cleanup_runtime_summary.json"
MD_OUT = OUT_DIR / "live_throw_cleanup_runtime_summary.md"
SUMMARY_CONTRACT_ID = "objc3c.error_runtime.closure.live.throw.cleanup.runtime.summary.v1"
REQUIRED_CASES = {
    "catch-filter-finalization-source",
    "executable-try-throw-do-catch-semantics",
    "executable-throw-catch-cleanup-lowering",
    "error-runtime-abi-cleanup",
    "live-error-runtime-integration",
}


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"missing report: {path}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"report is not a JSON object: {path}")
    return payload


def main() -> int:
    acceptance = load_json(ACCEPTANCE_REPORT)
    conformance = load_json(CONFORMANCE_REPORT)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    expect(acceptance.get("status") == "PASS", "runtime acceptance report is not PASS")
    expect(conformance.get("status") == "PASS", "runnable error conformance report is not PASS")

    acceptance_cases = acceptance.get("cases", [])
    expect(isinstance(acceptance_cases, list), "runtime acceptance cases were not published")
    case_map = {
        str(case.get("case_id")): case
        for case in acceptance_cases
        if isinstance(case, dict) and case.get("case_id") is not None
    }

    checks = {
        "runbook_mentions_coupled_runtime_story": "unwind ordering, cleanup execution, and catch filtering are one coupled runtime story" in runbook_text,
        "required_cases_present_and_passing": True,
        "catch_filter_source_summary_is_complete": False,
        "try_throw_do_catch_semantics_summary_is_complete": False,
        "lowering_case_publishes_helper_calls": False,
        "live_runtime_case_preserves_status_and_catch_kind": False,
        "conformance_requires_runtime_cases": False,
    }

    for case_id in sorted(REQUIRED_CASES):
        case = case_map.get(case_id)
        if not isinstance(case, dict) or case.get("passed") is not True:
            checks["required_cases_present_and_passing"] = False

    catch_case = case_map.get("catch-filter-finalization-source", {})
    catch_summary = catch_case.get("summary") if isinstance(catch_case, dict) else None
    if isinstance(catch_summary, dict):
        checks["catch_filter_source_summary_is_complete"] = (
            catch_summary.get("try_expression_sites") == 3
            and catch_summary.get("catch_clause_sites") == 2
            and catch_summary.get("bridge_callable_sites") == 2
            and catch_summary.get("try_eligible_bridge_callable_sites") == 2
        )

    try_case = case_map.get("executable-try-throw-do-catch-semantics", {})
    try_summary = try_case.get("summary") if isinstance(try_case, dict) else None
    if isinstance(try_summary, dict):
        native_fail_closed_fixture = try_summary.get("native_fail_closed_fixture")
        checks["try_throw_do_catch_semantics_summary_is_complete"] = (
            try_summary.get("try_expression_sites") == 3
            and try_summary.get("catch_clause_sites") == 2
            and try_summary.get("bridged_callable_try_sites") == 1
            and isinstance(native_fail_closed_fixture, dict)
            and native_fail_closed_fixture.get("native_emit_remains_fail_closed") is True
        )

    lowering_case = case_map.get("executable-throw-catch-cleanup-lowering", {})
    lowering_summary = lowering_case.get("summary") if isinstance(lowering_case, dict) else None
    if isinstance(lowering_summary, dict):
        helper_calls = lowering_summary.get("helper_calls")
        checks["lowering_case_publishes_helper_calls"] = isinstance(helper_calls, dict) and all(
            helper_calls.get(key) is True for key in ("store", "load", "status_bridge", "catch_match")
        )

    live_case = case_map.get("live-error-runtime-integration", {})
    live_summary = live_case.get("summary") if isinstance(live_case, dict) else None
    if isinstance(live_summary, dict):
        checks["live_runtime_case_preserves_status_and_catch_kind"] = (
            live_summary.get("status") == 0
            and live_summary.get("rc") == 54
            and live_summary.get("last_catch_kind_name") == "nserror"
        )

    required_case_ids = conformance.get("required_case_ids")
    if isinstance(required_case_ids, list):
        checks["conformance_requires_runtime_cases"] = all(
            case_id in required_case_ids
            for case_id in (
                "catch-filter-finalization-source",
                "executable-try-throw-do-catch-semantics",
                "executable-throw-catch-cleanup-lowering",
                "error-runtime-abi-cleanup",
                "live-error-runtime-integration",
            )
        )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/check_error_runtime_closure_live_throw_cleanup_runtime.py",
        "source_reports": [
            "tmp/reports/runtime/acceptance/summary.json",
            "tmp/reports/runtime/runnable-error-conformance/summary.json",
        ],
        "required_case_ids": sorted(REQUIRED_CASES),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Error Runtime Closure Live Throw Cleanup Runtime Summary\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Source reports: `{len(payload['source_reports'])}`\n"
        f"- Required cases: `{len(payload['required_case_ids'])}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
