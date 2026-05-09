"""Negative diagnostic inputs for metaprogramming macro-safety acceptance."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.fixture_compilation import NegativeDiagnosticExpectation


def build_macro_safety_negative_expectations(
    root: Path,
) -> list[NegativeDiagnosticExpectation]:
    negative_fixtures = {
        "missing_metadata": (
            root
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_missing_metadata.objc3",
            "O3S320",
            "requires both objc_macro_package and objc_macro_provenance",
        ),
        "orphan_metadata": (
            root
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_orphan_metadata.objc3",
            "O3S321",
            "macro package/provenance markers require objc_macro",
        ),
        "invalid_package": (
            root
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_invalid_package.objc3",
            "O3S322",
            "macro sandbox rejected package 'thirdparty.runtime'",
        ),
        "invalid_provenance": (
            root
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_invalid_provenance.objc3",
            "O3S323",
            "macro provenance must be a lowercase sha256 digest",
        ),
        "nonpure_callable": (
            root
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_nonpure.objc3",
            "O3S324",
            "must be pure, body-backed, non-async, and non-throws",
        ),
        "method_topology": (
            root
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_method_topology.objc3",
            "O3S325",
            "not sandbox-admitted",
        ),
    }
    return [
        NegativeDiagnosticExpectation(
            key=negative_key,
            fixture=fixture_path,
            expected_snippets=[expected_message],
            expected_codes=[expected_code],
        )
        for negative_key, (fixture_path, expected_code, expected_message) in negative_fixtures.items()
    ]


def summarize_negative_batch(negative_batch: dict[str, Any]) -> dict[str, Any]:
    return {
        entry["key"]: {
            "fixture": entry["fixture"],
            "diagnostics": entry["diagnostics"],
            "expected_code": entry["expected_codes"][0],
            "duration_seconds": entry["duration_seconds"],
        }
        for entry in negative_batch["results"]
    }
