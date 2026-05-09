"""Storage/reflection property execution runtime acceptance case registry."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult

from .storage_reflection_runtime_property_execution_dispatch_assertions import (
    assert_property_execution_dispatches,
)
from .storage_reflection_runtime_property_execution_payload import (
    capture_property_execution_payload,
)
from .storage_reflection_runtime_property_execution_probe import (
    PROPERTY_EXECUTION_CASE_ID,
    PROPERTY_EXECUTION_FIXTURE,
    PROPERTY_EXECUTION_PROBE,
    run_property_execution_probe,
)
from .storage_reflection_runtime_property_execution_runtime_assertions import (
    assert_property_execution_runtime_payload,
)
from .storage_reflection_runtime_property_execution_summary import (
    build_property_execution_summary,
)


def check_property_execution_case(clangxx: str, run_dir: Path) -> CaseResult:
    payload = run_property_execution_probe(clangxx, run_dir)
    facts = capture_property_execution_payload(payload)
    assert_property_execution_runtime_payload(facts)
    assert_property_execution_dispatches(facts)

    return CaseResult(
        case_id=PROPERTY_EXECUTION_CASE_ID,
        probe=PROPERTY_EXECUTION_PROBE,
        fixture=PROPERTY_EXECUTION_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary=build_property_execution_summary(facts),
    )


__all__ = ["check_property_execution_case"]
