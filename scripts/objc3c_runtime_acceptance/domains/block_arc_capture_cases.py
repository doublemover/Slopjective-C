"""Block/ARC capture legality runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    NegativeDiagnosticExpectation,
    ROOT,
    compile_fixture_outputs,
    compile_negative_diagnostic_batch,
)


def check_escaping_block_capture_legality_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "escaping-block-capture-legality"

    argument_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_argument_positive.objc3"
    )
    _, _, argument_manifest_path = compile_fixture_outputs(
        argument_fixture, case_dir / "argument-positive"
    )
    argument_manifest = json.loads(argument_manifest_path.read_text(encoding="utf-8"))
    argument_escape_surface = (
        argument_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    argument_copy_dispose_surface = (
        argument_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    return_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_return_positive.objc3"
    )
    _, _, return_manifest_path = compile_fixture_outputs(
        return_fixture, case_dir / "return-positive"
    )
    return_manifest = json.loads(return_manifest_path.read_text(encoding="utf-8"))
    return_escape_surface = (
        return_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    return_copy_dispose_surface = (
        return_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    negative_batch = compile_negative_diagnostic_batch(
        case_id="escaping-block-capture-legality",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="bad-call-negative",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "capture_legality_escape_invocation_bad_call.objc3",
                expected_snippets=[
                    "type mismatch: expected 'i32' argument for parameter 0 of callable 'closure', got 'bool'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="missing-capture-negative",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "capture_legality_escape_invocation_missing_capture.objc3",
                expected_snippets=["undefined capture 'seed' in block literal"],
                expected_codes=["O3S202"],
            ),
        ],
    )
    bad_call_negative = negative_batch["results"][0]
    missing_capture_negative = negative_batch["results"][1]
    byref_escape_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_byref_positive.objc3"
    )
    _, _, byref_escape_manifest_path = compile_fixture_outputs(
        byref_escape_fixture, case_dir / "byref-escape-positive"
    )
    byref_escape_manifest = json.loads(
        byref_escape_manifest_path.read_text(encoding="utf-8")
    )
    byref_escape_surface = (
        byref_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    byref_copy_dispose_surface = (
        byref_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    owned_escape_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_owned_capture_positive.objc3"
    )
    _, _, owned_escape_manifest_path = compile_fixture_outputs(
        owned_escape_fixture, case_dir / "owned-escape-positive"
    )
    owned_escape_manifest = json.loads(
        owned_escape_manifest_path.read_text(encoding="utf-8")
    )
    owned_escape_surface = (
        owned_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    owned_escape_copy_dispose_surface = (
        owned_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    expect(
        argument_escape_surface.get("escape_to_heap_sites") == 1
        and argument_escape_surface.get("requires_byref_cells_sites") == 0,
        "expected escaping argument fixture to publish one heap-promotion candidate without byref cells",
    )
    expect(
        argument_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and argument_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected escaping argument fixture to keep copy/dispose helpers elided",
    )
    expect(
        return_escape_surface.get("escape_to_heap_sites") == 1
        and return_escape_surface.get("requires_byref_cells_sites") == 0,
        "expected escaping return fixture to publish one heap-promotion candidate without byref cells",
    )
    expect(
        return_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and return_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected escaping return fixture to keep copy/dispose helpers elided",
    )
    expect(
        byref_escape_surface.get("escape_to_heap_sites") == 1
        and byref_escape_surface.get("requires_byref_cells_sites") == 1,
        "expected escaping byref fixture to publish one heap-promotion candidate with one byref-cell site",
    )
    expect(
        byref_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and byref_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected escaping byref fixture to require copy/dispose helpers",
    )
    expect(
        owned_escape_surface.get("escape_to_heap_sites") == 1
        and owned_escape_surface.get("requires_byref_cells_sites") == 0,
        "expected escaping owned-capture fixture to publish one heap-promotion candidate without byref cells",
    )
    expect(
        owned_escape_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and owned_escape_copy_dispose_surface.get("dispose_helper_required_sites")
        == 1,
        "expected escaping owned-capture fixture to require copy/dispose helpers",
    )

    return CaseResult(
        case_id="escaping-block-capture-legality",
        probe="compile-manifest-diagnostics-and-llvm-ir",
        fixture="tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "argument_escape_to_heap_sites": argument_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "return_escape_to_heap_sites": return_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "bad_call_diagnostic_count": bad_call_negative["diagnostic_count"],
            "missing_capture_diagnostic_count": missing_capture_negative[
                "diagnostic_count"
            ],
            "byref_escape_to_heap_sites": byref_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "byref_copy_helper_required_sites": byref_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "owned_escape_to_heap_sites": owned_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "owned_copy_helper_required_sites": (
                owned_escape_copy_dispose_surface.get("copy_helper_required_sites")
            ),
            "negative_diagnostics_batch": negative_batch,
        },
    )
