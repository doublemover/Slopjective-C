"""Storage/reflection property layout and instance allocation runtime cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult

from .storage_reflection_runtime_layout_artifacts import (
    INSTANCE_ALLOCATION_LAYOUT_CASE_ID,
    INSTANCE_ALLOCATION_LAYOUT_PROBE,
    PROPERTY_LAYOUT_CASE_ID,
    PROPERTY_LAYOUT_PROBE,
    SYNTHESIZED_ACCESSOR_PROPERTY_FIXTURE,
    run_instance_allocation_layout_probe,
    run_property_layout_probe,
)
from .storage_reflection_runtime_layout_assertions import (
    assert_instance_allocation_layout_payload,
    assert_property_layout_payload,
)
from .storage_reflection_runtime_layout_payload import (
    capture_instance_allocation_layout_payload,
    capture_property_layout_payload,
)
from .storage_reflection_runtime_layout_summary import (
    build_instance_allocation_layout_summary,
    build_property_layout_summary,
)


def check_property_layout_case(clangxx: str, run_dir: Path) -> CaseResult:
    artifacts = run_property_layout_probe(clangxx, run_dir)
    facts = capture_property_layout_payload(artifacts)
    assert_property_layout_payload(facts)

    return CaseResult(
        case_id=PROPERTY_LAYOUT_CASE_ID,
        probe=PROPERTY_LAYOUT_PROBE,
        fixture=SYNTHESIZED_ACCESSOR_PROPERTY_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary=build_property_layout_summary(facts),
    )


def check_instance_allocation_layout_runtime_case(
    clangxx: str,
    run_dir: Path,
) -> CaseResult:
    artifacts = run_instance_allocation_layout_probe(clangxx, run_dir)
    facts = capture_instance_allocation_layout_payload(artifacts)
    assert_instance_allocation_layout_payload(facts)

    return CaseResult(
        case_id=INSTANCE_ALLOCATION_LAYOUT_CASE_ID,
        probe=INSTANCE_ALLOCATION_LAYOUT_PROBE,
        fixture=SYNTHESIZED_ACCESSOR_PROPERTY_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary=build_instance_allocation_layout_summary(facts),
    )


__all__ = [
    "check_property_layout_case",
    "check_instance_allocation_layout_runtime_case",
]
