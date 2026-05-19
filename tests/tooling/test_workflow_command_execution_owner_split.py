from __future__ import annotations

import os
import sys

from scripts.objc3c_tooling import subprocess_command
from scripts.objc3c_tooling.subprocess_environment import python_child_environment
from scripts.objc3c_workflow import command_execution
from scripts.objc3c_workflow import composite_step_nested
from scripts.objc3c_workflow.command_execution_policy import (
    WORKFLOW_COMMAND_CWD_OWNER,
    WORKFLOW_COMMAND_EXECUTION_OWNER,
    WORKFLOW_COMMAND_EXECUTION_POLICY,
    WORKFLOW_COMMAND_OUTPUT_POLICY_OWNER,
    command_for_execution,
    workflow_command_subprocess_kwargs,
)
from scripts.objc3c_workflow.command_output import extract_output_line
from scripts.objc3c_workflow.command_output_lines import (
    COMMAND_OUTPUT_LINE_OWNER,
    COMMAND_OUTPUT_MISSING_LINE,
    extract_prefixed_value,
    normalized_output_line,
)
from scripts.objc3c_workflow.environment import ROOT


class _Completed:
    returncode = 37


def test_workflow_command_execution_policy_owns_root_and_capture_contract(monkeypatch) -> None:
    calls: list[tuple[tuple[str, ...], dict[str, object]]] = []

    def fake_run_completed(command, **kwargs):
        calls.append((tuple(command), kwargs))
        return _Completed()

    monkeypatch.setattr(command_execution, "run_completed", fake_run_completed)

    assert command_execution.run(["python", "-m", "scripts.objc3c_workflow", "lint"]) == 37
    assert calls == [
        (
            ("python", "-m", "scripts.objc3c_workflow", "lint"),
            {"cwd": ROOT, "capture_output": False},
        )
    ]
    assert WORKFLOW_COMMAND_EXECUTION_POLICY.owner == WORKFLOW_COMMAND_EXECUTION_OWNER
    assert WORKFLOW_COMMAND_EXECUTION_POLICY.cwd_owner == WORKFLOW_COMMAND_CWD_OWNER
    assert WORKFLOW_COMMAND_EXECUTION_POLICY.output_policy_owner == WORKFLOW_COMMAND_OUTPUT_POLICY_OWNER
    assert workflow_command_subprocess_kwargs() == {"cwd": ROOT, "capture_output": False}
    assert command_for_execution(["a", "b"]) == ("a", "b")


def test_python_child_environment_pins_repo_root_for_file_script_imports(
    monkeypatch,
) -> None:
    monkeypatch.setenv("PYTHONPATH", "existing-path")

    env = python_child_environment()

    assert env is not None
    pythonpath_entries = env["PYTHONPATH"].split(os.pathsep)
    assert pythonpath_entries[0] == str(ROOT)
    assert "existing-path" in pythonpath_entries
    assert env["PYTHONDONTWRITEBYTECODE"] == "1"


def test_public_workflow_commands_execute_in_process_without_shell(
    monkeypatch,
) -> None:
    calls: list[tuple[str, list[str]]] = []

    def fake_execute_nested_action(action: str, rest: list[str]) -> int:
        calls.append((action, rest))
        print("captured stdout")
        print("captured stderr", file=sys.stderr)
        return 0

    monkeypatch.setattr(
        composite_step_nested,
        "execute_nested_action",
        fake_execute_nested_action,
    )

    result = subprocess_command.run_capture(
        ["npm", "run", "objc3c", "--", "demo-action", "arg"],
        echo=False,
    )

    assert result.returncode == 0
    assert result.stdout == "captured stdout\n"
    assert result.stderr == "captured stderr\n"
    assert calls == [("demo-action", ["arg"])]


def test_workflow_command_output_line_owner_drives_facade() -> None:
    stdout = "\n  ignored: old\n  template_path: temp/template\n  harness_path: temp/harness\n"

    assert COMMAND_OUTPUT_LINE_OWNER == "objc3c-workflow-command-output-lines"
    assert normalized_output_line("  template_path: temp/template  ") == "template_path: temp/template"
    assert extract_prefixed_value(stdout, "template_path:") == "temp/template"
    assert extract_output_line(stdout, "harness_path:") == "temp/harness"
    assert extract_output_line(stdout, "missing:") == COMMAND_OUTPUT_MISSING_LINE
