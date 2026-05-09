from __future__ import annotations

from scripts.objc3c_workflow.cli_argv import process_argv, workflow_argv
from scripts.objc3c_workflow.cli_argv_source import process_argv as owned_process_argv


def test_cli_argv_facade_uses_process_argv_owner() -> None:
    assert process_argv is owned_process_argv
    assert workflow_argv(["lint", "--dry"]) == ["lint", "--dry"]
