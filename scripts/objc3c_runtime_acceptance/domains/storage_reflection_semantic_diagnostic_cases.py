"""Storage/reflection runtime acceptance case helpers."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import NegativeDiagnosticExpectation
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_expect_failure
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.fixture_compilation import compile_negative_diagnostic_batch
from objc3c_runtime_acceptance.paths import ROOT

from ..runtime_contract_storage_reflection import (
    RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
)
from .storage_reflection_owner_contracts import storage_reflection_case_summary


PROPERTY_REFLECTION_ACCESSOR_COMPATIBILITY_DIAGNOSTICS_CASE_ID = (
    "property-reflection-accessor-compatibility-diagnostics"
)

def check_property_reflection_accessor_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / PROPERTY_REFLECTION_ACCESSOR_COMPATIBILITY_DIAGNOSTICS_CASE_ID
    negative_batch = compile_negative_diagnostic_batch(
        case_id="property-reflection-accessor-compatibility-diagnostics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="accessor-selector-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_accessor_selector_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: effective getter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="setter-selector-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_setter_selector_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: effective setter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="reflection-attribute-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_reflection_attribute_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: reflected property attribute and ownership profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
        ],
    )
    negative_results = {str(entry["key"]): entry for entry in negative_batch["results"]}

    return CaseResult(
        case_id=PROPERTY_REFLECTION_ACCESSOR_COMPATIBILITY_DIAGNOSTICS_CASE_ID,
        probe="compile-diagnostics",
        fixture="tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=storage_reflection_case_summary(
            PROPERTY_REFLECTION_ACCESSOR_COMPATIBILITY_DIAGNOSTICS_CASE_ID,
            {
                "getter_selector_negative_diagnostic_count": negative_results[
                    "accessor-selector-mismatch"
                ][
                    "diagnostic_count"
                ],
                "setter_selector_negative_diagnostic_count": negative_results[
                    "setter-selector-mismatch"
                ][
                    "diagnostic_count"
                ],
                "reflection_attribute_negative_diagnostic_count": negative_results[
                    "reflection-attribute-mismatch"
                ][
                    "diagnostic_count"
                ],
                "negative_diagnostics_batch": negative_batch,
            },
        ),
    )


