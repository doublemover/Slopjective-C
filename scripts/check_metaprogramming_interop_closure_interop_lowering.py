#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/interop_bridge_cross_module_artifact_implementation_contract.json"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-interop-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-interop-e2e/summary.json"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/interop-lowering"
JSON_OUT = OUT_DIR / "interop_lowering_summary.json"
MD_OUT = OUT_DIR / "interop_lowering_summary.md"

EXPECTED_BRIDGE_ABI_MODEL = "private-runtime-snapshots-publish-package-loader-topology-and-bridge-generation-readiness-through-the-live-runtime-library-without-public-abi-widening"
EXPECTED_INTEROP_IMPL_MODEL = "live-runtime-package-loader-snapshots-agree-with-the-emitted-interop-link-plan-and-bridge-artifacts-for-the-current-mixed-image-packaging-boundary"


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
        "summary_script_link_matches": contract["summary_script"] == "scripts/check_metaprogramming_interop_closure_interop_lowering.py",
        "all_authoritative_code_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_code_paths"]),
        "all_authoritative_fixture_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_fixture_paths"]),
        "all_authoritative_probe_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_probe_paths"]),
        "acceptance_report_passes": acceptance_report.get("status") == "PASS",
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "e2e_report_passes": e2e_report.get("status") == "PASS",
        "acceptance_report_contains_all_authoritative_cases": all(case_id in acceptance_case_ids for case_id in contract["authoritative_case_ids"]),
        "conformance_report_requires_all_authoritative_cases": all(case_id in conformance_report.get("required_case_ids", []) for case_id in contract["authoritative_case_ids"]),
        "acceptance_report_publishes_all_authoritative_surfaces": all(surface in acceptance_report for surface in contract["authoritative_surfaces"]),
        "bridge_abi_surface_matches_expected_model": acceptance_report.get("runtime_package_loader_bridge_abi_surface", {}).get("runtime_abi_model") == EXPECTED_BRIDGE_ABI_MODEL,
        "interop_implementation_surface_matches_expected_model": acceptance_report.get("runtime_package_loading_interop_implementation_surface", {}).get("implementation_model") == EXPECTED_INTEROP_IMPL_MODEL,
        "conformance_report_keeps_narrow_boundary": conformance_report.get("integration_surface_comparison") == "stale-or-missing" and conformance_report.get("live_case_run_dir") is None,
        "e2e_preserves_packaging_probe_payload": e2e_report.get("packaging_probe_payload", {}).get("operator_visible_evidence_ready") == 1,
        "e2e_preserves_bridge_probe_payload": e2e_report.get("bridge_probe_payload", {}).get("bridge_generation_ready") == 1 and e2e_report.get("bridge_probe_payload", {}).get("cross_module_packaging_ready") == 1,
        "e2e_preserves_packaged_probe_executables": bool(e2e_report.get("packaged_probe_executables", {}).get("packaging_probe")) and bool(e2e_report.get("packaged_probe_executables", {}).get("bridge_probe")),
    }

    summary = {
        "issue": "metaprogramming-interop-closure-interop-lowering",
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
        "# Interop Lowering Summary\n\n"
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
