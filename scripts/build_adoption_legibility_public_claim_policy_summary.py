#!/usr/bin/env python3
"""Build the adoption public claim policy summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file


ROOT = Path(__file__).resolve().parents[1]
POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "public_claim_policy.json"
BOUNDARY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "boundary_inventory.json"
RELEASE_UPGRADE_CLAIM_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "upgrade_support_claim_policy.json"
PERFORMANCE_CLAIM_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "claim_policy.json"
LONG_HORIZON_DEPRECATION_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "long_horizon_operations" / "deprecation_support_policy.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "adoption-legibility" / "public-claim-policy-summary.json"




def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    policy = load_json(POLICY_PATH)
    boundary = load_json(BOUNDARY_PATH)
    release_upgrade_claim_policy = load_json(RELEASE_UPGRADE_CLAIM_POLICY)
    performance_claim_policy = load_json(PERFORMANCE_CLAIM_POLICY)
    long_horizon_policy = load_json(LONG_HORIZON_DEPRECATION_POLICY)
    failures: list[str] = []

    dependencies = [str(path) for path in policy.get("depends_on", [])]
    for raw_path in dependencies:
        expect((ROOT / raw_path).is_file(), f"missing dependency {raw_path}", failures)

    claim_classes = policy.get("claim_classes", [])
    expect(isinstance(claim_classes, list) and len(claim_classes) >= 3, "claim class set is too narrow", failures)
    boundary_surfaces = {
        str(path)
        for key in ("primary_evaluator_surfaces", "substrate_runbooks", "substrate_fixture_surfaces")
        for path in boundary.get(key, [])
    }
    for entry in claim_classes if isinstance(claim_classes, list) else []:
        if not isinstance(entry, dict):
            failures.append("claim class entry must be an object")
            continue
        claim_id = str(entry.get("claim_id"))
        expect(bool(entry.get("support_class")), f"{claim_id} missing support_class", failures)
        expect(bool(entry.get("allowed_scope")), f"{claim_id} missing allowed_scope", failures)
        required_evidence = entry.get("requires_evidence", [])
        expect(isinstance(required_evidence, list) and len(required_evidence) >= 3, f"{claim_id} evidence set is too narrow", failures)
        required_surfaces = [str(path) for path in entry.get("required_reference_surfaces", [])]
        expect(len(required_surfaces) >= 3, f"{claim_id} must name concrete reference surfaces", failures)
        for raw_path in required_surfaces:
            expect((ROOT / raw_path).is_file(), f"{claim_id} references missing surface {raw_path}", failures)
            expect(raw_path in boundary_surfaces, f"{claim_id} surface {raw_path} is not in the boundary inventory", failures)

    forbidden_claims = [str(claim).lower() for claim in policy.get("forbidden_claims", [])]
    release_forbidden = [str(claim).lower() for claim in release_upgrade_claim_policy.get("forbidden_claims", [])]
    expect(any("cross-major" in claim for claim in forbidden_claims + release_forbidden), "cross-major conversion must fail closed", failures)
    expect(any("forever compatible" in claim for claim in forbidden_claims), "forever compatibility must be forbidden", failures)
    expect(any("performance" in claim for claim in forbidden_claims), "performance overclaim guardrail missing", failures)

    performance_statuses = performance_claim_policy.get("claim_statuses", [])
    expect(
        any(isinstance(status, dict) and status.get("status") == "release-ready" for status in performance_statuses),
        "performance governance release-ready status missing",
        failures,
    )
    support_policy = long_horizon_policy.get("support_policy", {})
    expect(isinstance(support_policy, dict) and bool(support_policy), "long-horizon support policy missing", failures)
    required_publication_fields = policy.get("required_publication_fields", [])
    expect(
        {"claim_id", "support_class", "allowed_scope", "evidence_paths"}.issubset(set(required_publication_fields)),
        "publication fields do not identify claim, scope, support class, and evidence paths",
        failures,
    )
    deferred_policy = policy.get("deferred_behavior_policy", {})
    expect(
        isinstance(deferred_policy, dict) and deferred_policy.get("claim_status_when_deferred_behavior_is_unnamed") == "blocked",
        "deferred behavior policy must block unnamed deferred behavior",
        failures,
    )

    payload = {
        "contract_id": "objc3c.adoption_legibility.public_claim_policy.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "policy": repo_rel(POLICY_PATH),
        "boundary_inventory": repo_rel(BOUNDARY_PATH),
        "release_upgrade_claim_policy": repo_rel(RELEASE_UPGRADE_CLAIM_POLICY),
        "performance_claim_policy": repo_rel(PERFORMANCE_CLAIM_POLICY),
        "long_horizon_deprecation_policy": repo_rel(LONG_HORIZON_DEPRECATION_POLICY),
        "claim_class_count": len(claim_classes) if isinstance(claim_classes, list) else 0,
        "fail_closed_condition_count": len(policy.get("fail_closed_conditions", [])),
        "forbidden_claim_count": len(policy.get("forbidden_claims", [])),
        "required_publication_fields": required_publication_fields,
        "support_classes": [entry.get("support_class") for entry in claim_classes if isinstance(entry, dict)],
        "forbidden_claims": policy.get("forbidden_claims", []),
        "deferred_behavior_policy": deferred_policy,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("adoption-legibility-public-claim-policy: PASS" if not failures else "adoption-legibility-public-claim-policy: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
