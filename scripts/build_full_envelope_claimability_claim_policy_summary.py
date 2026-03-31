#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MATRIX_CONTRACT_PATH = ROOT / "tests/tooling/fixtures/full_envelope_claimability/support_matrix_claim_taxonomy.json"
POLICY_CONTRACT_PATH = ROOT / "tests/tooling/fixtures/full_envelope_claimability/production_strength_claim_policy.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"
RELEASE_OPERATIONS_RUNBOOK = ROOT / "docs/runbooks/objc3c_release_operations.md"
OUT_DIR = ROOT / "tmp/reports/full-envelope-claimability/claim-policy"
JSON_OUT = OUT_DIR / "claim_policy_summary.json"
MD_OUT = OUT_DIR / "claim_policy_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def resolve_repo_path(raw_path: str) -> Path:
    path = Path(raw_path)
    return path if path.is_absolute() else ROOT / path


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def main() -> int:
    matrix_contract = read_json(MATRIX_CONTRACT_PATH)
    policy_contract = read_json(POLICY_CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    release_operations_text = RELEASE_OPERATIONS_RUNBOOK.read_text(encoding="utf-8")

    evidence_family_names = {
        family["family"]
        for family in matrix_contract["evidence_families"]
    }
    matrix_support_classes = {
        item["class"]
        for item in matrix_contract["support_classes"]
    }
    allowed_support_classes = {
        support_class
        for window in policy_contract["support_windows"]
        for support_class in window["allowed_support_classes"]
    }

    checks = {
        "summary_script_link_matches": policy_contract["summary_script"] == "scripts/build_full_envelope_claimability_claim_policy_summary.py",
        "runbook_mentions_production_strength_policy": "## Production-Strength Claim And Support-Window Policy" in runbook_text,
        "runbook_mentions_stable_candidate_preview_rules": all(
            marker in runbook_text for marker in ("`stable`", "`candidate`", "`preview`")
        ),
        "release_operations_runbook_mentions_support_windows": all(
            marker in release_operations_text for marker in policy_contract["required_release_operations_strings"]
        ),
        "required_evidence_families_match_matrix_contract": set(policy_contract["required_evidence_families"]) == evidence_family_names,
        "allowed_support_classes_are_defined": allowed_support_classes.issubset(matrix_support_classes),
        "all_required_public_claim_surfaces_exist": all(
            resolve_repo_path(path).exists() for path in policy_contract["required_public_claim_surfaces"]
        ),
        "all_required_runtime_boundary_runbooks_exist": all(
            resolve_repo_path(path).exists() for path in policy_contract["required_runtime_boundary_runbooks"]
        ),
        "preview_window_excludes_supported_claims": all(
            window["window"] != "preview" or "supported" not in window["allowed_support_classes"]
            for window in policy_contract["support_windows"]
        ),
    }

    payload = {
        "contract_id": "objc3c.full_envelope.claimability.production.strength.claim.policy.summary.v1",
        "source_contract_id": policy_contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_full_envelope_claimability_claim_policy_summary.py",
        "runbook": repo_rel(RUNBOOK_PATH),
        "support_window_count": len(policy_contract["support_windows"]),
        "required_evidence_family_count": len(policy_contract["required_evidence_families"]),
        "required_public_claim_surface_count": len(policy_contract["required_public_claim_surfaces"]),
        "required_runtime_boundary_runbook_count": len(policy_contract["required_runtime_boundary_runbooks"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Full-Envelope Claim Policy Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Support windows: `{payload['support_window_count']}`\n"
        f"- Required evidence families: `{payload['required_evidence_family_count']}`\n"
        f"- Required public claim surfaces: `{payload['required_public_claim_surface_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
