#!/usr/bin/env python3
"""Build the long-horizon deprecation support policy summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file


ROOT = Path(__file__).resolve().parents[1]
POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "long_horizon_operations" / "deprecation_support_policy.json"
VERSIONING_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "versioning_model.json"
UPGRADE_CLAIM_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "upgrade_support_claim_policy.json"
UPDATE_CHANNEL_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "update_channel_policy.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "long-horizon-operations" / "deprecation-support-policy-summary.json"




def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    policy = load_json(POLICY_PATH)
    versioning = load_json(VERSIONING_MODEL)
    claim_policy = load_json(UPGRADE_CLAIM_POLICY)
    channel_policy = load_json(UPDATE_CHANNEL_POLICY)
    failures: list[str] = []

    dependencies = [str(path) for path in policy.get("depends_on", [])]
    for raw_path in dependencies:
        expect((ROOT / raw_path).is_file(), f"missing dependency {raw_path}", failures)

    support_policy = policy.get("support_policy", {})
    expect(isinstance(support_policy, dict), "support_policy must be an object", failures)
    expect(
        support_policy.get("current_support_authority") == repo_rel(VERSIONING_MODEL),
        "support authority drifted from release operations versioning model",
        failures,
    )
    expect(versioning.get("supported_major_line") is not None, "versioning model missing supported_major_line", failures)
    support_windows = versioning.get("support_windows")
    expect(isinstance(support_windows, dict) and bool(support_windows), "versioning model missing support windows", failures)

    warning_ids = {str(entry.get("warning_id")) for entry in channel_policy.get("warning_classes", []) if isinstance(entry, dict)}
    warning_ids.add("none")
    deprecation_states = policy.get("deprecation_states", [])
    expect(isinstance(deprecation_states, list) and len(deprecation_states) >= 4, "deprecation state set is too narrow", failures)
    for entry in deprecation_states if isinstance(deprecation_states, list) else []:
        if not isinstance(entry, dict):
            failures.append("deprecation state entry must be an object")
            continue
        expect(str(entry.get("required_warning_class")) in warning_ids, f"unknown warning class {entry.get('required_warning_class')}", failures)
        expect("operator_action" in entry, f"state {entry.get('state')} missing operator_action", failures)

    forbidden_claims = claim_policy.get("forbidden_claims", [])
    expect(isinstance(forbidden_claims, list) and forbidden_claims, "release upgrade claim policy missing forbidden claims", failures)
    blocking_conditions = policy.get("release_blocking_conditions", [])
    expect(isinstance(blocking_conditions, list) and len(blocking_conditions) >= 4, "release-blocking conditions are too narrow", failures)

    payload = {
        "contract_id": "objc3c.long_horizon_operations.deprecation_support_policy.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "policy": repo_rel(POLICY_PATH),
        "versioning_model": repo_rel(VERSIONING_MODEL),
        "upgrade_claim_policy": repo_rel(UPGRADE_CLAIM_POLICY),
        "update_channel_policy": repo_rel(UPDATE_CHANNEL_POLICY),
        "supported_major_line": versioning.get("supported_major_line"),
        "support_window_count": len(support_windows) if isinstance(support_windows, dict) else 0,
        "deprecation_state_count": len(deprecation_states) if isinstance(deprecation_states, list) else 0,
        "warning_class_count": len(warning_ids - {"none"}),
        "forbidden_claim_count": len(forbidden_claims) if isinstance(forbidden_claims, list) else 0,
        "required_publication_fields": policy.get("required_publication_fields", []),
        "claim_guardrails": policy.get("claim_guardrails", []),
        "release_blocking_conditions": blocking_conditions,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("long-horizon-deprecation-policy: PASS" if not failures else "long-horizon-deprecation-policy: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
