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
OUT_DIR = ROOT / "tmp/reports/error-runtime-closure/bridge-artifact"
JSON_OUT = OUT_DIR / "bridge_artifact_summary.json"
MD_OUT = OUT_DIR / "bridge_artifact_summary.md"
SUMMARY_CONTRACT_ID = "objc3c.error_runtime.closure.bridge.artifact.summary.v1"
REQUIRED_ACCEPTANCE_CASES = {
    "bridging-filter-unwind-compatibility-diagnostics",
    "error-lowering-unwind-bridge-helper-surface",
    "cross-module-error-metadata-replay-preservation",
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

    checks = {
        "runbook_mentions_cross_module_claim_limit": "cross-module propagation claims are limited to the manifest/runtime-registration/replay path" in runbook_text,
        "acceptance_cases_present_and_passing": True,
        "bridge_surface_contract_published": False,
        "cross_module_case_publishes_import_surface_and_link_plan": False,
        "conformance_requires_bridge_and_cross_module_cases": False,
        "conformance_preserves_private_bridge_boundary": False,
        "e2e_preserves_packaged_probe_bridge_counts": False,
    }

    for case_id in sorted(REQUIRED_ACCEPTANCE_CASES):
        case = case_map.get(case_id)
        if not isinstance(case, dict) or case.get("passed") is not True:
            checks["acceptance_cases_present_and_passing"] = False

    bridge_surface = acceptance.get("runtime_error_lowering_unwind_bridge_helper_surface")
    if isinstance(bridge_surface, dict):
        checks["bridge_surface_contract_published"] = (
            bridge_surface.get("contract_id") == "objc3c.runtime.error.lowering.unwind.bridge.helper.surface.v1"
            and "objc3c.error_handling.result.and.bridging.artifact.replay.v1" in bridge_surface.get("source_contract_ids", [])
            and "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp" in bridge_surface.get("authoritative_code_paths", [])
        )

    cross_module_case = case_map.get("cross-module-error-metadata-replay-preservation", {})
    cross_module_summary = cross_module_case.get("summary") if isinstance(cross_module_case, dict) else None
    if isinstance(cross_module_summary, dict):
        checks["cross_module_case_publishes_import_surface_and_link_plan"] = all(
            isinstance(cross_module_summary.get(field), str) and cross_module_summary.get(field, "") != ""
            for field in (
                "provider_import_surface",
                "consumer_link_plan",
                "imported_module_name",
                "local_module_name",
            )
        )

    required_case_ids = conformance.get("required_case_ids")
    implementation_surface = conformance.get("error_runtime_implementation_surface")
    if isinstance(required_case_ids, list) and isinstance(implementation_surface, dict):
        checks["conformance_requires_bridge_and_cross_module_cases"] = all(
            case_id in required_case_ids
            for case_id in (
                "bridging-filter-unwind-compatibility-diagnostics",
                "error-lowering-unwind-bridge-helper-surface",
                "cross-module-error-metadata-replay-preservation",
                "live-error-runtime-integration",
            )
        )
        checks["conformance_preserves_private_bridge_boundary"] = (
            implementation_surface.get("error_lowering_unwind_bridge_helper_surface_contract_id")
            == "objc3c.runtime.error.lowering.unwind.bridge.helper.surface.v1"
            and isinstance(implementation_surface.get("private_error_runtime_abi_boundary"), list)
            and "objc3_runtime_copy_error_bridge_state_for_testing" in implementation_surface.get("private_error_runtime_abi_boundary", [])
        )

    probe_payload = e2e.get("probe_payload")
    if isinstance(probe_payload, dict):
        checks["e2e_preserves_packaged_probe_bridge_counts"] = (
            probe_payload.get("status") == 0
            and probe_payload.get("store_call_count") == 1
            and probe_payload.get("load_call_count") == 1
            and probe_payload.get("status_bridge_call_count") == 1
            and probe_payload.get("catch_match_call_count") == 1
            and probe_payload.get("last_catch_kind_name") == "nserror"
        )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/check_error_runtime_closure_bridge_artifact.py",
        "source_reports": [
            "tmp/reports/runtime/acceptance/summary.json",
            "tmp/reports/runtime/runnable-error-conformance/summary.json",
            "tmp/reports/runtime/runnable-error-e2e/summary.json",
        ],
        "required_acceptance_case_ids": sorted(REQUIRED_ACCEPTANCE_CASES),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Error Runtime Closure Bridge Artifact Summary\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Source reports: `{len(payload['source_reports'])}`\n"
        f"- Required acceptance cases: `{len(payload['required_acceptance_case_ids'])}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
