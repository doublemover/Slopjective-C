#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/error_runtime_closure/error_lowering_runtime_artifact_contract.json"
OUT_DIR = ROOT / "tmp/reports/error-runtime-closure/error-lowering-runtime-artifact"
JSON_OUT = OUT_DIR / "error_lowering_runtime_artifact_summary.json"
MD_OUT = OUT_DIR / "error_lowering_runtime_artifact_summary.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_error_runtime_closure.md"
ACCEPTANCE_PATH = ROOT / "scripts/check_objc3c_runtime_acceptance.py"
INTEGRATION_PATH = ROOT / "scripts/check_objc3c_runtime_architecture_integration.py"
CONFORMANCE_PATH = ROOT / "scripts/check_objc3c_runnable_error_conformance.py"
E2E_PATH = ROOT / "scripts/check_objc3c_runnable_error_end_to_end.py"
WORKFLOW_PATH = ROOT / "scripts/objc3c_public_workflow_runner.py"
FRONTEND_ARTIFACTS_PATH = ROOT / "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
RUNTIME_IMPORT_PATH = ROOT / "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp"


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

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_error_runtime_closure_artifact_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "runbook_mentions_shared_acceptance_truth": "the canonical compile-manifest and runtime-registration surface for this milestone is the shared acceptance output published by `scripts/check_objc3c_runtime_acceptance.py`" in runbook_text,
        "acceptance_builds_error_surfaces": all(
            builder in acceptance_text
            for builder in (
                "build_runtime_error_lowering_unwind_bridge_helper_surface",
                "build_runtime_error_runtime_abi_cleanup_surface",
                "build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface",
            )
        ),
        "integration_replays_error_surfaces": all(surface in integration_text for surface in contract["canonical_surfaces"]),
        "conformance_reads_acceptance_and_integration_reports": "ACCEPTANCE_REPORT" in conformance_text and "INTEGRATION_REPORT" in conformance_text,
        "conformance_preserves_error_surface_packets": all(surface in conformance_text for surface in contract["canonical_surfaces"]),
        "e2e_preserves_packaged_error_fixture_and_probe": '"error_runtime_fixture"' in e2e_text and '"error_runtime_probe"' in e2e_text,
        "workflow_preserves_public_error_commands": "validate-error-conformance" in workflow_text and "validate-runnable-error" in workflow_text,
        "frontend_artifacts_publish_runtime_registration_surface": "runtime_registration_manifest" in frontend_artifacts_text,
        "runtime_import_surface_preserves_runtime_registration_replay": "ready_for_live_registration_discovery_replay" in runtime_import_text and "unexpected runtime registration manifest contract id" in runtime_import_text,
    }

    summary = {
        "issue": "error-runtime-closure-error-lowering-runtime-artifact-contract",
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
        "# Error Runtime Closure Artifact Summary\n\n"
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

