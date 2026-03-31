#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/executable_proof_abi_contract.json"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-metaprogramming-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-metaprogramming-e2e/summary.json"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/live-metaprogramming-runtime"
JSON_OUT = OUT_DIR / "live_metaprogramming_runtime_summary.json"
MD_OUT = OUT_DIR / "live_metaprogramming_runtime_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def case_ids_from_acceptance(report: dict[str, Any]) -> set[str]:
    return {case.get("case_id") for case in report.get("cases", []) if isinstance(case, dict) and case.get("case_id")}


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    acceptance_report = read_json(ACCEPTANCE_REPORT)
    conformance_report = read_json(CONFORMANCE_REPORT)
    e2e_report = read_json(E2E_REPORT)
    acceptance_case_ids = case_ids_from_acceptance(acceptance_report)
    abi_surface = conformance_report.get("metaprogramming_runtime_abi_cache_surface", {})
    impl_surface = conformance_report.get("metaprogramming_cache_runtime_integration_implementation_surface", {})

    checks = {
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "e2e_report_passes": e2e_report.get("status") == "PASS",
        "acceptance_report_passes": acceptance_report.get("status") == "PASS",
        "acceptance_report_contains_live_metaprogramming_cases": all(case_id in acceptance_case_ids for case_id in (
            "metaprogramming-executable-lowering",
            "metaprogramming-runtime-abi-cache-surface",
            "live-metaprogramming-cache-runtime-integration",
            "property-layout",
            "property-execution",
        )),
        "runtime_abi_surface_preserves_private_boundary": abi_surface.get("expansion_host_boundary_snapshot_symbol") == "objc3_runtime_copy_metaprogramming_expansion_host_boundary_snapshot_for_testing" and abi_surface.get("macro_host_process_cache_integration_snapshot_symbol") == "objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing",
        "runtime_abi_surface_keeps_public_header_narrow": abi_surface.get("fail_closed_model") == "public-runtime-header-remains-registration-lookup-dispatch-only-and-runtime-package-loading-stays-disabled-until-deliberate-metaprogramming-runtime-abi-widening",
        "implementation_surface_carries_live_cache_case": impl_surface.get("authoritative_case_ids") == ["live-metaprogramming-cache-runtime-integration"],
        "e2e_materialization_sequence_is_truthful": e2e_report.get("provider_first_materialization_state") == "materialized" and e2e_report.get("provider_second_materialization_state") == "cache-hit",
        "e2e_probe_and_link_plan_exist": bool(e2e_report.get("probe_executable_path")) and bool(e2e_report.get("consumer_link_plan_path")),
    }

    summary = {
        "issue": "metaprogramming-interop-closure-live-metaprogramming-runtime",
        "contract_id": contract["contract_id"],
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Live Metaprogramming Runtime Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
