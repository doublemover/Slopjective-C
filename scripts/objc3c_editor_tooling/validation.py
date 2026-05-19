from __future__ import annotations

from typing import Any

from objc3c_editor_tooling.input_loading import CompileInvocationResult


def compile_summary_exit_code(result: CompileInvocationResult) -> int | None:
    if result.summary_available:
        return None
    return result.returncode if result.returncode != 0 else 1


def diagnostics_entries(diagnostics_payload: dict[str, Any]) -> list[Any]:
    entries = diagnostics_payload.get("diagnostics", [])
    return entries if isinstance(entries, list) else []
