#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/lowering_runtime_artifact_contract.json"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/lowering-runtime-artifact"
JSON_OUT = OUT_DIR / "metaprogramming_interop_closure_artifact_summary.json"
MD_OUT = OUT_DIR / "metaprogramming_interop_closure_artifact_summary.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_metaprogramming_interop_closure.md"
ACCEPTANCE_PATH = ROOT / "scripts/check_objc3c_runtime_acceptance.py"
INTEGRATION_PATH = ROOT / "scripts/check_objc3c_runtime_architecture_integration.py"
CONFORMANCE_PATH = ROOT / "scripts/check_objc3c_runnable_metaprogramming_conformance.py"
E2E_PATH = ROOT / "scripts/check_objc3c_runnable_metaprogramming_end_to_end.py"
WORKFLOW_PATH = ROOT / "scripts/objc3c_public_workflow_runner.py"
FRONTEND_ARTIFACTS_PATH = ROOT / "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
RUNTIME_IMPORT_PATH = ROOT / "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp"
RUNTIME_CPP_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    acceptance_text = ACCEPTANCE_PATH.read_text(encoding="utf-8")
    integration_text = INTEGRATION_PATH.read_text(encoding="utf-8")
    conformance_text = CONFORMANCE_PATH.read_text(encoding="utf-8")
    e2e_text = E2E_PATH.read_text(encoding="utf-8")
    workflow_text = WORKFLOW_PATH.read_text(encoding="utf-8")
    frontend_artifacts_text = FRONTEND_ARTIFACTS_PATH.read_text(encoding="utf-8")
    runtime_import_text = RUNTIME_IMPORT_PATH.read_text(encoding="utf-8")
    runtime_cpp_text = RUNTIME_CPP_PATH.read_text(encoding="utf-8")

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_metaprogramming_interop_closure_artifact_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "runbook_mentions_shared_acceptance_truth": "the canonical compile-manifest and runtime-registration truth for this milestone is the shared acceptance output published by `scripts/check_objc3c_runtime_acceptance.py`" in runbook_text,
        "runbook_forbids_parallel_milestone_local_manifest_truth": "milestone-local checks must consume those emitted surfaces instead of creating parallel manifest truth" in runbook_text,
        "acceptance_builds_metaprogramming_surfaces": all(
            builder in acceptance_text
            for builder in (
                "build_runtime_metaprogramming_lowering_host_cache_surface",
                "build_runtime_cross_module_metaprogramming_artifact_preservation_surface",
                "build_runtime_metaprogramming_runtime_abi_cache_surface",
                "build_runtime_metaprogramming_cache_runtime_integration_implementation_surface",
                "build_executable_synthesized_accessor_property_lowering_surface",
            )
        ),
        "integration_replays_shared_metaprogramming_surfaces": all(surface in integration_text for surface in contract["canonical_surfaces"][:-1]),
        "conformance_preserves_metaprogramming_surface_packets": all(surface in conformance_text for surface in contract["canonical_surfaces"][:-1]),
        "e2e_preserves_packaged_metaprogramming_artifacts": all(token in e2e_text for token in ("provider_host_cache_artifact_path", "provider_runtime_import_surface_path", "consumer_link_plan_path")),
        "workflow_preserves_public_metaprogramming_commands": "validate-metaprogramming-conformance" in workflow_text and "validate-runnable-metaprogramming" in workflow_text,
        "frontend_artifacts_publish_synthesized_accessor_surface": "executable_synthesized_accessor_property_lowering_surface" in frontend_artifacts_text,
        "runtime_import_surface_preserves_property_behavior_artifact_counts": "metaprogramming_local_interface_property_behavior_artifact_count" in runtime_import_text and "metaprogramming_local_implementation_property_behavior_artifact_count" in runtime_import_text,
        "runtime_cpp_preserves_metaprogramming_cache_snapshot_symbols": "objc3_runtime_copy_metaprogramming_expansion_host_boundary_snapshot_for_testing" in runtime_cpp_text and "objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing" in runtime_cpp_text,
    }

    summary = {
        "issue": "metaprogramming-interop-closure-lowering-runtime-artifact-contract",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_artifact_count": len(contract["canonical_artifacts"]),
        "canonical_surface_count": len(contract["canonical_surfaces"]),
        "artifact_rule_count": len(contract["artifact_rules"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Metaprogramming Interop Closure Artifact Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical artifacts: `{summary['canonical_artifact_count']}`\n"
        f"- Canonical surfaces: `{summary['canonical_surface_count']}`\n"
        f"- Artifact rules: `{summary['artifact_rule_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
