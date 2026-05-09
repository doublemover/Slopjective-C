"""Runtime acceptance summary assembly."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.case_catalog import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.progress_state import RuntimeAcceptanceProgress
from objc3c_runtime_acceptance.summary_base import build_base_summary_fields
from objc3c_runtime_acceptance.summary_bootstrap import (
    build_bootstrap_summary_sections,
)
from objc3c_runtime_acceptance.summary_interop_sections import (
    build_interop_summary_sections,
)
from objc3c_runtime_acceptance.summary_concurrency_sections import (
    build_concurrency_summary_sections,
)
from objc3c_runtime_acceptance.summary_error_sections import (
    build_error_summary_sections,
)
from objc3c_runtime_acceptance.summary_metaprogramming_sections import (
    build_metaprogramming_summary_sections,
)
from objc3c_runtime_acceptance.summary_object_model_sections import (
    build_object_model_summary_sections,
)
from objc3c_runtime_acceptance.summary_release_sections import (
    build_release_summary_sections,
)
from objc3c_runtime_acceptance.summary_storage_block_sections import (
    build_storage_block_summary_sections,
)
from objc3c_runtime_acceptance.summary_suite_sections import (
    build_suite_and_abi_summary_sections,
)
from objc3c_runtime_acceptance.summary_owner_contracts import (
    build_reporting_owner_contract,
)


def build_runtime_acceptance_summary(
    *,
    args: Any,
    run_dir: Path,
    report_path: Path,
    progress_path: Path,
    clangxx: str,
    results: list[CaseResult],
    acceptance_progress: RuntimeAcceptanceProgress,
    domains: RuntimeAcceptanceDomains,
    available_suites: dict[str, tuple[str, ...]],
) -> dict[str, Any]:
    return {
        "reporting_owner_contract": build_reporting_owner_contract(),
        **build_base_summary_fields(
            args=args,
            run_dir=run_dir,
            progress_path=progress_path,
            clangxx=clangxx,
            results=results,
            acceptance_progress=acceptance_progress,
            available_suites=available_suites,
        ),
        **build_bootstrap_summary_sections(results=results, domains=domains),
        **build_metaprogramming_summary_sections(results=results, domains=domains),
        **build_concurrency_summary_sections(results=results, domains=domains),
        **build_error_summary_sections(results=results, domains=domains),
        **build_interop_summary_sections(results=results, domains=domains),
        **build_release_summary_sections(results=results, domains=domains),
        **build_object_model_summary_sections(results=results, domains=domains),
        **build_storage_block_summary_sections(results=results, domains=domains),
        **build_suite_and_abi_summary_sections(
            results=results,
            report_path=report_path,
            domains=domains,
        ),
    }


__all__ = ["build_runtime_acceptance_summary"]
