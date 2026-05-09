from __future__ import annotations

from scripts.objc3c_workflow.progress import format_command
from scripts.objc3c_workflow.progress_format import format_command as owned_format_command


def test_progress_facade_exports_owned_command_formatter() -> None:
    assert format_command is owned_format_command
    assert format_command(["python", "-m", "scripts.objc3c_workflow", "lint"]) == (
        "python -m scripts.objc3c_workflow lint"
    )
