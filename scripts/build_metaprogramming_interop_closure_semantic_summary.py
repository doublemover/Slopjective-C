#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/metaprogramming_runtime_semantic_model.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_metaprogramming_interop_closure.md"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-metaprogramming-conformance/summary.json"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/metaprogramming-semantic"
JSON_OUT = OUT_DIR / "metaprogramming_runtime_semantic_summary.json"
MD_OUT = OUT_DIR / "metaprogramming_runtime_semantic_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    conformance_report = read_json(CONFORMANCE_REPORT)
    program_surface_text = (ROOT / "stdlib/program_surface.json").read_text(encoding="utf-8")

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_metaprogramming_interop_closure_semantic_summary.py",
        "all_authoritative_code_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_code_paths"]),
        "all_authoritative_fixture_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_fixture_paths"]),
        "all_authoritative_probe_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_probe_paths"]),
        "runbook_mentions_runtime_surface_boundary": "macro/package/provenance, property-behavior, host-cache, and runtime-boundary claims are supported only through the current runtime acceptance, runnable metaprogramming conformance, and runnable metaprogramming e2e surfaces" in runbook_text,
        "runbook_mentions_public_header_fail_closed": "package loading through the public runtime header remains fail-closed until deliberate ABI widening lands" in runbook_text,
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "conformance_report_requires_all_case_ids": all(
            case_id in conformance_report.get("required_case_ids", [])
            for case_id in contract["authoritative_case_ids"]
        ),
        "conformance_report_publishes_all_surfaces": all(
            surface in conformance_report.get("required_surface_keys", []) or surface in conformance_report
            for surface in contract["canonical_surfaces"]
        ),
        "runtime_abi_cache_surface_preserves_snapshot_boundary": (
            conformance_report.get("metaprogramming_runtime_abi_cache_surface", {}).get("macro_host_process_cache_integration_snapshot_symbol")
            == "objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing"
        ),
        "runtime_abi_cache_surface_keeps_public_header_fail_closed": (
            conformance_report.get("metaprogramming_runtime_abi_cache_surface", {}).get("fail_closed_model")
            == "public-runtime-header-remains-registration-lookup-dispatch-only-and-runtime-package-loading-stays-disabled-until-deliberate-metaprogramming-runtime-abi-widening"
        ),
        "implementation_surface_carries_live_cache_case": (
            "live-metaprogramming-cache-runtime-integration"
            in conformance_report.get("metaprogramming_cache_runtime_integration_implementation_surface", {}).get("authoritative_case_ids", [])
        ),
        "stdlib_program_surface_still_marks_actor_helper_story_narrowly": "actor-shaped" in program_surface_text,
    }

    summary = {
        "issue": "metaprogramming-interop-closure-metaprogramming-runtime-semantic",
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
        "# Metaprogramming Runtime Semantic Summary\n\n"
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
