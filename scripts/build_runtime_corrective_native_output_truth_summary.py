#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/runtime_corrective/native_output_truth_policy.json"
OUT_DIR = ROOT / "tmp/reports/m316/M316-B003"
JSON_OUT = OUT_DIR / "native_output_truth_policy_summary.json"
MD_OUT = OUT_DIR / "native_output_truth_policy_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNTIME_README_PATH = ROOT / "tests/tooling/runtime/README.md"
ACCEPTANCE_SCRIPT_PATH = ROOT / "scripts/check_objc3c_runtime_acceptance.py"
REPLAY_PROOF_PATH = ROOT / "scripts/check_objc3c_execution_replay_proof.ps1"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_runtime_corrective.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def count_literal(text: str, needle: str) -> int:
    return text.count(needle)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    runtime_readme_text = RUNTIME_README_PATH.read_text(encoding="utf-8")
    acceptance_text = ACCEPTANCE_SCRIPT_PATH.read_text(encoding="utf-8")
    replay_proof_text = REPLAY_PROOF_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    checks = {
        "runbook_link_matches": contract["runbook"] == "docs/runbooks/objc3c_runtime_corrective.md",
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_runtime_corrective_native_output_truth_summary.py",
        "acceptance_surface_uses_truthfulness_contract_id": contract["compile_output_truthfulness_contract_id"] in acceptance_text,
        "acceptance_surface_uses_provenance_contract_id": contract["compile_output_provenance_contract_id"] in acceptance_text,
        "replay_proof_requires_truthfulness_envelope": "missing compile output truthfulness envelope" in replay_proof_text,
        "replay_proof_requires_truthful_compile_output": "compile output truthfulness envelope did not certify emitted artifacts" in replay_proof_text,
        "replay_proof_requires_registration_manifest_truth_binding": "runtime registration manifest missing compile output truthfulness contract id" in replay_proof_text,
        "replay_proof_requires_coupled_provenance_binding": "runtime registration manifest missing compile provenance binding" in replay_proof_text,
        "docs_publish_coupled_native_proof_requirement": "native proof remains invalid unless the emitted object, manifest, and linked runtime probe all come from the same reproducible compile path" in doc_text,
        "runtime_readme_rejects_sidecar_only_proof": "sidecar-only evidence with no matching executable compile or probe path" in runtime_readme_text,
        "runbook_mentions_native_output_truth_policy": "native_output_truth_policy.json" in runbook_text,
        "forbidden_proof_sources_are_narrowing_claims": "hand-authored-ll" in contract["forbidden_proof_sources"],
    }

    measured_inventory = {
        "required_coupled_artifact_count": len(contract["required_coupled_artifacts"]),
        "required_truth_binding_count": len(contract["required_truth_bindings"]),
        "authoritative_claim_class_count": len(contract["authoritative_claim_classes"]),
        "allowed_proof_source_count": len(contract["allowed_proof_sources"]),
        "forbidden_proof_source_count": len(contract["forbidden_proof_sources"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "runtime_registration_manifest_occurrences_in_replay_proof": count_literal(replay_proof_text, "runtime-registration-manifest"),
        "compile_output_truthfulness_occurrences_in_replay_proof": count_literal(replay_proof_text, "compile output truthfulness"),
        "sidecar_only_occurrences_across_claim_surfaces": count_literal(doc_text, "sidecar-only")
        + count_literal(runtime_readme_text, "sidecar-only")
        + count_literal(acceptance_text, "sidecar-only"),
    }

    summary = {
        "issue": "M316-B003",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "measured_inventory": measured_inventory,
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M316-B003 Native Output Truth Policy Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Required coupled artifacts: `{measured_inventory['required_coupled_artifact_count']}`\n"
        f"- Required truth bindings: `{measured_inventory['required_truth_binding_count']}`\n"
        f"- Allowed proof sources: `{measured_inventory['allowed_proof_source_count']}`\n"
        f"- Forbidden proof sources: `{measured_inventory['forbidden_proof_source_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
