from scripts.objc3c_workflow.action_acceptance_policy import (
    action_arg_count,
    action_is_registered,
    action_rejects_extra_args,
)
from scripts.objc3c_workflow.action_metadata_lookup import WorkflowActionMetadata

from action_acceptance_policy_owner_split_support import (
    passthrough_action_spec,
    strict_action_spec,
)


def assert_action_acceptance_registration_and_arg_rules() -> None:
    def handler(args: list[str]) -> int:
        return len(args)

    strict = WorkflowActionMetadata("lint", strict_action_spec(), handler)
    passthrough = WorkflowActionMetadata("compile-objc3c", passthrough_action_spec(), handler)
    assert action_is_registered(strict) is True
    assert action_is_registered(WorkflowActionMetadata("lint", None, handler)) is False
    assert action_is_registered(WorkflowActionMetadata("lint", strict.spec, None)) is False
    assert action_rejects_extra_args(strict, ["unexpected"]) is True
    assert action_rejects_extra_args(strict, []) is False
    assert action_rejects_extra_args(passthrough, ["sample.objc3"]) is False
    assert action_arg_count(["a", "b"]) == 2
