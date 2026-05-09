from __future__ import annotations

from scripts.objc3c_workflow.command_result_acceptance import (
    accepted_action,
    rejected_extra_args,
    unknown_action,
)
from scripts.objc3c_workflow.command_result_completion import completed_action
from scripts.objc3c_workflow.command_result_output import emit_result_error
from scripts.objc3c_workflow.command_result_policy import (
    COMMAND_RESULT_ACCEPTED_EXIT_CODE,
    COMMAND_RESULT_ERROR_STREAM_OWNER,
    COMMAND_RESULT_POLICY_OWNER,
    COMMAND_RESULT_REJECTED_EXIT_CODE,
    extra_arguments_result_message,
    unknown_action_result_message,
)
from scripts.objc3c_workflow.command_result_status import (
    REJECTED_STATUSES,
    STATUS_ACCEPTED,
    STATUS_COMPLETED,
    STATUS_EXTRA_ARGUMENTS_REJECTED,
    STATUS_FAILED,
    STATUS_UNKNOWN_ACTION,
    completion_status,
)


def test_workflow_command_result_status_owner_drives_factories() -> None:
    assert accepted_action("lint", 0).status == STATUS_ACCEPTED
    assert unknown_action("missing").status == STATUS_UNKNOWN_ACTION
    assert rejected_extra_args("lint", 1).status == STATUS_EXTRA_ARGUMENTS_REJECTED
    assert completed_action("lint", 0, 0).status == STATUS_COMPLETED
    assert completed_action("lint", 1, 0).status == STATUS_FAILED
    assert completion_status(0) == STATUS_COMPLETED
    assert completion_status(2) == STATUS_FAILED


def test_rejected_statuses_match_unaccepted_result_payloads() -> None:
    unknown = unknown_action("missing")
    rejected = rejected_extra_args("lint", 1)

    assert REJECTED_STATUSES == {
        STATUS_EXTRA_ARGUMENTS_REJECTED,
        STATUS_UNKNOWN_ACTION,
    }
    assert unknown.status in REJECTED_STATUSES
    assert rejected.status in REJECTED_STATUSES
    assert unknown.to_payload()["accepted"] is False
    assert rejected.to_payload()["accepted"] is False


def test_command_result_policy_owns_exit_codes_and_error_messages(capsys) -> None:
    unknown = unknown_action("missing")
    rejected = rejected_extra_args("lint", 1)

    assert COMMAND_RESULT_POLICY_OWNER == "objc3c-workflow-command-result-policy"
    assert COMMAND_RESULT_ERROR_STREAM_OWNER == "objc3c-workflow-command-result-stderr"
    assert accepted_action("lint", 0).exit_code == COMMAND_RESULT_ACCEPTED_EXIT_CODE
    assert unknown.exit_code == COMMAND_RESULT_REJECTED_EXIT_CODE
    assert rejected.exit_code == COMMAND_RESULT_REJECTED_EXIT_CODE
    assert unknown.message == unknown_action_result_message("missing")
    assert rejected.message == extra_arguments_result_message("lint")

    emit_result_error(unknown)
    emit_result_error(accepted_action("lint", 0))
    assert capsys.readouterr().err == "unknown action: missing\n"
