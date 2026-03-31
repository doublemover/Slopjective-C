#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/runtime_corrective/lowering_provenance_artifact_contract.json"
OUT_DIR = ROOT / "tmp/reports/runtime-corrective/lowering-provenance-artifact"
JSON_OUT = OUT_DIR / "lowering_provenance_artifact_contract_summary.json"
MD_OUT = OUT_DIR / "lowering_provenance_artifact_contract_summary.md"
COMPILE_SCRIPT_PATH = ROOT / "scripts/objc3c_native_compile.ps1"
REPLAY_PROOF_PATH = ROOT / "scripts/check_objc3c_execution_replay_proof.ps1"
HARNESS_PATH = ROOT / "scripts/shared_compiler_runtime_acceptance_harness.py"
ACCEPTANCE_SCRIPT_PATH = ROOT / "scripts/check_objc3c_runtime_acceptance.py"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_runtime_corrective.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def count_literal(text: str, needle: str) -> int:
    return text.count(needle)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    compile_text = COMPILE_SCRIPT_PATH.read_text(encoding="utf-8")
    replay_text = REPLAY_PROOF_PATH.read_text(encoding="utf-8")
    harness_text = HARNESS_PATH.read_text(encoding="utf-8")
    acceptance_text = ACCEPTANCE_SCRIPT_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    checks = {
        "runbook_link_matches": contract["runbook"] == "docs/runbooks/objc3c_runtime_corrective.md",
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_runtime_corrective_lowering_provenance_summary.py",
        "all_generation_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_generation_paths"]),
        "compile_script_emits_registration_manifest": "runtime-registration-manifest.json" in compile_text,
        "compile_script_emits_compile_provenance": ".compile-provenance.json" in compile_text,
        "compile_script_exposes_object_backend_selection": "--objc3-ir-object-backend" in compile_text,
        "replay_proof_requires_required_artifacts": all(artifact in replay_text for artifact in contract["required_artifacts"][1:3]),
        "replay_proof_requires_truthfulness_contract": contract["compile_output_truthfulness_contract_id"] in replay_text,
        "acceptance_surface_uses_truth_contracts": contract["compile_output_truthfulness_contract_id"] in acceptance_text and contract["compile_output_provenance_contract_id"] in acceptance_text,
        "harness_carries_shared_compile_truth_contracts": "shared_compile_truth_contracts" in harness_text and "COMPILE_PROVENANCE_CONTRACT_ID" in harness_text and "COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID" in harness_text,
        "acceptance_surface_mentions_lowering_contract_ids": all(contract_id in acceptance_text for contract_id in contract["lowering_surface_contract_ids"]),
        "runbook_mentions_lowering_provenance_contract": "lowering_provenance_artifact_contract.json" in runbook_text,
        "non_goals_reject_sidecar_only_lowering": "no-sidecar-only-lowering-proof" in contract["explicit_non_goals"],
    }

    measured_inventory = {
        "authoritative_generation_path_count": len(contract["authoritative_generation_paths"]),
        "required_artifact_count": len(contract["required_artifacts"]),
        "required_bundle_binding_count": len(contract["required_bundle_bindings"]),
        "lowering_surface_contract_count": len(contract["lowering_surface_contract_ids"]),
        "evidence_coupling_clause_count": len(contract["evidence_coupling_model"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "compile_provenance_occurrences_in_compile_script": count_literal(compile_text, "compile-provenance"),
        "registration_manifest_occurrences_in_compile_script": count_literal(compile_text, "runtime-registration-manifest"),
        "truthfulness_occurrences_across_replay_and_harness": count_literal(replay_text, "truthfulness") + count_literal(harness_text, "truthfulness"),
    }

    summary = {
        "issue": "runtime-corrective-lowering-provenance-artifact",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "measured_inventory": measured_inventory,
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Runtime Corrective Lowering Provenance Artifact Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Authoritative generation paths: `{measured_inventory['authoritative_generation_path_count']}`\n"
        f"- Required artifacts: `{measured_inventory['required_artifact_count']}`\n"
        f"- Required bundle bindings: `{measured_inventory['required_bundle_binding_count']}`\n"
        f"- Lowering surface contracts: `{measured_inventory['lowering_surface_contract_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
