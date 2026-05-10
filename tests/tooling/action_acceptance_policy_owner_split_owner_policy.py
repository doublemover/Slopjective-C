from scripts.objc3c_workflow.action_acceptance_policy import (
    ACTION_ACCEPTANCE_EXTRA_ARG_OWNER,
    ACTION_ACCEPTANCE_POLICY_OWNER,
    ACTION_ACCEPTANCE_UNKNOWN_OWNER,
)


def assert_action_acceptance_owner_policy_constants() -> None:
    assert ACTION_ACCEPTANCE_POLICY_OWNER == "objc3c-workflow-action-acceptance-policy"
    assert ACTION_ACCEPTANCE_EXTRA_ARG_OWNER == "objc3c-workflow-action-extra-argument-policy"
    assert ACTION_ACCEPTANCE_UNKNOWN_OWNER == "objc3c-workflow-action-unknown-policy"
