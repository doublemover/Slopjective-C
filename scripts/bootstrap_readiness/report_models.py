from __future__ import annotations

from typing import Any, TypeAlias, TypedDict


class CommandSummary(TypedDict):
    argv: list[str]
    exit_code: int
    stdout_bytes: int
    stderr_bytes: int


SummaryPayload: TypeAlias = dict[str, Any]
