"""Storage/reflection cross-module runtime acceptance case registry."""

from __future__ import annotations

from pathlib import Path
from time import perf_counter

from objc3c_runtime_acceptance.case_result import CaseResult

from ..runtime_contract_storage_reflection import STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE
from .storage_reflection_cross_module_artifacts import (
    capture_cross_module_storage_reflection_artifacts,
)
from .storage_reflection_cross_module_assertions import (
    assert_cross_module_storage_reflection_artifacts,
)
from .storage_reflection_cross_module_summary import (
    build_cross_module_storage_reflection_summary,
)


def check_cross_module_storage_reflection_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    artifacts = capture_cross_module_storage_reflection_artifacts(run_dir)
    assert_cross_module_storage_reflection_artifacts(artifacts)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    return CaseResult(
        case_id="cross-module-storage-reflection-artifact-preservation",
        probe=None,
        fixture=STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary=build_cross_module_storage_reflection_summary(
            artifacts,
            case_total_ms,
        ),
    )


__all__ = ["check_cross_module_storage_reflection_artifact_preservation_case"]
