#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/property_behavior_runtime_materialization_policy.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_metaprogramming_interop_closure.md"
ACCEPTANCE_REPORT = ROOT / "tmp/reports/runtime/acceptance/summary.json"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-metaprogramming-conformance/summary.json"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/property-behavior-policy"
JSON_OUT = OUT_DIR / "property_behavior_runtime_materialization_policy_summary.json"
MD_OUT = OUT_DIR / "property_behavior_runtime_materialization_policy_summary.md"

EXPECTED_SOURCE_MODEL = "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists"
EXPECTED_STORAGE_MODEL = "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals"
EXPECTED_DESCRIPTOR_MODEL = "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers"
EXPECTED_RUNTIME_MODEL = "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def case_ids_from_acceptance(report: dict[str, Any]) -> set[str]:
    return {case.get("case_id") for case in report.get("cases", []) if isinstance(case, dict) and case.get("case_id")}


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    acceptance_report = read_json(ACCEPTANCE_REPORT)
    conformance_report = read_json(CONFORMANCE_REPORT)
    acceptance_case_ids = case_ids_from_acceptance(acceptance_report)
    synthesized_surface = acceptance_report.get("executable_synthesized_accessor_property_lowering_surface", {})
    runtime_impl_surface = acceptance_report.get("runtime_property_ivar_accessor_reflection_implementation_surface", {})
    storage_abi_surface = acceptance_report.get("storage_accessor_runtime_abi_surface", {})
    metaprogramming_runtime_abi_surface = conformance_report.get("metaprogramming_runtime_abi_cache_surface", {})

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_metaprogramming_interop_closure_property_behavior_policy_summary.py",
        "all_authoritative_code_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_code_paths"]),
        "all_authoritative_fixture_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_fixture_paths"]),
        "all_authoritative_probe_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_probe_paths"]),
        "runbook_mentions_property_behaviors_are_runtime_observable": "property behaviors must be treated as runtime-observable behavior rather than compile-time decoration" in runbook_text,
        "runbook_mentions_unsupported_property_behaviors_fail_closed": "unsupported property behavior combinations stay fail-closed and diagnostic-backed until dedicated runtime evidence lands" in runbook_text,
        "acceptance_report_passes": acceptance_report.get("status") == "PASS",
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "acceptance_report_carries_all_authoritative_case_ids": all(case_id in acceptance_case_ids for case_id in contract["authoritative_case_ids"]),
        "conformance_report_carries_property_behavior_case": "metaprogramming-derive-property-behavior-semantics" in conformance_report.get("required_case_ids", []),
        "synthesized_surface_source_model_matches": synthesized_surface.get("source_model") == EXPECTED_SOURCE_MODEL,
        "synthesized_surface_storage_model_matches": synthesized_surface.get("storage_model") == EXPECTED_STORAGE_MODEL,
        "synthesized_surface_descriptor_model_matches": synthesized_surface.get("property_descriptor_model") == EXPECTED_DESCRIPTOR_MODEL,
        "synthesized_surface_explicit_non_goals_preserve_no_storage_global_fallback": "no-storage-global-fallbacks-or-sidecar-body-proof" in synthesized_surface.get("explicit_non_goals", []),
        "synthesized_surface_requires_real_runtime_artifacts": synthesized_surface.get("requires_coupled_registration_manifest") is True and synthesized_surface.get("requires_real_compile_output") is True and synthesized_surface.get("requires_linked_runtime_probe") is True,
        "runtime_boundary_preserves_property_behavior_model": metaprogramming_runtime_abi_surface.get("expansion_runtime_model") == "property-behavior-runtime-support-is-live-while-macro-host-execution-runtime-process-launch-and-package-loading-remain-fail-closed-on-the-expansion-boundary-snapshot",
        "runtime_implementation_surface_preserves_runtime_owned_layout_model": runtime_impl_surface.get("implementation_model") == "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation",
        "runtime_implementation_surface_preserves_fail_closed_lookup_model": runtime_impl_surface.get("fail_closed_model") == "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-fallback-synthesis",
        "storage_abi_surface_preserves_current_property_helpers": storage_abi_surface.get("current_property_read_symbol") == "objc3_runtime_read_current_property_i32" and storage_abi_surface.get("current_property_write_symbol") == "objc3_runtime_write_current_property_i32" and storage_abi_surface.get("current_property_exchange_symbol") == "objc3_runtime_exchange_current_property_i32",
        "runtime_cpp_preserves_expected_runtime_model_string": EXPECTED_RUNTIME_MODEL in (ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp").read_text(encoding="utf-8"),
    }

    summary = {
        "issue": "metaprogramming-interop-closure-property-behavior-runtime-materialization-policy",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_surface_count": len(contract["canonical_surfaces"]),
        "semantic_model_count": len(contract["semantic_models"]),
        "claim_narrowing_constraint_count": len(contract["claim_narrowing_constraints"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Property Behavior Runtime Materialization Policy Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical surfaces: `{summary['canonical_surface_count']}`\n"
        f"- Semantic models: `{summary['semantic_model_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
