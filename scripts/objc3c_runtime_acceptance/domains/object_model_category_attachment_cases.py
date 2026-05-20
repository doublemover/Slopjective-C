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
RUNTIME_OBJECT_FOUNDATION_INVALID_METADATA_PROBE = (
    "tests/tooling/runtime/protocol_category_invalid_metadata_probe.cpp"
)


def _assert_protocol_category_payload(payload: dict[str, Any]) -> None:
    graph = payload.get("graph_state", {})
    widget = payload.get("widget_entry", {})
    base = payload.get("base_entry", {})
    worker = payload.get("worker_query", {})
    tracer = payload.get("tracer_query", {})
    base_worker = payload.get("base_worker_query", {})
    derived_worker = payload.get("derived_worker_query", {})
    category_first_state = payload.get("category_first_state", {})
    category_second_state = payload.get("category_second_state", {})
    method_state = payload.get("method_state", {})
    category_entry = payload.get("category_entry", {})
    strict_error_entry = payload.get("strict_error_entry", {})
    strict_error_i32_result = payload.get("protocol_strict_error_i32_result", {})
    strict_error_typed_result = payload.get(
        "protocol_strict_error_typed_result", {}
    )

    expect(payload.get("category_value") == 13, "expected category dispatch to return 13")
    expect(
        payload.get("category_cached_value") == 13,
        "expected cached category dispatch to return 13",
    )
    expect(payload.get("class_value") == 11, "expected class dispatch to return 11")
    expect(
        payload.get("protocol_strict_error")
        == payload.get("protocol_strict_error_expected"),
        "expected protocol-declared optional selector miss to fail closed",
    )
    expect(
        strict_error_i32_result.get("status_code") == -1
        and strict_error_i32_result.get("return_kind") == 0
        and strict_error_i32_result.get("value") == 0
        and strict_error_i32_result.get("diagnostic_code") == "O3RT001"
        and strict_error_i32_result.get("diagnostic_message")
        == "runtime dispatch failed: unknown selector"
        and strict_error_i32_result.get("result_contract")
        == "typed-dispatch-strict-error-result",
        "expected ignoredValue i32 checked dispatch to expose strict-error status, diagnostic, and result contract",
    )
    expect(
        strict_error_typed_result.get("status_code") == -1
        and strict_error_typed_result.get("return_kind") == 0
        and strict_error_typed_result.get("return_kind_name") == "unsupported"
        and strict_error_typed_result.get("i32_value") == 0
        and strict_error_typed_result.get("bool_value") == 0
        and strict_error_typed_result.get("object_reference") == 0
        and strict_error_typed_result.get("class_reference") == 0
        and strict_error_typed_result.get("selector_reference") == 0
        and strict_error_typed_result.get("protocol_reference") == 0
        and strict_error_typed_result.get("diagnostic_code") == "O3RT001"
        and strict_error_typed_result.get("diagnostic_message")
        == "runtime dispatch failed: unknown selector"
        and strict_error_typed_result.get("result_contract")
        == "typed-dispatch-strict-error-result",
        "expected ignoredValue typed checked dispatch to expose strict-error status, diagnostic, and result contract",
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
        worker.get("malformed_metadata") == 0,
        "expected direct Worker conformance query to avoid malformed metadata",
    )
    expect(
        worker.get("matched_protocol_depth", 0) == 0
        and worker.get("matched_from_category", 0) == 0,
        "expected direct Worker conformance to publish a direct class match route",
    )
    expect(
        tracer.get("conforms") == 1
        and tracer.get("malformed_metadata") == 0
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
        base_worker.get("malformed_metadata") == 0,
        "expected Base/Worker conformance miss to stay distinct from malformed metadata",
    )
    expect(
        derived_worker.get("class_found") == 1
        and derived_worker.get("protocol_found") == 1
        and derived_worker.get("conforms") == 1
        and derived_worker.get("malformed_metadata") == 0
        and derived_worker.get("matched_protocol_owner_identity") == "protocol:Worker"
        and derived_worker.get("matched_class_name") == "Derived"
        and derived_worker.get("matched_protocol_depth", 0) >= 1
        and derived_worker.get("matched_via_inherited_protocol") == 1
        and derived_worker.get("matched_from_category") == 0,
        "expected Derived to conform to Worker through inherited Tracer protocol semantics",
    )
    expect(
        method_state.get("last_selector") == "ignoredValue"
        and method_state.get("last_dispatch_strict_error") == 1,
        "expected unresolved protocol selector dispatch to publish strict-error cache state",
    )
    expect(
        category_first_state.get("last_selector") == "tracedValue"
        and category_first_state.get("last_dispatch_used_cache") == 0
        and category_first_state.get("last_dispatch_resolved_live_method") == 1
        and category_first_state.get("last_category_probe_count", 0) >= 1,
        "expected first category dispatch to resolve through slow-path merged category lookup",
    )
    expect(
        category_second_state.get("last_selector") == "tracedValue"
        and category_second_state.get("last_dispatch_used_cache") == 1
        and category_second_state.get("last_dispatch_resolved_live_method") == 1,
        "expected second category dispatch to hit the merged dispatch cache",
    )
    expect(
        category_entry.get("found") == 1
        and category_entry.get("resolved") == 1
        and category_entry.get("selector") == "tracedValue"
        and category_entry.get("resolved_class_name") == "Widget"
        and category_entry.get("resolved_owner_identity")
        == "implementation:Widget(Tracing)::instance_method:tracedValue"
        and category_entry.get("category_probe_count", 0) >= 1,
        "expected tracedValue cache entry to preserve the category implementation owner",
    )
    expect(
        strict_error_entry.get("found") == 1
        and strict_error_entry.get("resolved") == 0
        and strict_error_entry.get("selector") == "ignoredValue"
        and strict_error_entry.get("category_probe_count", 0) >= 1
        and strict_error_entry.get("protocol_probe_count", 0) >= 1,
        "expected ignoredValue to preserve protocol/category negative lookup evidence",
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


def _assert_invalid_protocol_metadata_payload(payload: dict[str, Any]) -> None:
    missing_category = payload.get("missing_category_target", {})
    expect(
        payload.get("registration_status") == -4
        and payload.get("last_registration_status") == -4,
        "expected unregistered protocol reference to fail image registration",
    )
    expect(
        payload.get("registered_image_count") == 0
        and payload.get("realized_class_count") == 0
        and payload.get("class_found") == 0,
        "expected invalid protocol metadata to publish no image or class graph",
    )
    expect(
        payload.get("malformed_class_metadata_rejection_count", 0) >= 1,
        "expected invalid protocol metadata to increment malformed metadata rejection count",
    )
    expect(
        payload.get("last_malformed_class_graph_reason")
        == "unknown protocol reference in class BrokenProtocolRef",
        "expected stable fail-closed diagnostic reason for unregistered protocol refs",
    )
    expect(
        missing_category.get("registration_status") == -4
        and missing_category.get("last_registration_status") == -4,
        "expected category metadata targeting an absent class to fail image registration",
    )
    expect(
        missing_category.get("registered_image_count") == 0
        and missing_category.get("realized_class_count") == 0
        and missing_category.get("class_found") == 0,
        "expected missing category target metadata to publish no image or class graph",
    )
    expect(
        missing_category.get("malformed_class_metadata_rejection_count", 0) >= 1,
        "expected missing category target metadata to increment malformed metadata rejection count",
    )
    expect(
        missing_category.get("last_malformed_class_graph_reason")
        == "category attachment target class is missing for MissingOwner(Tracing)",
        "expected stable fail-closed diagnostic reason for category target availability",
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
            ),
            NegativeDiagnosticExpectation(
                key="category-merge-conflicting-method",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "category_merge_conflicting_method.objc3",
                expected_snippets=[
                    "category merge failure: category 'Widget(Beta)' selector '+value' conflicts with attached category 'Widget(Alpha)'",
                ],
                expected_codes=["O3S219"],
            ),
            NegativeDiagnosticExpectation(
                key="category-merge-conflicting-property",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "category_merge_conflicting_property.objc3",
                expected_snippets=[
                    "category merge failure: category 'Widget(Beta)' property 'token' conflicts with attached category 'Widget(Alpha)'",
                ],
                expected_codes=["O3S219"],
            ),
            NegativeDiagnosticExpectation(
                key="category-merge-missing-pair",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "category_merge_missing_pair.objc3",
                expected_snippets=[
                    "category merge failure: category interface 'Widget(Debug)' is missing category implementation for realized class 'Widget'",
                ],
                expected_codes=["O3S219"],
            ),
            NegativeDiagnosticExpectation(
                key="category-unknown-class-rejected",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "category_unknown_class_rejected.objc3",
                expected_snippets=[
                    "category attachment failure: category interface 'MissingOwner(Tracing)' attaches to unknown class 'MissingOwner'",
                    "category attachment failure: category implementation 'MissingOwner(Tracing)' attaches to unknown class 'MissingOwner'",
                ],
                expected_codes=["O3S219"],
            ),
            NegativeDiagnosticExpectation(
                key="category-unavailable-class-rejected",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "category_unavailable_class_rejected.objc3",
                expected_snippets=[
                    "category attachment failure: category interface 'Widget(Tracing)' attaches to unavailable class 'Widget' without a realized implementation",
                    "category attachment failure: category implementation 'Widget(Tracing)' attaches to unavailable class 'Widget' without a realized implementation",
                ],
                expected_codes=["O3S219"],
            ),
            NegativeDiagnosticExpectation(
                key="protocol-requirement-duplicate-conflict",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "protocol_requirement_duplicate_conflict_rejected.objc3",
                expected_snippets=[
                    "protocol requirement conflict: duplicate selector '-work' in protocol 'Worker'",
                ],
                expected_codes=["O3S218"],
            ),
            NegativeDiagnosticExpectation(
                key="protocol-requirement-inherited-conflict",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "protocol_requirement_inherited_conflict_rejected.objc3",
                expected_snippets=[
                    "protocol requirement conflict: protocol 'MixedReadable' inherits incompatible selector '-readValue' from protocol 'TextReadable'",
                ],
                expected_codes=["O3S218"],
            ),
            NegativeDiagnosticExpectation(
                key="protocol-dispatch-intent-rejected",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "protocol_dispatch_intent_rejected.objc3",
                expected_snippets=[
                    "dispatch-control semantics failed: selector '-shared' in protocol 'P' cannot use Part 9 dispatch-control callable attributes",
                ],
                expected_codes=["O3S314"],
            ),
            NegativeDiagnosticExpectation(
                key="category-method-dispatch-intent-rejected",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "category_method_dispatch_intent_rejected.objc3",
                expected_snippets=[
                    "dispatch-control semantics failed: selector '-shared' in category 'Box(Ext)' cannot use Part 9 dispatch-control callable attributes",
                ],
                expected_codes=["O3S315"],
            ),
            NegativeDiagnosticExpectation(
                key="category-container-dispatch-intent-rejected",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "category_container_dispatch_intent_rejected.objc3",
                expected_snippets=[
                    "dispatch-control semantics failed: category 'Box(Ext)' cannot use objc_direct_members, objc_final, or objc_sealed container attributes",
                ],
                expected_codes=["O3S316"],
            ),
            NegativeDiagnosticExpectation(
                key="duplicate-protocol-runtime-export",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "duplicate_protocol_runtime_export.objc3",
                expected_snippets=[
                    "duplicate protocol 'Worker'",
                ],
                expected_codes=["O3S200"],
            ),
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
    invalid_metadata_probe = ROOT / RUNTIME_OBJECT_FOUNDATION_INVALID_METADATA_PROBE
    invalid_metadata_exe = case_dir / "protocol_category_invalid_metadata_probe.exe"
    compile_probe(clangxx, invalid_metadata_probe, invalid_metadata_exe, [])
    invalid_metadata_payload = parse_json_output(
        run_probe(invalid_metadata_exe),
        "runtime protocol/category invalid metadata probe",
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    conflict_diagnostics = _compile_category_conflict_diagnostics(case_dir)

    _assert_protocol_category_payload(payload)
    _assert_protocol_category_compile_artifacts(manifest)
    _assert_invalid_protocol_metadata_payload(invalid_metadata_payload)

    return CaseResult(
        case_id=RUNTIME_OBJECT_FOUNDATION_CASE_ID,
        probe=RUNTIME_OBJECT_FOUNDATION_PROBE,
        fixture=RUNTIME_OBJECT_FOUNDATION_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "category_value": payload["category_value"],
            "category_cached_value": payload["category_cached_value"],
            "class_value": payload["class_value"],
            "protocol_strict_error_i32_status": payload[
                "protocol_strict_error_i32_result"
            ]["status_code"],
            "protocol_strict_error_i32_diagnostic": payload[
                "protocol_strict_error_i32_result"
            ]["diagnostic_code"],
            "protocol_strict_error_i32_result_contract": payload[
                "protocol_strict_error_i32_result"
            ]["result_contract"],
            "protocol_strict_error_typed_status": payload[
                "protocol_strict_error_typed_result"
            ]["status_code"],
            "protocol_strict_error_typed_diagnostic": payload[
                "protocol_strict_error_typed_result"
            ]["diagnostic_code"],
            "protocol_strict_error_typed_result_contract": payload[
                "protocol_strict_error_typed_result"
            ]["result_contract"],
            "attached_category_count": payload["graph_state"]["attached_category_count"],
            "tracer_visited_protocol_count": payload["tracer_query"][
                "visited_protocol_count"
            ],
            "derived_worker_protocol_depth": payload["derived_worker_query"][
                "matched_protocol_depth"
            ],
            "category_conflict_diagnostic_count": conflict_diagnostics[
                "results"
            ][0]["diagnostic_count"],
            "category_protocol_negative_fixture_count": conflict_diagnostics[
                "fixture_count"
            ],
            "invalid_protocol_metadata_registration_status": invalid_metadata_payload[
                "registration_status"
            ],
            "invalid_protocol_metadata_reason": invalid_metadata_payload[
                "last_malformed_class_graph_reason"
            ],
            "missing_category_target_registration_status": invalid_metadata_payload[
                "missing_category_target"
            ]["registration_status"],
            "missing_category_target_reason": invalid_metadata_payload[
                "missing_category_target"
            ]["last_malformed_class_graph_reason"],
        },
    )


__all__ = [
    "RUNTIME_OBJECT_FOUNDATION_CASE_ID",
    "RUNTIME_OBJECT_FOUNDATION_FIXTURE",
    "RUNTIME_OBJECT_FOUNDATION_INVALID_METADATA_PROBE",
    "RUNTIME_OBJECT_FOUNDATION_PROBE",
    "check_runtime_object_foundation_protocol_category_case",
]
