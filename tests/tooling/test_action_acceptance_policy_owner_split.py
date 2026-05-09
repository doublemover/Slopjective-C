from __future__ import annotations

from scripts.objc3c_workflow.action_acceptance_policy import (
    ACTION_ACCEPTANCE_EXTRA_ARG_OWNER,
    ACTION_ACCEPTANCE_POLICY_OWNER,
    ACTION_ACCEPTANCE_UNKNOWN_OWNER,
    action_arg_count,
    action_is_registered,
    action_rejects_extra_args,
)
from scripts.objc3c_workflow.action_spec import ActionSpec


def test_action_acceptance_policy_owns_registration_and_arg_rules() -> None:
    strict_spec = ActionSpec("lint", "Core", "Lint", "npm run objc3c -- lint")
    passthrough_spec = ActionSpec(
        "compile-objc3c",
        "Compiler",
        "Compile",
        "npm run objc3c -- compile-objc3c -- <source>",
        pass_through_args=True,
    )

    assert ACTION_ACCEPTANCE_POLICY_OWNER == "objc3c-workflow-action-acceptance-policy"
    assert ACTION_ACCEPTANCE_EXTRA_ARG_OWNER == "objc3c-workflow-action-extra-argument-policy"
    assert ACTION_ACCEPTANCE_UNKNOWN_OWNER == "objc3c-workflow-action-unknown-policy"
    assert action_is_registered(strict_spec, object()) is True
    assert action_is_registered(None, object()) is False
    assert action_is_registered(strict_spec, None) is False
    assert action_rejects_extra_args(strict_spec, ["unexpected"]) is True
    assert action_rejects_extra_args(strict_spec, []) is False
    assert action_rejects_extra_args(passthrough_spec, ["sample.objc3"]) is False
    assert action_arg_count(["a", "b"]) == 2
