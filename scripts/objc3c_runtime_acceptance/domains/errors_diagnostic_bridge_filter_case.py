"""Bridge filter and unwind compatibility diagnostic acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import (
    NegativeDiagnosticExpectation,
    compile_live_error_runtime_fixture_outputs,
    compile_negative_diagnostic_batch,
)
from objc3c_runtime_acceptance.paths import ROOT


def check_bridging_filter_unwind_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "bridging-filter-unwind-compatibility-diagnostics"
    positive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "bridge_legality_positive.objc3"
    )
    _, _, manifest_path = compile_live_error_runtime_fixture_outputs(
        positive_fixture, case_dir / "positive"
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_bridge_legality", {})
    )
    expect(
        isinstance(surface, dict),
        "expected bridge legality positive fixture to publish objc_error_handling_error_bridge_legality",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.error.bridge.legality.v1",
        "dependency_contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_bridge_legality",
        "bridge_legality_landed": True,
        "try_bridge_filter_landed": True,
        "unsupported_combinations_fail_closed": True,
        "native_emit_remains_fail_closed": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected bridge legality diagnostics to preserve {field_name}",
        )
    expect(
        surface.get("bridge_callable_sites") == 2
        and surface.get("objc_nserror_callable_sites") == 1
        and surface.get("objc_status_code_callable_sites") == 1
        and surface.get("semantically_valid_bridge_callable_sites") == 2
        and surface.get("try_eligible_bridge_callable_sites") == 2
        and surface.get("unsupported_combination_sites") == 0
        and surface.get("contract_violation_sites") == 0,
        "expected bridge legality diagnostics to preserve the positive bridge counts",
    )

    native_fail_closed_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "bridge_legality_native_fail_closed.objc3"
    )
    _, _, native_manifest_path = compile_live_error_runtime_fixture_outputs(
        native_fail_closed_fixture, case_dir / "native-fail-closed"
    )
    native_manifest = json.loads(native_manifest_path.read_text(encoding="utf-8"))
    native_surface = (
        native_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_bridge_legality", {})
    )
    expect(
        isinstance(native_surface, dict),
        "expected bridge native fail-closed fixture to publish objc_error_handling_error_bridge_legality",
    )
    expect(
        native_surface.get("native_emit_remains_fail_closed") is True
        and native_surface.get("bridge_callable_sites") == 1
        and native_surface.get("objc_status_code_callable_sites") == 1
        and native_surface.get("try_eligible_bridge_callable_sites") == 1,
        "expected bridge native fail-closed fixture to preserve the lowered bridge boundary",
    )

    negatives = [
        (
            "bridge_legality_nserror_missing_out_negative.objc3",
            ["objc_nserror requires an NSError out parameter"],
            ["O3S275"],
        ),
        (
            "bridge_legality_nserror_bad_return_negative.objc3",
            ["objc_nserror currently requires a BOOL-like success return"],
            ["O3S276"],
        ),
        (
            "bridge_legality_throws_conflict_negative.objc3",
            ["NSError/status bridge markers cannot currently be combined with throws"],
            ["O3S277"],
        ),
        (
            "bridge_legality_marker_conflict_negative.objc3",
            ["objc_nserror and objc_status_code cannot appear on the same callable"],
            ["O3S278"],
        ),
        (
            "bridge_legality_bad_error_type_negative.objc3",
            ["objc_status_code currently requires error_type: NSError"],
            ["O3S280"],
        ),
        (
            "bridge_legality_missing_mapping_negative.objc3",
            ["objc_status_code mapping symbol must resolve to a declared function"],
            ["O3S282"],
        ),
        (
            "bridge_legality_bad_mapping_signature_negative.objc3",
            ["objc_status_code mapping function must accept one matching status parameter and return NSError"],
            ["O3S283"],
        ),
        (
            "bridge_legality_bad_status_return_negative.objc3",
            ["objc_status_code requires a BOOL-like or integer status return"],
            ["O3S281"],
        ),
    ]
    negative_batch = compile_negative_diagnostic_batch(
        case_id="bridging-filter-unwind-compatibility-diagnostics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key=Path(fixture_name).stem,
                fixture=ROOT / "tests" / "tooling" / "fixtures" / "native" / fixture_name,
                expected_snippets=expected_snippets,
                expected_codes=expected_codes,
            )
            for fixture_name, expected_snippets, expected_codes in negatives
        ],
    )
    negative_summaries = [
        {
            "fixture": entry["fixture"],
            "diagnostic_codes": entry["diagnostic_codes"],
            "duration_seconds": entry["duration_seconds"],
        }
        for entry in negative_batch["results"]
    ]

    return CaseResult(
        case_id="bridging-filter-unwind-compatibility-diagnostics",
        probe="compile-manifest-semantic-surface-plus-compatibility-diagnostics",
        fixture="tests/tooling/fixtures/native/bridge_legality_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "bridge_callable_sites": surface.get("bridge_callable_sites"),
            "try_eligible_bridge_callable_sites": surface.get(
                "try_eligible_bridge_callable_sites"
            ),
            "native_fail_closed_fixture": {
                "fixture": str(native_fail_closed_fixture.relative_to(ROOT)).replace(
                    "\\", "/"
                ),
                "native_emit_remains_fail_closed": native_surface.get(
                    "native_emit_remains_fail_closed"
                ),
            },
            "negative_fixtures": negative_summaries,
            "negative_diagnostics_batch": negative_batch,
        },
    )


__all__ = ["check_bridging_filter_unwind_compatibility_diagnostics_case"]
