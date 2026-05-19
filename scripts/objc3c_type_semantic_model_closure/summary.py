from __future__ import annotations

from typing import Any

from objc3c_type_semantic_model_closure.summary_checks import compile_summary_checks
from objc3c_type_semantic_model_closure.summary_payload import build_summary_payload
from objc3c_type_semantic_model_closure.summary_runs import compile_runtime_runs


def build_summary() -> dict[str, Any]:
    runs = compile_runtime_runs()
    model, checks, static_presence = compile_summary_checks(runs)
    status = "PASS" if all(checks.values()) else "FAIL"
    return build_summary_payload(
        runs=runs,
        model=model,
        checks=checks,
        static_presence=static_presence,
        status=status,
    )


__all__ = ["build_summary"]
