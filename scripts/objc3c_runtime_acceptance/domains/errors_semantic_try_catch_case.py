"""Executable try/throw/do/catch semantic acceptance case."""

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


def check_executable_try_throw_do_catch_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "executable-try-throw-do-catch-semantics"
    positive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "try_do_catch_semantics_positive.objc3"
    )
    _, _, manifest_path = compile_live_error_runtime_fixture_outputs(
        positive_fixture, case_dir / "positive"
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_try_do_catch_semantics", {})
    )
    expect(
        isinstance(surface, dict),
        "expected try/do/catch positive fixture to publish objc_error_handling_try_do_catch_semantics",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "dependency_contract_id": "objc3c.error_handling.error.semantic.model.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_try_do_catch_semantics",
        "try_surface_landed": True,
        "throw_surface_landed": True,
        "do_catch_surface_landed": True,
        "throwing_context_legality_enforced": True,
        "native_emit_remains_fail_closed": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected executable try/do/catch semantics to preserve {field_name}",
        )
    expect(
        surface.get("try_expression_sites") == 3
        and surface.get("try_propagating_sites") == 1
        and surface.get("try_optional_sites") == 1
        and surface.get("try_forced_sites") == 1
        and surface.get("throw_statement_sites") == 1
        and surface.get("do_catch_sites") == 1
        and surface.get("catch_clause_sites") == 2
        and surface.get("catch_binding_sites") == 1
        and surface.get("catch_all_sites") == 1
        and surface.get("throwing_callable_try_sites") == 2
        and surface.get("bridged_callable_try_sites") == 1
        and surface.get("caller_propagation_sites") == 1
        and surface.get("local_handler_sites") == 0
        and surface.get("rethrow_sites") == 0
        and surface.get("contract_violation_sites") == 0,
        "expected executable try/do/catch semantics to preserve the positive semantic counts",
    )

    negatives = [
        (
            "try_requires_throwing_context_negative.objc3",
            ["propagating try requires a throws function or an enclosing do/catch"],
            ["O3S272"],
        ),
        (
            "try_requires_throwing_or_bridged_operand_negative.objc3",
            ["try operand must be a throwing or NSError-bridged call surface"],
            ["O3S271"],
        ),
        (
            "throw_requires_throws_or_catch_negative.objc3",
            ["throw statements require a throws function or a catch body"],
            ["O3S274"],
        ),
        (
            "catch_after_catch_all_negative.objc3",
            ["catch clauses after a catch-all are unreachable"],
            ["O3S269"],
        ),
    ]
    negative_batch = compile_negative_diagnostic_batch(
        case_id="executable-try-throw-do-catch-semantics",
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

    native_fail_closed_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "try_do_catch_native_fail_closed.objc3"
    )
    _, _, native_manifest_path = compile_live_error_runtime_fixture_outputs(
        native_fail_closed_fixture, case_dir / "native-fail-closed"
    )
    native_manifest = json.loads(native_manifest_path.read_text(encoding="utf-8"))
    native_surface = (
        native_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_try_do_catch_semantics", {})
    )
    expect(
        isinstance(native_surface, dict),
        "expected native fail-closed fixture to publish objc_error_handling_try_do_catch_semantics",
    )
    expect(
        native_surface.get("native_emit_remains_fail_closed") is True
        and native_surface.get("try_expression_sites") == 1
        and native_surface.get("do_catch_sites") == 1
        and native_surface.get("bridged_callable_try_sites") == 1,
        "expected native fail-closed fixture to preserve the semantic fail-closed lowering boundary",
    )

    return CaseResult(
        case_id="executable-try-throw-do-catch-semantics",
        probe="compile-manifest-semantic-surface-plus-fail-closed-diagnostics",
        fixture="tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "try_expression_sites": surface.get("try_expression_sites"),
            "catch_clause_sites": surface.get("catch_clause_sites"),
            "bridged_callable_try_sites": surface.get("bridged_callable_try_sites"),
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


__all__ = ["check_executable_try_throw_do_catch_semantics_case"]
