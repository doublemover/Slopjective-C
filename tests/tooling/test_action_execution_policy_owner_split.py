from __future__ import annotations

from scripts.objc3c_workflow.action_execution_policy import (
    ACTION_EXECUTION_COMPLETION_OWNER,
    ACTION_EXECUTION_POLICY_OWNER,
    completion_arg_count,
    handler_args,
    should_execute_handler,
)
from scripts.objc3c_workflow.command_result_acceptance import accepted_action, unknown_action


def test_action_execution_policy_owns_handler_args_and_completion_rules() -> None:
    rest = ("sample.objc3", "--emit-ir")

    assert ACTION_EXECUTION_POLICY_OWNER == "objc3c-workflow-action-execution-policy"
    assert ACTION_EXECUTION_COMPLETION_OWNER == "objc3c-workflow-action-completion-policy"
    assert handler_args(rest) == ["sample.objc3", "--emit-ir"]
    assert completion_arg_count(rest) == 2
    assert should_execute_handler(accepted_action("compile-objc3c", 2)) is True
    assert should_execute_handler(unknown_action("missing")) is False
