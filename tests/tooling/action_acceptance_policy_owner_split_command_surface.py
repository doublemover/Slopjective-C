from scripts.objc3c_workflow.action_acceptance_policy import (
    action_arg_count,
    action_is_registered,
    action_rejects_extra_args,
)

from action_acceptance_policy_owner_split_support import (
    passthrough_action_spec,
    strict_action_spec,
)


def assert_action_acceptance_registration_and_arg_rules() -> None:
    strict_spec = strict_action_spec()
    passthrough_spec = passthrough_action_spec()

    assert action_is_registered(strict_spec, object()) is True
    assert action_is_registered(None, object()) is False
    assert action_is_registered(strict_spec, None) is False
    assert action_rejects_extra_args(strict_spec, ["unexpected"]) is True
    assert action_rejects_extra_args(strict_spec, []) is False
    assert action_rejects_extra_args(passthrough_spec, ["sample.objc3"]) is False
    assert action_arg_count(["a", "b"]) == 2
