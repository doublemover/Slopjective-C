"""Metaprogramming derive/property behavior semantic acceptance case."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.metaprogramming_derive_property_negative_cases import (
    check_negative_derive_property_diagnostics,
)
from objc3c_runtime_acceptance.domains.metaprogramming_derive_property_positive_cases import (
    DERIVE_PROPERTY_DERIVE_FIXTURE,
    check_positive_derive_property_surfaces,
)
from objc3c_runtime_acceptance.paths import ROOT


def check_metaprogramming_derive_property_behavior_semantics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-derive-property-behavior-semantics"
    positive_summary = check_positive_derive_property_surfaces(case_dir)
    negative_summary = check_negative_derive_property_diagnostics(case_dir)

    summary = {
        **positive_summary,
        "negative_cases": negative_summary,
    }

    return CaseResult(
        case_id="metaprogramming-derive-property-behavior-semantics",
        probe="compile-manifest-derive-property-behavior-semantics",
        fixture=str(DERIVE_PROPERTY_DERIVE_FIXTURE.relative_to(ROOT)).replace(
            "\\", "/"
        ),
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )

__all__ = ["check_metaprogramming_derive_property_behavior_semantics_case"]
