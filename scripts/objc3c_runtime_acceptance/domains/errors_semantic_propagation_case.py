"""Error propagation cleanup semantic acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.errors_semantic_propagation_contract import (
    ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT,
    expected_error_propagation_cleanup_counts,
    expected_error_propagation_cleanup_fields,
)
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_live_error_runtime_fixture_outputs,
)
from objc3c_runtime_acceptance.paths import ROOT


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
    _, _, manifest_path = compile_live_error_runtime_fixture_outputs(
        fixture, case_dir / "compile"
    )
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
    for field_name, expected_value in expected_error_propagation_cleanup_fields().items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected error semantic model to preserve {field_name}",
        )
    for field_name, expected_count in expected_error_propagation_cleanup_counts().items():
        expect(
            surface.get(field_name) == expected_count,
            f"expected error semantic model to preserve {field_name}",
        )

    return CaseResult(
        case_id="error-propagation-cleanup-semantics",
        probe="compile-manifest-semantic-surface",
        fixture="tests/tooling/fixtures/native/error_source_closure_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "owner_contract_id": (
                ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT.owner_contract_id
            ),
            "throws_declaration_sites": surface.get("throws_declaration_sites"),
            "result_like_sites": surface.get("result_like_sites"),
            "ns_error_bridging_sites": surface.get("ns_error_bridging_sites"),
            "propagation_runtime_deferred": surface.get("propagation_runtime_deferred"),
        },
    )


__all__ = ["check_error_propagation_cleanup_semantics_case"]
