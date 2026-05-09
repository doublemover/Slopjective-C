"""Error propagation cleanup semantic acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import ROOT, compile_fixture_outputs


def check_error_propagation_cleanup_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-propagation-cleanup-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "error_source_closure_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_semantic_model", {})
    )
    expect(
        isinstance(surface, dict),
        "expected error semantic model fixture to publish objc_error_handling_error_semantic_model",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.error.semantic.model.v1",
        "frontend_dependency_contract_id": "objc3c.error_handling.error.source.closure.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_semantic_model",
        "throws_declaration_semantics_landed": True,
        "result_carrier_profile_semantics_landed": True,
        "ns_error_bridging_profile_semantics_landed": True,
        "bridge_marker_semantics_landed": True,
        "parser_fail_closed_boundary_required": True,
        "parser_fail_closed_boundary_preserved": True,
        "propagation_runtime_deferred": True,
        "status_to_error_runtime_deferred": True,
        "native_error_abi_deferred": True,
        "placeholder_throws_summary_carried": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected error semantic model to preserve {field_name}",
        )
    expect(
        surface.get("throws_declaration_sites") == 1
        and surface.get("result_like_sites") == 7
        and surface.get("ns_error_bridging_sites") == 3
        and surface.get("placeholder_throws_propagation_sites") == 0
        and surface.get("placeholder_unwind_cleanup_sites") == 0,
        "expected error semantic model to preserve propagation and cleanup counts",
    )

    return CaseResult(
        case_id="error-propagation-cleanup-semantics",
        probe="compile-manifest-semantic-surface",
        fixture="tests/tooling/fixtures/native/error_source_closure_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "throws_declaration_sites": surface.get("throws_declaration_sites"),
            "result_like_sites": surface.get("result_like_sites"),
            "ns_error_bridging_sites": surface.get("ns_error_bridging_sites"),
            "propagation_runtime_deferred": surface.get("propagation_runtime_deferred"),
        },
    )


__all__ = ["check_error_propagation_cleanup_semantics_case"]
