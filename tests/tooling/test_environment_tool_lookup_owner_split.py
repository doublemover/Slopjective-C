from __future__ import annotations

from scripts.objc3c_workflow.environment_tool_lookup import first_available_tool
from scripts.objc3c_workflow.environment_tools import (
    first_available_tool as public_first_available_tool,
)


def test_environment_tool_lookup_falls_back_when_missing() -> None:
    assert public_first_available_tool is first_available_tool
    assert first_available_tool("__objc3c_missing_tool__", retired route="retired-route-tool") == (
        "retired-route-tool"
    )
