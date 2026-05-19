"""Cross-module Block/ARC artifact preservation acceptance case."""

from __future__ import annotations

from pathlib import Path
from time import perf_counter

from objc3c_runtime_acceptance.case_result import CaseResult

from ..runtime_contract_block_arc import BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE
from .block_arc_cross_module_artifacts import (
    capture_cross_module_block_ownership_artifacts,
)
from .block_arc_cross_module_assertions import (
    assert_cross_module_block_ownership_artifacts,
)
from .block_arc_cross_module_summary import (
    CROSS_MODULE_BLOCK_OWNERSHIP_CASE_ID,
    build_cross_module_block_ownership_summary,
)


def check_cross_module_block_ownership_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    artifacts = capture_cross_module_block_ownership_artifacts(run_dir)
    assert_cross_module_block_ownership_artifacts(artifacts)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    return CaseResult(
        case_id=CROSS_MODULE_BLOCK_OWNERSHIP_CASE_ID,
        probe=None,
        fixture=BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE,
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary=build_cross_module_block_ownership_summary(
            artifacts,
            case_total_ms,
        ),
    )


__all__ = ["check_cross_module_block_ownership_artifact_preservation_case"]
