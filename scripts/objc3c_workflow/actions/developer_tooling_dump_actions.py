"""Public developer-tooling frontend dump action entrypoints."""

from __future__ import annotations

from .developer_tooling_dump_runner import run_developer_tooling_dump


def action_inspect_compile_observability(rest: list[str]) -> int:
    return run_developer_tooling_dump(
        "inspect-compile-observability",
        "--dump-observability-json",
        "compile-observability.json",
        rest,
    )


def action_inspect_runtime_inspector(rest: list[str]) -> int:
    return run_developer_tooling_dump(
        "inspect-runtime-inspector",
        "--dump-runtime-inspector-json",
        "runtime-inspector.json",
        rest,
    )


def action_trace_compile_stages(rest: list[str]) -> int:
    return run_developer_tooling_dump(
        "trace-compile-stages",
        "--dump-stage-trace-json",
        "compile-stage-trace.json",
        rest,
    )
