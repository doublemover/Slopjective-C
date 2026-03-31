#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/property_behavior_macro_runtime_materialization_implementation_contract.json"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-metaprogramming-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-metaprogramming-e2e/summary.json"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/metaprogramming-lowering"
JSON_OUT = OUT_DIR / "metaprogramming_lowering_summary.json"
MD_OUT = OUT_DIR / "metaprogramming_lowering_summary.md"


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

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/check_metaprogramming_interop_closure_metaprogramming_lowering.py",
        "all_authoritative_code_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_code_paths"]),
        "all_authoritative_fixture_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_fixture_paths"]),
        "all_authoritative_probe_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_probe_paths"]),
        "acceptance_report_passes": acceptance_report.get("status") == "PASS",
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "e2e_report_passes": e2e_report.get("status") == "PASS",
        "acceptance_report_contains_all_authoritative_cases": all(case_id in acceptance_case_ids for case_id in contract["authoritative_case_ids"]),
        "conformance_report_requires_runtime_materialization_cases": all(case_id in conformance_report.get("required_case_ids", []) for case_id in (
            "metaprogramming-derive-property-behavior-semantics",
            "metaprogramming-executable-lowering",
            "cross-module-metaprogramming-artifact-preservation",
            "metaprogramming-runtime-abi-cache-surface",
            "live-metaprogramming-cache-runtime-integration",
        )),
        "acceptance_report_publishes_all_authoritative_surfaces": all(surface in acceptance_report for surface in contract["authoritative_surfaces"]),
        "runtime_abi_surface_keeps_property_behavior_narrow": acceptance_report.get("runtime_metaprogramming_runtime_abi_cache_surface", {}).get("expansion_runtime_model") == "property-behavior-runtime-support-is-live-while-macro-host-execution-runtime-process-launch-and-package-loading-remain-fail-closed-on-the-expansion-boundary-snapshot",
        "synthesized_accessor_surface_keeps_storage_helper_model": acceptance_report.get("executable_synthesized_accessor_property_lowering_surface", {}).get("storage_model") == "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals",
        "runtime_property_surface_keeps_runtime_owned_layout_model": acceptance_report.get("runtime_property_ivar_accessor_reflection_implementation_surface", {}).get("implementation_model") == "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation",
        "e2e_materializes_then_reuses_host_cache": e2e_report.get("provider_first_materialization_state") == "materialized" and e2e_report.get("provider_second_materialization_state") == "cache-hit",
        "e2e_preserves_cross_module_link_plan": bool(e2e_report.get("consumer_link_plan_path")),
    }

    summary = {
        "issue": "metaprogramming-interop-closure-metaprogramming-lowering",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_report_count": len(contract["canonical_reports"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "authoritative_surface_count": len(contract["authoritative_surfaces"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Metaprogramming Lowering Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical reports: `{summary['canonical_report_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Authoritative surfaces: `{summary['authoritative_surface_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
