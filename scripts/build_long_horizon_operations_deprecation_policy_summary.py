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

EXPECTED_DEPRECATION_STATES = {
    "active",
    "soft-deprecated",
    "hard-deprecated",
    "removed",
}

EXPECTED_FALSE_CLAIM_STATES = {
    "hard-deprecated",
    "removed",
}

FALSE_CLAIM_EVIDENCE_FIELDS = {
    "conversion_replay_evidence",
    "revert_evidence",
}

HARD_TRANSITION_EVIDENCE = {
    "generated-conversion-replay",
    "generated-revert-evidence",
}




def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def warning_severity_map(channel_policy: dict[str, Any]) -> dict[str, str]:
    return {
        str(entry.get("warning_id")): str(entry.get("severity"))
        for entry in channel_policy.get("warning_classes", [])
        if isinstance(entry, dict) and isinstance(entry.get("warning_id"), str)
    }


def validate_deprecation_states(
    policy: dict[str, Any],
    channel_policy: dict[str, Any],
    failures: list[str],
) -> tuple[list[dict[str, Any]], set[str], set[str]]:
    warning_severities = warning_severity_map(channel_policy)
    warning_ids = set(warning_severities)
    warning_ids.add("none")
    deprecation_states = policy.get("deprecation_states", [])
    expect(
        isinstance(deprecation_states, list)
        and len(deprecation_states) >= len(EXPECTED_DEPRECATION_STATES),
        "deprecation state set is too narrow",
        failures,
    )

    state_names: set[str] = set()
    false_claim_states: set[str] = set()
    for entry in deprecation_states if isinstance(deprecation_states, list) else []:
        if not isinstance(entry, dict):
            failures.append("deprecation state entry must be an object")
            continue
        state = str(entry.get("state"))
        state_names.add(state)
        warning_class = str(entry.get("required_warning_class"))
        public_claim_allowed = entry.get("public_claim_allowed")
        expect(warning_class in warning_ids, f"unknown warning class {warning_class}", failures)
        expect("operator_action" in entry, f"state {state} missing operator_action", failures)
        if state == "active":
            expect(public_claim_allowed is True, "active state must allow public claims", failures)
            expect(warning_class == "none", "active state must not require warnings", failures)
        if state == "soft-deprecated":
            expect(public_claim_allowed is True, "soft-deprecated must keep claims warning-gated", failures)
            expect(
                warning_severities.get(warning_class) == "warn",
                "soft-deprecated must use warning severity",
                failures,
            )
        if state in EXPECTED_FALSE_CLAIM_STATES:
            expect(
                public_claim_allowed is False,
                f"{state} must block public claims",
                failures,
            )
            false_claim_states.add(state)
        elif public_claim_allowed is False:
            false_claim_states.add(state)
            expect(
                warning_severities.get(warning_class) == "error",
                f"{state} must use an error warning class",
                failures,
            )

        if state in false_claim_states:
            expect(
                warning_severities.get(warning_class) == "error",
                f"{state} must use an error warning class",
                failures,
            )

    missing_states = sorted(EXPECTED_DEPRECATION_STATES - state_names)
    expect(not missing_states, f"missing deprecation states: {', '.join(missing_states)}", failures)
    return (
        deprecation_states if isinstance(deprecation_states, list) else [],
        state_names,
        false_claim_states,
    )


def validate_publication_requirements(
    policy: dict[str, Any],
    state_names: set[str],
    false_claim_states: set[str],
    failures: list[str],
) -> tuple[list[str], list[dict[str, Any]]]:
    required_fields = policy.get("required_publication_fields", [])
    expect(
        isinstance(required_fields, list) and required_fields,
        "required_publication_fields must be non-empty",
        failures,
    )
    required_field_set = {
        str(field) for field in required_fields if isinstance(field, str) and field
    }
    field_requirements = policy.get("publication_field_requirements", [])
    expect(
        isinstance(field_requirements, list) and field_requirements,
        "publication_field_requirements must be non-empty",
        failures,
    )
    requirement_by_field: dict[str, set[str]] = {}
    for entry in field_requirements if isinstance(field_requirements, list) else []:
        if not isinstance(entry, dict):
            failures.append("publication field requirement entry must be an object")
            continue
        field = entry.get("field")
        states = entry.get("required_for_states")
        if not isinstance(field, str) or not field:
            failures.append("publication field requirement missing field")
            continue
        if not isinstance(states, list) or not states:
            failures.append(f"publication field {field} missing required_for_states")
            continue
        resolved_states = {str(state) for state in states}
        unknown_states = sorted(resolved_states - state_names)
        expect(
            not unknown_states,
            f"publication field {field} references unknown states: {', '.join(unknown_states)}",
            failures,
        )
        requirement_by_field[field] = resolved_states

    missing_requirements = sorted(required_field_set - set(requirement_by_field))
    expect(
        not missing_requirements,
        f"required publication fields lack requirements: {', '.join(missing_requirements)}",
        failures,
    )
    for evidence_field in FALSE_CLAIM_EVIDENCE_FIELDS:
        covered_states = requirement_by_field.get(evidence_field, set())
        missing_false_states = sorted(false_claim_states - covered_states)
        expect(
            not missing_false_states,
            f"{evidence_field} does not cover false-claim states: {', '.join(missing_false_states)}",
            failures,
        )
    return (
        required_fields if isinstance(required_fields, list) else [],
        field_requirements if isinstance(field_requirements, list) else [],
    )


def validate_transition_policy(
    policy: dict[str, Any],
    state_names: set[str],
    false_claim_states: set[str],
    failures: list[str],
) -> dict[str, Any]:
    transition_policy = policy.get("state_transition_policy")
    expect(
        isinstance(transition_policy, dict),
        "state_transition_policy must be an object",
        failures,
    )
    if not isinstance(transition_policy, dict):
        return {}

    allowed_transitions = transition_policy.get("allowed_transitions", [])
    expect(
        isinstance(allowed_transitions, list) and allowed_transitions,
        "allowed_transitions must be non-empty",
        failures,
    )
    transition_keys: set[str] = set()
    for entry in allowed_transitions if isinstance(allowed_transitions, list) else []:
        if not isinstance(entry, dict):
            failures.append("allowed transition entry must be an object")
            continue
        source = str(entry.get("from"))
        target = str(entry.get("to"))
        evidence = entry.get("required_evidence")
        transition_keys.add(f"{source}->{target}")
        expect(source in state_names, f"transition source {source} is unknown", failures)
        expect(target in state_names, f"transition target {target} is unknown", failures)
        expect(
            isinstance(evidence, list) and all(isinstance(item, str) for item in evidence),
            f"transition {source}->{target} missing required_evidence",
            failures,
        )
        if target in false_claim_states:
            missing = sorted(HARD_TRANSITION_EVIDENCE - set(evidence or []))
            expect(
                not missing,
                f"transition {source}->{target} misses hard-state evidence: {', '.join(missing)}",
                failures,
            )

    non_waivable_states = {
        str(state) for state in transition_policy.get("non_waivable_states", [])
    }
    missing_non_waivable = sorted(false_claim_states - non_waivable_states)
    expect(
        not missing_non_waivable,
        f"false-claim states are waiverable: {', '.join(missing_non_waivable)}",
        failures,
    )
    forbidden_transitions = {
        str(transition) for transition in transition_policy.get("forbidden_transitions", [])
    }
    illegal_overlap = sorted(transition_keys & forbidden_transitions)
    expect(
        not illegal_overlap,
        f"forbidden transitions are also allowed: {', '.join(illegal_overlap)}",
        failures,
    )
    return transition_policy


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

    deprecation_states, state_names, false_claim_states = validate_deprecation_states(
        policy,
        channel_policy,
        failures,
    )
    required_publication_fields, publication_field_requirements = (
        validate_publication_requirements(
            policy,
            state_names,
            false_claim_states,
            failures,
        )
    )
    transition_policy = validate_transition_policy(
        policy,
        state_names,
        false_claim_states,
        failures,
    )

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
        "false_claim_states": sorted(false_claim_states),
        "warning_class_count": len(warning_severity_map(channel_policy)),
        "forbidden_claim_count": len(forbidden_claims) if isinstance(forbidden_claims, list) else 0,
        "required_publication_fields": required_publication_fields,
        "publication_field_requirement_count": (
            len(publication_field_requirements)
            if isinstance(publication_field_requirements, list)
            else 0
        ),
        "allowed_transition_count": len(
            transition_policy.get("allowed_transitions", [])
            if isinstance(transition_policy, dict)
            else []
        ),
        "non_waivable_states": sorted(
            transition_policy.get("non_waivable_states", [])
            if isinstance(transition_policy, dict)
            else []
        ),
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
