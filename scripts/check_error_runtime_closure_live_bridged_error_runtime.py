#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-error-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-error-e2e/summary.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_error_runtime_closure.md"
OUT_DIR = ROOT / "tmp/reports/error-runtime-closure/live-bridged-error-runtime"
JSON_OUT = OUT_DIR / "live_bridged_error_runtime_summary.json"
MD_OUT = OUT_DIR / "live_bridged_error_runtime_summary.md"
SUMMARY_CONTRACT_ID = "objc3c.error_runtime.closure.live.bridged.error.runtime.summary.v1"


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
    e2e = load_json(E2E_REPORT)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    expect(acceptance.get("status") == "PASS", "runtime acceptance report is not PASS")
    expect(conformance.get("status") == "PASS", "runnable error conformance report is not PASS")
    expect(e2e.get("status") == "PASS", "runnable error e2e report is not PASS")

    acceptance_cases = acceptance.get("cases", [])
    expect(isinstance(acceptance_cases, list), "runtime acceptance cases were not published")
    case_map = {
        str(case.get("case_id")): case
        for case in acceptance_cases
        if isinstance(case, dict) and case.get("case_id") is not None
    }

    bridge_case = case_map.get("cross-module-error-metadata-replay-preservation", {})
    bridge_summary = bridge_case.get("summary") if isinstance(bridge_case, dict) else None
    live_case = case_map.get("live-error-runtime-integration", {})
    live_summary = live_case.get("summary") if isinstance(live_case, dict) else None
    implementation_surface = conformance.get("error_runtime_implementation_surface")
    probe_payload = e2e.get("probe_payload")

    checks = {
        "runbook_mentions_cross_module_claim_limit": "cross-module propagation claims are limited to the manifest/runtime-registration/replay path" in runbook_text,
        "acceptance_cross_module_case_passes": isinstance(bridge_case, dict) and bridge_case.get("passed") is True,
        "acceptance_live_runtime_case_passes": isinstance(live_case, dict) and live_case.get("passed") is True,
        "acceptance_cross_module_summary_is_complete": False,
        "acceptance_live_runtime_summary_is_complete": False,
        "conformance_implementation_surface_carries_live_case": False,
        "e2e_probe_payload_preserves_runtime_bridge_state": False,
    }

    if isinstance(bridge_summary, dict):
        checks["acceptance_cross_module_summary_is_complete"] = (
            isinstance(bridge_summary.get("provider_import_surface"), str)
            and bridge_summary.get("provider_import_surface", "") != ""
            and isinstance(bridge_summary.get("consumer_link_plan"), str)
            and bridge_summary.get("consumer_link_plan", "") != ""
            and bridge_summary.get("imported_module_registration_ordinal") == 1
            and bridge_summary.get("local_module_registration_ordinal") == 2
        )

    if isinstance(live_summary, dict):
        checks["acceptance_live_runtime_summary_is_complete"] = (
            live_summary.get("status") == 0
            and live_summary.get("rc") == 54
            and live_summary.get("last_catch_kind_name") == "nserror"
            and live_summary.get("throws_abi_contract") == "objc3c.error_handling.throws.abi.propagation.lowering.v1"
        )

    if isinstance(implementation_surface, dict):
        checks["conformance_implementation_surface_carries_live_case"] = (
            "live-error-runtime-integration" in implementation_surface.get("authoritative_case_ids", [])
            and "objc3_runtime_copy_error_bridge_state_for_testing" in implementation_surface.get("private_error_runtime_abi_boundary", [])
        )

    if isinstance(probe_payload, dict):
        checks["e2e_probe_payload_preserves_runtime_bridge_state"] = (
            probe_payload.get("status") == 0
            and probe_payload.get("store_call_count") == 1
            and probe_payload.get("load_call_count") == 1
            and probe_payload.get("status_bridge_call_count") == 1
            and probe_payload.get("last_status_bridge_status_value") == 5
            and probe_payload.get("last_status_bridge_error_value") == 45
            and probe_payload.get("catch_match_call_count") == 1
            and probe_payload.get("last_catch_kind_name") == "nserror"
        )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/check_error_runtime_closure_live_bridged_error_runtime.py",
        "source_reports": [
            "tmp/reports/runtime/acceptance/summary.json",
            "tmp/reports/runtime/runnable-error-conformance/summary.json",
            "tmp/reports/runtime/runnable-error-e2e/summary.json",
        ],
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Error Runtime Closure Live Bridged Error Runtime Summary\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Source reports: `{len(payload['source_reports'])}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
