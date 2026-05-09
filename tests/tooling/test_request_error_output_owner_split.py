from __future__ import annotations

from scripts.objc3c_workflow.argument_usage_error import WorkflowUsageError
from scripts.objc3c_workflow.request_error_output import emit_request_error
from scripts.objc3c_workflow.request_error_policy import (
    DEFAULT_REQUEST_ERROR_EXIT_CODE,
    REQUEST_ERROR_EXIT_CODE_OWNER,
    REQUEST_ERROR_OUTPUT_OWNER,
    REQUEST_ERROR_STREAM_OWNER,
    request_error_exit_code,
)
from scripts.objc3c_workflow.request_unknown_errors import emit_unknown_action
from scripts.objc3c_workflow.request_usage_errors import emit_usage_error


def test_request_error_output_policy_owns_default_stderr_exit(capsys) -> None:
    assert REQUEST_ERROR_OUTPUT_OWNER == "objc3c-workflow-request-error-output"
    assert REQUEST_ERROR_STREAM_OWNER == "objc3c-workflow-request-error-stderr"
    assert REQUEST_ERROR_EXIT_CODE_OWNER == "objc3c-workflow-request-error-exit-code"
    assert DEFAULT_REQUEST_ERROR_EXIT_CODE == 2
    assert request_error_exit_code(None) == DEFAULT_REQUEST_ERROR_EXIT_CODE
    assert request_error_exit_code(7) == 7

    assert emit_request_error("bad request") == DEFAULT_REQUEST_ERROR_EXIT_CODE
    captured = capsys.readouterr()
    assert captured.out == ""
    assert captured.err == "bad request\n"


def test_request_error_facades_delegate_to_owned_output_policy(capsys) -> None:
    assert emit_unknown_action("missing") == DEFAULT_REQUEST_ERROR_EXIT_CODE
    assert emit_usage_error(WorkflowUsageError("bad usage", 9)) == 9

    captured = capsys.readouterr()
    assert captured.out == ""
    assert captured.err == "unknown action: missing\nbad usage\n"
