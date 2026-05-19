"""Focused object-model protocol/category linked-runtime acceptance case."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.fixture_compilation import (
    NegativeDiagnosticExpectation,
)
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.fixture_compilation import compile_negative_diagnostic_batch
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe


RUNTIME_OBJECT_FOUNDATION_CASE_ID = "runtime-object-foundation-protocol-category"
RUNTIME_OBJECT_FOUNDATION_FIXTURE = (
    "tests/tooling/fixtures/native/category_attachment_protocol_runtime_library.objc3"
)
RUNTIME_OBJECT_FOUNDATION_PROBE = (
    "tests/tooling/runtime/category_attachment_protocol_runtime_probe.cpp"
)


def _assert_protocol_category_payload(payload: dict[str, Any]) -> None:
    graph = payload.get("graph_state", {})
    widget = payload.get("widget_entry", {})
    base = payload.get("base_entry", {})
    worker = payload.get("worker_query", {})
    tracer = payload.get("tracer_query", {})
    base_worker = payload.get("base_worker_query", {})
    method_state = payload.get("method_state", {})

    expect(payload.get("category_value") == 13, "expected category dispatch to return 13")
    expect(payload.get("class_value") == 11, "expected class dispatch to return 11")
    expect(
        payload.get("protocol_strict_error")
        == payload.get("protocol_strict_error_expected"),
        "expected protocol-declared optional selector miss to fail closed",
    )
    expect(
        graph.get("attached_category_count") == 1
        and graph.get("protocol_conformance_edge_count", 0) >= 2,
        "expected realized graph to expose attached category and protocol edges",
    )
    expect(
        widget.get("attached_category_count") == 1
        and widget.get("direct_protocol_count") == 1
        and widget.get("attached_protocol_count") == 1,
        "expected Widget to expose direct and category-attached protocols",
    )
    expect(
        base.get("found") == 1 and base.get("attached_protocol_count") == 0,
        "expected Base to stay realized without category-attached protocols",
    )
    expect(
        worker.get("conforms") == 1 and worker.get("matched_protocol_owner_identity"),
        "expected Widget to conform directly to Worker",
    )
    expect(
        tracer.get("conforms") == 1
        and tracer.get("visited_protocol_count", 0) >= 2
        and tracer.get("matched_attachment_owner_identity") == "category:Widget(Tracing)",
        "expected Widget to conform to inherited Tracer protocol through the category attachment",
    )
    expect(
        base_worker.get("class_found") == 1
        and base_worker.get("protocol_found") == 1
        and base_worker.get("conforms") == 0,
        "expected Base/Worker conformance query to fail closed without inheriting subclass protocols",
    )
    expect(
        method_state.get("last_selector") == "ignoredValue"
        and method_state.get("last_dispatch_strict_error") == 1,
        "expected unresolved protocol selector dispatch to publish strict-error cache state",
    )


def _assert_protocol_category_compile_artifacts(manifest: dict[str, Any]) -> None:
    category_surface = manifest.get("runtime_category_attachment_merged_dispatch_surface", {})
    realization_surface = manifest.get("runtime_class_metaclass_protocol_realization_surface", {})
    expect(
        category_surface.get("contract_id")
        == "objc3c.runtime.category.attachment.merged.dispatch.surface.v1",
        "expected fixture manifest to publish category attachment merged dispatch surface",
    )
    expect(
        realization_surface.get("contract_id")
        == "objc3c.runtime.class.metaclass.protocol.realization.v1",
        "expected fixture manifest to publish class/metaclass/protocol realization surface",
    )


def _compile_category_conflict_diagnostics(case_dir: Path) -> dict[str, Any]:
    return compile_negative_diagnostic_batch(
        case_id=RUNTIME_OBJECT_FOUNDATION_CASE_ID,
        out_dir=case_dir / "category-conflicts",
        expectations=[
            NegativeDiagnosticExpectation(
                key="category-attachment-collision",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "category_attachment_collision.objc3",
                expected_snippets=[
                    "duplicate category interface 'Root(Extras)'",
                ],
                expected_codes=["O3S200"],
            )
        ],
    )


def check_runtime_object_foundation_protocol_category_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / RUNTIME_OBJECT_FOUNDATION_CASE_ID
    fixture = ROOT / RUNTIME_OBJECT_FOUNDATION_FIXTURE
    obj_path, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / RUNTIME_OBJECT_FOUNDATION_PROBE
    exe_path = case_dir / "category_attachment_protocol_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(
        run_probe(exe_path), "runtime object foundation protocol/category probe"
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    conflict_diagnostics = _compile_category_conflict_diagnostics(case_dir)

    _assert_protocol_category_payload(payload)
    _assert_protocol_category_compile_artifacts(manifest)

    return CaseResult(
        case_id=RUNTIME_OBJECT_FOUNDATION_CASE_ID,
        probe=RUNTIME_OBJECT_FOUNDATION_PROBE,
        fixture=RUNTIME_OBJECT_FOUNDATION_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "category_value": payload["category_value"],
            "class_value": payload["class_value"],
            "attached_category_count": payload["graph_state"]["attached_category_count"],
            "tracer_visited_protocol_count": payload["tracer_query"][
                "visited_protocol_count"
            ],
            "category_conflict_diagnostic_count": conflict_diagnostics[
                "results"
            ][0]["diagnostic_count"],
        },
    )


__all__ = [
    "RUNTIME_OBJECT_FOUNDATION_CASE_ID",
    "RUNTIME_OBJECT_FOUNDATION_FIXTURE",
    "RUNTIME_OBJECT_FOUNDATION_PROBE",
    "check_runtime_object_foundation_protocol_category_case",
]
