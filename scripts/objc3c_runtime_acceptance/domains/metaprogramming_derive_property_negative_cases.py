"""Negative derive/property diagnostics for metaprogramming acceptance."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.fixture_compilation import run_fixture_compile
from objc3c_runtime_acceptance.paths import ROOT


NEGATIVE_DERIVE_PROPERTY_FIXTURES = {
    "unsupported_derive": (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "derive_expansion_inventory_negative_unsupported.objc3",
        "O3S317",
        "unsupported derive 'Networked'",
    ),
    "category_derive": (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "derive_expansion_inventory_negative_category.objc3",
        "O3S318",
        "derive expansion is only supported on primary interfaces",
    ),
    "selector_conflict": (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "derive_expansion_inventory_negative_selector_conflict.objc3",
        "O3S319",
        "derive expansion selector conflict",
    ),
    "unsupported_behavior": (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_behavior_legality_negative_unsupported.objc3",
        "O3S326",
        "unsupported property behavior 'Cached'",
    ),
    "nonobject_behavior": (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_behavior_legality_negative_nonobject.objc3",
        "O3S327",
        "requires an Objective-C object property",
    ),
    "protocol_observed": (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_behavior_legality_negative_protocol_observed.objc3",
        "O3S328",
        "requires a concrete interface or implementation property",
    ),
    "projected_writable": (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_behavior_legality_negative_projected_writable.objc3",
        "O3S330",
        "requires a readonly getter-only property",
    ),
}


def check_negative_derive_property_diagnostics(case_dir: Path) -> dict[str, Any]:
    negative_summary: dict[str, Any] = {}
    for negative_key, (
        fixture_path,
        expected_code,
        expected_message,
    ) in NEGATIVE_DERIVE_PROPERTY_FIXTURES.items():
        compile_dir = case_dir / negative_key / "compile"
        compile_result, _ = run_fixture_compile(
            fixture_path,
            compile_dir,
            write_provenance=False,
        )
        expect(
            compile_result.returncode != 0,
            f"expected negative metaprogramming fixture {negative_key} to fail compilation",
        )
        diagnostics_path = compile_dir / "module.diagnostics.txt"
        expect(
            diagnostics_path.is_file(),
            f"expected diagnostics for negative fixture {negative_key}",
        )
        diagnostics_text = diagnostics_path.read_text(encoding="utf-8")
        expect(
            expected_code in diagnostics_text and expected_message in diagnostics_text,
            f"expected negative metaprogramming fixture {negative_key} to preserve {expected_code}",
        )
        negative_summary[negative_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "diagnostics": str(diagnostics_path.relative_to(ROOT)).replace("\\", "/"),
            "expected_code": expected_code,
        }
    return negative_summary


__all__ = [
    "NEGATIVE_DERIVE_PROPERTY_FIXTURES",
    "check_negative_derive_property_diagnostics",
]
