#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/interop_runtime_ownership_abi_policy.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_metaprogramming_interop_closure.md"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-interop-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-interop-e2e/summary.json"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/interop-policy"
JSON_OUT = OUT_DIR / "interop_runtime_ownership_abi_policy_summary.json"
MD_OUT = OUT_DIR / "interop_runtime_ownership_abi_policy_summary.md"

EXPECTED_BRIDGE_ABI_MODEL = "private-runtime-snapshots-publish-package-loader-topology-and-bridge-generation-readiness-through-the-live-runtime-library-without-public-abi-widening"
EXPECTED_INTEROP_IMPL_MODEL = "live-runtime-package-loader-snapshots-agree-with-the-emitted-interop-link-plan-and-bridge-artifacts-for-the-current-mixed-image-packaging-boundary"
EXPECTED_E2E_PACKAGING_FAIL_CLOSED = "header-module-and-bridge-generation-remain-unclaimed-until-next-runtime-phase"
EXPECTED_E2E_BRIDGE_FAIL_CLOSED = "missing-generated-artifacts-or-drifted-import-surface-paths-disable-live-interop-bridge-generation-claims"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def case_ids_from_acceptance(report: dict[str, Any]) -> set[str]:
    return {case.get("case_id") for case in report.get("cases", []) if isinstance(case, dict) and case.get("case_id")}


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    acceptance_report = read_json(ACCEPTANCE_REPORT)
    conformance_report = read_json(CONFORMANCE_REPORT)
    e2e_report = read_json(E2E_REPORT)
    acceptance_case_ids = case_ids_from_acceptance(acceptance_report)
    package_loader_bridge_surface = acceptance_report.get("runtime_package_loader_bridge_abi_surface", {})
    package_loading_impl_surface = acceptance_report.get("runtime_package_loading_interop_implementation_surface", {})

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_metaprogramming_interop_closure_interop_policy_summary.py",
        "all_authoritative_code_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_code_paths"]),
        "all_authoritative_fixture_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_fixture_paths"]),
        "all_authoritative_probe_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_probe_paths"]),
        "runbook_mentions_abi_ownership_error_async_boundary": "interop claims must pass through ABI, ownership, error, and async compatibility on the real packaged toolchain; comparison-only narratives do not count" in runbook_text,
        "runbook_keeps_public_abi_widening_out_of_scope": "public runtime ABI widening for interop/package-loading helpers remains out of scope for this milestone" in runbook_text,
        "acceptance_report_passes": acceptance_report.get("status") == "PASS",
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "e2e_report_passes": e2e_report.get("status") == "PASS",
        "conformance_report_requires_all_interop_cases": all(case_id in conformance_report.get("required_case_ids", []) for case_id in contract["authoritative_interop_case_ids"]),
        "acceptance_report_contains_all_guard_cases": all(case_id in acceptance_case_ids for case_id in contract["cross_track_guard_case_ids"]),
        "conformance_report_preserves_narrow_integration_comparison": conformance_report.get("integration_surface_comparison") == "stale-or-missing" and conformance_report.get("live_case_run_dir") is None,
        "package_loader_bridge_surface_matches_expected_model": package_loader_bridge_surface.get("runtime_abi_model") == EXPECTED_BRIDGE_ABI_MODEL,
        "package_loader_bridge_surface_keeps_private_header_boundary": package_loader_bridge_surface.get("internal_header_path") == "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h" and package_loader_bridge_surface.get("public_header_path") == "native/objc3c/src/runtime/objc3_runtime.h",
        "package_loading_implementation_surface_matches_expected_model": package_loading_impl_surface.get("implementation_model") == EXPECTED_INTEROP_IMPL_MODEL,
        "package_loading_implementation_surface_requires_real_artifacts": package_loading_impl_surface.get("requires_runtime_import_surface_artifact") is True and package_loading_impl_surface.get("requires_cross_module_link_plan_artifact") is True and package_loading_impl_surface.get("requires_linked_runtime_probe") is True and package_loading_impl_surface.get("requires_real_compile_output") is True,
        "e2e_packaging_probe_keeps_fail_closed_boundary": e2e_report.get("packaging_probe_payload", {}).get("fail_closed_model") == EXPECTED_E2E_PACKAGING_FAIL_CLOSED,
        "e2e_bridge_probe_keeps_fail_closed_boundary": e2e_report.get("bridge_probe_payload", {}).get("fail_closed_model") == EXPECTED_E2E_BRIDGE_FAIL_CLOSED,
        "runtime_cpp_preserves_bridge_generation_snapshot_symbols": "objc3_runtime_copy_interop_bridge_packaging_toolchain_snapshot_for_testing" in (ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp").read_text(encoding="utf-8") and "objc3_runtime_copy_interop_bridge_generation_snapshot_for_testing" in (ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp").read_text(encoding="utf-8"),
    }

    summary = {
        "issue": "metaprogramming-interop-closure-interop-runtime-ownership-abi-policy",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_surface_count": len(contract["canonical_surfaces"]),
        "semantic_model_count": len(contract["semantic_models"]),
        "claim_narrowing_constraint_count": len(contract["claim_narrowing_constraints"]),
        "authoritative_interop_case_id_count": len(contract["authoritative_interop_case_ids"]),
        "cross_track_guard_case_id_count": len(contract["cross_track_guard_case_ids"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Interop Runtime Ownership ABI Policy Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical surfaces: `{summary['canonical_surface_count']}`\n"
        f"- Semantic models: `{summary['semantic_model_count']}`\n"
        f"- Interop case ids: `{summary['authoritative_interop_case_id_count']}`\n"
        f"- Guard case ids: `{summary['cross_track_guard_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
