#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/interop_runtime_ownership_abi_policy.json"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-interop-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-interop-e2e/summary.json"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/live-interop-runtime"
JSON_OUT = OUT_DIR / "live_interop_runtime_summary.json"
MD_OUT = OUT_DIR / "live_interop_runtime_summary.md"

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
    bridge_surface = acceptance_report.get("runtime_package_loader_bridge_abi_surface", {})
    impl_surface = acceptance_report.get("runtime_package_loading_interop_implementation_surface", {})

    checks = {
        "acceptance_report_passes": acceptance_report.get("status") == "PASS",
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "e2e_report_passes": e2e_report.get("status") == "PASS",
        "acceptance_report_contains_live_interop_cases": all(case_id in acceptance_case_ids for case_id in (
            "mixed-image-compatibility-interop-semantics",
            "c-cpp-swift-bridge-compatibility-semantics",
            "runtime-package-loader-bridge-abi",
            "live-package-loading-interop-runtime-implementation",
        )),
        "conformance_report_requires_all_interop_cases": all(case_id in conformance_report.get("required_case_ids", []) for case_id in contract["authoritative_interop_case_ids"]),
        "bridge_surface_matches_expected_model": bridge_surface.get("runtime_abi_model") == EXPECTED_BRIDGE_ABI_MODEL,
        "bridge_surface_keeps_private_header_boundary": bridge_surface.get("internal_header_path") == "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h" and bridge_surface.get("public_header_path") == "native/objc3c/src/runtime/objc3_runtime.h",
        "implementation_surface_matches_expected_model": impl_surface.get("implementation_model") == EXPECTED_INTEROP_IMPL_MODEL,
        "implementation_surface_requires_real_artifacts": impl_surface.get("requires_runtime_import_surface_artifact") is True and impl_surface.get("requires_cross_module_link_plan_artifact") is True and impl_surface.get("requires_linked_runtime_probe") is True and impl_surface.get("requires_real_compile_output") is True,
        "e2e_packaging_probe_reports_operator_visible_evidence": e2e_report.get("packaging_probe_payload", {}).get("operator_visible_evidence_ready") == 1,
        "e2e_bridge_probe_reports_full_generation_readiness": e2e_report.get("bridge_probe_payload", {}).get("runtime_generation_ready") == 1 and e2e_report.get("bridge_probe_payload", {}).get("bridge_generation_ready") == 1,
        "e2e_preserves_provider_and_consumer_artifacts": bool(e2e_report.get("provider_bridge_json_path")) and bool(e2e_report.get("consumer_cross_module_link_plan_path")),
    }

    summary = {
        "issue": "metaprogramming-interop-closure-live-interop-runtime",
        "contract_id": contract["contract_id"],
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Live Interop Runtime Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
