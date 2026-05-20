#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json

ROOT = Path(__file__).resolve().parents[1]
REPORT_PATH = ROOT / "tmp/reports/runtime/acceptance/summary.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_error_runtime_closure.md"
OUT_DIR = ROOT / "tmp/reports/error-runtime-closure/semantics-bridge"
JSON_OUT = OUT_DIR / "semantics_bridge_summary.json"
MD_OUT = OUT_DIR / "semantics_bridge_summary.md"
SUMMARY_CONTRACT_ID = "objc3c.error_runtime.closure.semantics.bridge.summary.v1"
REQUIRED_CASES = {
    "catch-filter-finalization-source",
    "executable-try-throw-do-catch-semantics",
    "bridging-filter-unwind-compatibility-diagnostics",
    "error-runtime-abi-cleanup",
    "live-error-runtime-integration",
}
REQUIRED_SURFACES = {
    "runtime_catch_filter_finalization_source_surface": "objc3c.runtime.catch.filter.finalization.source.surface.v1",
    "runtime_bridging_filter_unwind_diagnostics_surface": "objc3c.runtime.bridging.filter.unwind.diagnostics.surface.v1",
    "runtime_error_runtime_abi_cleanup_surface": "objc3c.runtime.error.runtime.abi.cleanup.surface.v1",
    "runtime_error_propagation_catch_cleanup_runtime_implementation_surface": "objc3c.runtime.error.propagation.catch.cleanup.runtime.implementation.surface.v1",
}


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def case_map(cases: Any) -> dict[str, dict[str, Any]]:
    expect(isinstance(cases, list), "runtime acceptance cases were not published")
    return {
        str(case.get("case_id")): case
        for case in cases
        if isinstance(case, dict) and case.get("case_id") is not None
    }


def diagnostic_codes(summary: dict[str, Any]) -> set[str]:
    negative_batch = summary.get("negative_diagnostics_batch")
    results = negative_batch.get("results") if isinstance(negative_batch, dict) else []
    codes: set[str] = set()
    if not isinstance(results, list):
        return codes
    for entry in results:
        if not isinstance(entry, dict):
            continue
        for code in entry.get("diagnostic_codes", []):
            codes.add(str(code))
    return codes


def fixture_paths(surface: Any) -> set[str]:
    if not isinstance(surface, dict):
        return set()
    paths = surface.get("authoritative_fixture_paths", [])
    if not isinstance(paths, list):
        return set()
    return {str(path) for path in paths}


def main() -> int:
    report = load_json(REPORT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    expect(report.get("status") == "PASS", "runtime acceptance report is not PASS")

    cases = case_map(report.get("cases", []))
    checks = {
        "runbook_preserves_private_bridge_boundary": "private foreign-exception normalization" in runbook_text
        and "public runtime ABI widening" in runbook_text,
        "required_cases_present_and_passing": True,
        "required_surface_packets_present": True,
        "try_catch_semantics_include_rethrow_and_fail_closed_diagnostics": False,
        "catch_filter_surface_inventory_includes_rethrow_and_status_fixtures": False,
        "bridge_semantics_include_status_negative_and_ready_live_surface": False,
        "bridge_surface_inventory_includes_status_missing_out_fixture": False,
        "runtime_abi_preserves_foreign_exception_bridge": False,
        "live_runtime_preserves_status_bridge_catch_kind": False,
    }

    for case_id in sorted(REQUIRED_CASES):
        case = cases.get(case_id)
        if not isinstance(case, dict) or case.get("passed") is not True:
            checks["required_cases_present_and_passing"] = False

    for surface_key, contract_id in REQUIRED_SURFACES.items():
        surface = report.get(surface_key)
        if not isinstance(surface, dict) or surface.get("contract_id") != contract_id:
            checks["required_surface_packets_present"] = False

    try_case = cases.get("executable-try-throw-do-catch-semantics", {})
    try_summary = try_case.get("summary") if isinstance(try_case, dict) else None
    if isinstance(try_summary, dict):
        rethrow_fixture = try_summary.get("rethrow_fixture")
        live_runtime_fixture = try_summary.get("live_runtime_surface_fixture")
        checks["try_catch_semantics_include_rethrow_and_fail_closed_diagnostics"] = (
            isinstance(rethrow_fixture, dict)
            and rethrow_fixture.get("rethrow_sites") == 1
            and isinstance(live_runtime_fixture, dict)
            and live_runtime_fixture.get("native_emit_remains_fail_closed") is False
            and live_runtime_fixture.get("ready_for_lowering_and_runtime") is True
            and {"O3S272", "O3S271", "O3S341", "O3S274", "O3S284", "O3S202", "O3S269"}
            <= diagnostic_codes(try_summary)
        )

    catch_filter_surface = report.get("runtime_catch_filter_finalization_source_surface")
    catch_filter_fixtures = fixture_paths(catch_filter_surface)
    checks["catch_filter_surface_inventory_includes_rethrow_and_status_fixtures"] = {
        "tests/tooling/fixtures/native/rethrow_in_throws_catch_positive.objc3",
        "tests/tooling/fixtures/native/bridge_legality_status_missing_out_negative.objc3",
    } <= catch_filter_fixtures

    bridge_case = cases.get("bridging-filter-unwind-compatibility-diagnostics", {})
    bridge_summary = bridge_case.get("summary") if isinstance(bridge_case, dict) else None
    if isinstance(bridge_summary, dict):
        live_runtime_fixture = bridge_summary.get("live_runtime_surface_fixture")
        checks["bridge_semantics_include_status_negative_and_ready_live_surface"] = (
            isinstance(live_runtime_fixture, dict)
            and live_runtime_fixture.get("native_emit_remains_fail_closed") is False
            and live_runtime_fixture.get("ready_for_lowering_and_runtime") is True
            and {"O3S275", "O3S279", "O3S276", "O3S277", "O3S278", "O3S280", "O3S282", "O3S283", "O3S281"}
            <= diagnostic_codes(bridge_summary)
        )

    bridge_surface = report.get("runtime_bridging_filter_unwind_diagnostics_surface")
    checks["bridge_surface_inventory_includes_status_missing_out_fixture"] = (
        "tests/tooling/fixtures/native/bridge_legality_status_missing_out_negative.objc3"
        in fixture_paths(bridge_surface)
    )

    abi_case = cases.get("error-runtime-abi-cleanup", {})
    abi_summary = abi_case.get("summary") if isinstance(abi_case, dict) else None
    if isinstance(abi_summary, dict):
        checks["runtime_abi_preserves_foreign_exception_bridge"] = (
            abi_summary.get("foreign_exception_bridge_symbol")
            == "objc3_runtime_bridge_foreign_exception_error_i32"
        )

    live_case = cases.get("live-error-runtime-integration", {})
    live_summary = live_case.get("summary") if isinstance(live_case, dict) else None
    if isinstance(live_summary, dict):
        checks["live_runtime_preserves_status_bridge_catch_kind"] = (
            live_summary.get("status") == 0
            and live_summary.get("rc") == 54
            and live_summary.get("last_catch_kind_name") == "nserror"
            and live_summary.get("throws_abi_contract")
            == "objc3c.error_handling.throws.abi.propagation.lowering.v1"
        )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/check_error_runtime_closure_semantics_bridge.py",
        "source_report": "tmp/reports/runtime/acceptance/summary.json",
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACES.keys()),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Error Runtime Closure Semantics Bridge Summary\n\n"
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
