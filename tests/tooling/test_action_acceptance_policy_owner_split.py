from action_acceptance_policy_owner_split_command_surface import (
    assert_action_acceptance_registration_and_arg_rules,
)
from action_acceptance_policy_owner_split_owner_policy import (
    assert_action_acceptance_owner_policy_constants,
)


def test_action_acceptance_policy_owns_registration_and_arg_rules() -> None:
    assert_action_acceptance_owner_policy_constants()
    assert_action_acceptance_registration_and_arg_rules()
