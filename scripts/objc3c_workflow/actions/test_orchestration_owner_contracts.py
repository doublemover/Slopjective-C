"""Owner contract checks for public test orchestration profiles."""

from __future__ import annotations

from collections.abc import Mapping

TEST_ORCHESTRATION_REQUIRED_PROFILE_OWNER_KEYS = (
    "profile_owner",
    "source_owner",
    "command_owner",
    "report_owner",
    "hard_blocking_decision_owner",
)

TEST_ORCHESTRATION_REQUIRED_STEP_OWNER_KEYS = (
    "action",
    "source_owner",
    "command_owner",
    "hard_blocking_decision_owner",
)


def require_test_orchestration_profile_payload(
    payload: Mapping[str, object],
) -> None:
    missing_profile_owners = [
        key
        for key in TEST_ORCHESTRATION_REQUIRED_PROFILE_OWNER_KEYS
        if not isinstance(payload.get(key), str) or not payload.get(key)
    ]
    if missing_profile_owners:
        raise ValueError(
            "test orchestration profile is missing owner keys: "
            + ", ".join(sorted(missing_profile_owners))
        )

    steps = payload.get("steps")
    if not isinstance(steps, list) or not steps:
        raise ValueError("test orchestration profile is missing owner-tagged steps")
    for index, step in enumerate(steps, start=1):
        if not isinstance(step, Mapping):
            raise ValueError(f"test orchestration step {index} is not a mapping")
        missing_step_owners = [
            key
            for key in TEST_ORCHESTRATION_REQUIRED_STEP_OWNER_KEYS
            if not isinstance(step.get(key), str) or not step.get(key)
        ]
        if missing_step_owners:
            raise ValueError(
                f"test orchestration step {index} is missing owner keys: "
                + ", ".join(sorted(missing_step_owners))
            )


def require_test_orchestration_profile_payloads(
    payloads: Mapping[str, Mapping[str, object]],
) -> None:
    if not payloads:
        raise ValueError("test orchestration profile catalog is empty")
    for payload in payloads.values():
        require_test_orchestration_profile_payload(payload)
