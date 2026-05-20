"""Catch filter finalization source acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_live_error_runtime_fixture_outputs,
)
from objc3c_runtime_acceptance.paths import ROOT


def check_catch_filter_finalization_source_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "catch-filter-finalization-source"
    try_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "try_do_catch_semantics_positive.objc3"
    )
    _, _, try_manifest_path = compile_live_error_runtime_fixture_outputs(
        try_fixture, case_dir / "try"
    )
    try_manifest = json.loads(try_manifest_path.read_text(encoding="utf-8"))
    try_surface = (
        try_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_try_do_catch_semantics", {})
    )
    expect(
        isinstance(try_surface, dict),
        "expected try/do/catch fixture to publish objc_error_handling_try_do_catch_semantics",
    )
    expected_try_fields = {
        "contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "dependency_contract_id": "objc3c.error_handling.error.semantic.model.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_try_do_catch_semantics",
        "try_surface_landed": True,
        "throw_surface_landed": True,
        "do_catch_surface_landed": True,
        "throwing_context_legality_enforced": True,
        "native_emit_remains_fail_closed": False,
        "deterministic": True,
        "ready_for_lowering_and_runtime": True,
    }
    for field_name, expected_value in expected_try_fields.items():
        expect(
            try_surface.get(field_name) == expected_value,
            f"expected try/do/catch source surface to preserve {field_name}",
        )
    expect(
        try_surface.get("try_expression_sites") == 3
        and try_surface.get("throw_statement_sites") == 1
        and try_surface.get("do_catch_sites") == 1
        and try_surface.get("catch_clause_sites") == 2
        and try_surface.get("catch_all_sites") == 1,
        "expected try/do/catch source surface to preserve the catch/finalization source counts",
    )

    bridge_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "bridge_legality_positive.objc3"
    )
    _, _, bridge_manifest_path = compile_live_error_runtime_fixture_outputs(
        bridge_fixture, case_dir / "bridge"
    )
    bridge_manifest = json.loads(bridge_manifest_path.read_text(encoding="utf-8"))
    bridge_surface = (
        bridge_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_bridge_legality", {})
    )
    expect(
        isinstance(bridge_surface, dict),
        "expected bridge legality fixture to publish objc_error_handling_error_bridge_legality",
    )
    expected_bridge_fields = {
        "contract_id": "objc3c.error_handling.error.bridge.legality.v1",
        "dependency_contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_bridge_legality",
        "bridge_legality_landed": True,
        "try_bridge_filter_landed": True,
        "unsupported_combinations_fail_closed": True,
        "native_emit_remains_fail_closed": False,
        "deterministic": True,
        "ready_for_lowering_and_runtime": True,
    }
    for field_name, expected_value in expected_bridge_fields.items():
        expect(
            bridge_surface.get(field_name) == expected_value,
            f"expected bridge legality source surface to preserve {field_name}",
        )
    expect(
        bridge_surface.get("bridge_callable_sites") == 2
        and bridge_surface.get("semantically_valid_bridge_callable_sites") == 2
        and bridge_surface.get("try_eligible_bridge_callable_sites") == 2
        and bridge_surface.get("unsupported_combination_sites") == 0
        and bridge_surface.get("throws_bridge_conflict_sites") == 0,
        "expected bridge legality source surface to preserve the catch-filter eligibility counts",
    )

    return CaseResult(
        case_id="catch-filter-finalization-source",
        probe="compile-manifest-source-surface",
        fixture="tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "try_expression_sites": try_surface.get("try_expression_sites"),
            "catch_clause_sites": try_surface.get("catch_clause_sites"),
            "bridge_callable_sites": bridge_surface.get("bridge_callable_sites"),
            "try_eligible_bridge_callable_sites": bridge_surface.get(
                "try_eligible_bridge_callable_sites"
            ),
        },
    )


__all__ = ["check_catch_filter_finalization_source_case"]
