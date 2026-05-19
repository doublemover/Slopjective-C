"""Error execution cleanup source acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_live_error_runtime_fixture_outputs,
)
from objc3c_runtime_acceptance.paths import ROOT


def check_error_execution_cleanup_source_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-execution-cleanup-source"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "error_source_closure_positive.objc3"
    )
    _, _, manifest_path = compile_live_error_runtime_fixture_outputs(
        fixture, case_dir / "compile"
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_source_closure", {})
    )
    expect(
        isinstance(surface, dict),
        "expected error source closure fixture to publish objc_error_handling_error_source_closure",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.error.source.closure.v1",
        "frontend_surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_source_closure",
        "throws_declaration_source_supported": True,
        "result_carrier_source_supported": True,
        "ns_error_bridging_source_supported": True,
        "error_bridge_marker_source_supported": True,
        "try_keyword_reserved": True,
        "throw_keyword_reserved": True,
        "catch_keyword_reserved": True,
        "try_fail_closed": True,
        "throw_fail_closed": True,
        "do_catch_fail_closed": True,
        "deterministic_handoff": True,
        "ready_for_semantic_expansion": True,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected error source closure surface to preserve {field_name}",
        )
    expect(
        surface.get("function_throws_declaration_sites") == 1,
        "expected error source closure surface to publish one function throws declaration site",
    )
    expect(
        surface.get("result_like_sites") == 7
        and surface.get("result_success_sites") == 1
        and surface.get("result_failure_sites") == 2
        and surface.get("result_branch_sites") == 4
        and surface.get("result_payload_sites") == 3,
        "expected error source closure surface to preserve the result carrier source counts",
    )
    expect(
        surface.get("ns_error_bridging_sites") == 3
        and surface.get("ns_error_out_parameter_sites") == 1
        and surface.get("ns_error_bridge_path_sites") == 1,
        "expected error source closure surface to preserve the NSError bridge source counts",
    )
    return CaseResult(
        case_id="error-execution-cleanup-source",
        probe="compile-manifest-source-surface",
        fixture="tests/tooling/fixtures/native/error_source_closure_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "throws_declaration_sites": surface.get(
                "function_throws_declaration_sites"
            ),
            "result_like_sites": surface.get("result_like_sites"),
            "ns_error_bridging_sites": surface.get("ns_error_bridging_sites"),
            "ready_for_semantic_expansion": surface.get("ready_for_semantic_expansion"),
        },
    )


__all__ = ["check_error_execution_cleanup_source_case"]
