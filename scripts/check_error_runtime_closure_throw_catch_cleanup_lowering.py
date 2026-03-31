#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
REPORT_PATH = ROOT / "tmp/reports/runtime/acceptance/summary.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_error_runtime_closure.md"
OUT_DIR = ROOT / "tmp/reports/error-runtime-closure/throw-catch-cleanup-lowering"
JSON_OUT = OUT_DIR / "throw_catch_cleanup_lowering_summary.json"
MD_OUT = OUT_DIR / "throw_catch_cleanup_lowering_summary.md"
SUMMARY_CONTRACT_ID = "objc3c.error_runtime.closure.throw.catch.cleanup.lowering.summary.v1"
REQUIRED_CASES = {
    "executable-throw-catch-cleanup-lowering",
    "error-runtime-abi-cleanup",
    "live-error-runtime-integration",
}
REQUIRED_SURFACES = {
    "runtime_error_lowering_unwind_bridge_helper_surface": "objc3c.runtime.error.lowering.unwind.bridge.helper.surface.v1",
    "runtime_error_runtime_abi_cleanup_surface": "objc3c.runtime.error.runtime.abi.cleanup.surface.v1",
    "runtime_error_propagation_catch_cleanup_runtime_implementation_surface": "objc3c.runtime.error.propagation.catch.cleanup.runtime.implementation.surface.v1",
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
    report = load_json(REPORT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    expect(report.get("status") == "PASS", "runtime acceptance report is not PASS")

    cases = report.get("cases", [])
    expect(isinstance(cases, list), "runtime acceptance cases were not published")
    case_map = {
        str(case.get("case_id")): case
        for case in cases
        if isinstance(case, dict) and case.get("case_id") is not None
    }

    checks = {
        "runbook_mentions_shared_acceptance_truth": "shared acceptance output published by `scripts/check_objc3c_runtime_acceptance.py`" in runbook_text,
        "required_cases_present_and_passing": True,
        "required_surface_packets_present": True,
        "lowering_case_publishes_helper_calls": False,
        "runtime_abi_case_publishes_helper_symbols": False,
        "live_runtime_case_preserves_throws_contract": False,
    }

    for case_id in sorted(REQUIRED_CASES):
        case = case_map.get(case_id)
        if not isinstance(case, dict) or case.get("passed") is not True:
            checks["required_cases_present_and_passing"] = False

    for surface_key, contract_id in REQUIRED_SURFACES.items():
        surface = report.get(surface_key)
        if not isinstance(surface, dict) or surface.get("contract_id") != contract_id:
            checks["required_surface_packets_present"] = False

    lowering_case = case_map.get("executable-throw-catch-cleanup-lowering", {})
    lowering_summary = lowering_case.get("summary") if isinstance(lowering_case, dict) else None
    if isinstance(lowering_summary, dict):
        helper_calls = lowering_summary.get("helper_calls")
        checks["lowering_case_publishes_helper_calls"] = isinstance(helper_calls, dict) and all(
            helper_calls.get(key) is True for key in ("store", "load", "status_bridge", "catch_match")
        ) and lowering_summary.get("throws_abi_contract") == "objc3c.error_handling.throws.abi.propagation.lowering.v1"

    abi_case = case_map.get("error-runtime-abi-cleanup", {})
    abi_summary = abi_case.get("summary") if isinstance(abi_case, dict) else None
    if isinstance(abi_summary, dict):
        checks["runtime_abi_case_publishes_helper_symbols"] = all(
            isinstance(abi_summary.get(field), str) and abi_summary.get(field, "") != ""
            for field in (
                "bridge_state_snapshot_symbol",
                "store_symbol",
                "load_symbol",
                "status_bridge_symbol",
                "nserror_bridge_symbol",
                "catch_match_symbol",
            )
        )

    live_case = case_map.get("live-error-runtime-integration", {})
    live_summary = live_case.get("summary") if isinstance(live_case, dict) else None
    if isinstance(live_summary, dict):
        checks["live_runtime_case_preserves_throws_contract"] = (
            live_summary.get("status") == 0
            and live_summary.get("last_catch_kind_name") == "nserror"
            and live_summary.get("throws_abi_contract") == "objc3c.error_handling.throws.abi.propagation.lowering.v1"
        )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/check_error_runtime_closure_throw_catch_cleanup_lowering.py",
        "source_report": "tmp/reports/runtime/acceptance/summary.json",
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACES.keys()),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Error Runtime Closure Throw/Catch/Cleanup Lowering Summary\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Source report: `{payload['source_report']}`\n"
        f"- Required cases: `{len(payload['required_case_ids'])}`\n"
        f"- Required surfaces: `{len(payload['required_surface_keys'])}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
