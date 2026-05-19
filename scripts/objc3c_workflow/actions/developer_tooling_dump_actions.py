"""Public developer-tooling frontend dump action entrypoints."""

from __future__ import annotations

from .developer_tooling_dump_contracts import (
    COMPILE_OBSERVABILITY_DUMP,
    COMPILE_STAGE_TRACE_DUMP,
    RUNTIME_INSPECTOR_DUMP,
)
from .developer_tooling_dump_runner import run_developer_tooling_dump


def action_inspect_compile_observability(rest: list[str]) -> int:
    return run_developer_tooling_dump(COMPILE_OBSERVABILITY_DUMP, rest)


def action_inspect_runtime_inspector(rest: list[str]) -> int:
    return run_developer_tooling_dump(RUNTIME_INSPECTOR_DUMP, rest)


def action_trace_compile_stages(rest: list[str]) -> int:
    return run_developer_tooling_dump(COMPILE_STAGE_TRACE_DUMP, rest)
