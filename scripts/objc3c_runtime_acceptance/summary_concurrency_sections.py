"""Concurrency summary sections."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_catalog import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.case_result import CaseResult


def build_concurrency_summary_sections(
    *,
    results: list[CaseResult],
    domains: RuntimeAcceptanceDomains,
) -> dict[str, Any]:
    return {
        "runtime_unified_concurrency_source_surface": (
            domains.concurrency.build_runtime_unified_concurrency_source_surface(
                results
            )
        ),
        "runtime_async_task_actor_normalization_completion_surface": (
            domains.concurrency.build_runtime_async_task_actor_normalization_completion_surface(
                results
            )
        ),
        "runtime_unified_concurrency_lowering_metadata_surface": (
            domains.concurrency.build_runtime_unified_concurrency_lowering_metadata_surface(
                results
            )
        ),
        "runtime_unified_concurrency_runtime_abi_surface": (
            domains.concurrency.build_runtime_unified_concurrency_runtime_abi_surface(
                results
            )
        ),
    }


__all__ = ["build_concurrency_summary_sections"]
