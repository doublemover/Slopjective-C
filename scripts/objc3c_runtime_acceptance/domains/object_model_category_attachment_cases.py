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
NATIVE_EXECUTION_NEGATIVE_ROOT = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "negative"
)


def _assert_protocol_category_payload(payload: dict[str, Any]) -> None:
    graph = payload.get("graph_state", {})
    widget = payload.get("widget_entry", {})
    base = payload.get("base_entry", {})
    worker = payload.get("worker_query", {})
    tracer = payload.get("tracer_query", {})
    base_worker = payload.get("base_worker_query", {})
    derived_worker = payload.get("derived_worker_query", {})
    missing_protocol = payload.get("missing_protocol_query", {})
    missing_class = payload.get("missing_class_query", {})
    category_first_state = payload.get("category_first_state", {})
    category_second_state = payload.get("category_second_state", {})
    method_state = payload.get("method_state", {})
    category_entry = payload.get("category_entry", {})
    strict_error_entry = payload.get("strict_error_entry", {})
    category_bool_typed_result = payload.get("category_bool_typed_result", {})
    strict_error_i32_result = payload.get("protocol_strict_error_i32_result", {})
    strict_error_typed_result = payload.get(
        "protocol_strict_error_typed_result", {}
    )

    expect(
        payload.get("report_contract")
        == "objc3c.runtime.category-attachment-protocol.report.v2"
        and payload.get("source_path")
        == "tests/tooling/runtime/category_attachment_protocol_runtime_probe.cpp",
        "expected source-stable category/protocol runtime report identity",
    )
    expect(payload.get("category_value") == 13, "expected category dispatch to return 13")
    expect(
        payload.get("category_cached_value") == 13,
        "expected cached category dispatch to return 13",
    )
    expect(
        payload.get("auxiliary_category_value") == 29,
        "expected second attached category dispatch to return 29",
    )
    expect(
        payload.get("category_bool_value") == 1
        and category_bool_typed_result.get("status_code") == 0
        and category_bool_typed_result.get("return_kind") == 2
        and category_bool_typed_result.get("return_kind_name") == "bool"
        and category_bool_typed_result.get("bool_value") == 1
        and category_bool_typed_result.get("i32_value") == 0
        and category_bool_typed_result.get("result_contract")
        == "typed-dispatch-value-result",
        "expected category bool dispatch to use typed bool value marshalling",
    )
    expect(payload.get("class_value") == 11, "expected class dispatch to return 11")
    expect(
        payload.get("super_inherited_value") == 7,
        "expected super lookup start to resolve inherited Base method",
    )
    expect(
        payload.get("nil_receiver_value") == 0,
        "expected nil receiver category dispatch to return zero without live lookup",
    )
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
        graph.get("attached_category_count") == 2
        and graph.get("protocol_conformance_edge_count", 0) >= 2,
        "expected realized graph to expose attached category and protocol edges",
    )
    expect(
        widget.get("attached_category_count") == 2
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
        missing_protocol.get("class_found") == 1
        and missing_protocol.get("protocol_found") == 0
        and missing_protocol.get("conforms") == 0
        and missing_protocol.get("malformed_metadata") == 0
        and missing_protocol.get("class_name") == "Widget"
        and missing_protocol.get("protocol_name") == "MissingProtocol",
        "expected missing protocol conformance query to fail closed without malformed metadata",
    )
    expect(
        missing_class.get("class_found") == 0
        and missing_class.get("protocol_found") == 1
        and missing_class.get("conforms") == 0
        and missing_class.get("malformed_metadata") == 0
        and missing_class.get("class_name") == "MissingConformanceClass"
        and missing_class.get("protocol_name") == "Worker",
        "expected missing class conformance query to fail closed without malformed metadata",
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
        and category_first_state.get("last_category_probe_count", 0) == 1,
        "expected first category dispatch to resolve through newest category in merged lookup",
    )
    expect(
        category_second_state.get("last_selector") == "tracedValue"
        and category_second_state.get("last_dispatch_used_cache") == 1
        and category_second_state.get("last_dispatch_resolved_live_method") == 1
        and category_second_state.get("last_category_probe_count", 0) == 1,
        "expected second category dispatch to hit the merged dispatch cache",
    )
    expect(
        category_entry.get("found") == 1
        and category_entry.get("resolved") == 1
        and category_entry.get("selector") == "tracedValue"
        and category_entry.get("resolved_class_name") == "Widget"
        and category_entry.get("resolved_owner_identity")
        == "implementation:Widget(Tracing)::instance_method:tracedValue"
        and category_entry.get("category_probe_count", 0) == 1,
        "expected tracedValue cache entry to preserve newest category lookup order",
    )
    expect(
        strict_error_entry.get("found") == 1
        and strict_error_entry.get("resolved") == 0
        and strict_error_entry.get("selector") == "ignoredValue"
        and strict_error_entry.get("category_probe_count", 0) >= 2
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
    forward_protocol = payload.get("forward_protocol_reference", {})
    missing_category = payload.get("missing_category_target", {})
    conflicting_category = payload.get("conflicting_category_owner", {})
    duplicate_requirement = payload.get("duplicate_protocol_requirement", {})
    inherited_requirement = payload.get(
        "inherited_protocol_requirement_conflict", {}
    )
    adopted_requirement = payload.get("adopted_protocol_requirement_conflict", {})
    forward_inherited = payload.get("forward_inherited_protocol_reference", {})
    invalid_cases = {
        "unregistered-protocol-reference": payload,
        "forward-protocol-reference": forward_protocol,
        "missing-category-target": missing_category,
        "conflicting-category-owner": conflicting_category,
    }
    for case_id, case_payload in invalid_cases.items():
        expect(
            case_payload.get("case_id") == case_id,
            f"expected stable invalid-metadata case id {case_id}",
        )
        expect(
            case_payload.get("source_path")
            == "tests/tooling/runtime/protocol_category_invalid_metadata_probe.cpp",
            f"expected {case_id} to publish a source-stable probe path",
        )
        expect(
            case_payload.get("registration_status_name")
            == "OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS",
            f"expected {case_id} to publish stable registration status name",
        )
        expect(
            case_payload.get("diagnostic_code") == "O3RT004"
            and case_payload.get("snapshot_diagnostic_code") == "O3RT004"
            and case_payload.get("diagnostic_class") == "malformed-runtime-metadata"
            and case_payload.get("snapshot_diagnostic_class")
            == "malformed-runtime-metadata",
            f"expected {case_id} to publish stable malformed metadata diagnostic fields",
        )
        expect(
            case_payload.get("reason_matches_expected") is True,
            f"expected {case_id} to publish expected reason match evidence",
        )
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
        payload.get("target_kind") == "protocol"
        and payload.get("visibility_state") == "unregistered"
        and payload.get("availability_state") == "unavailable",
        "expected structured visibility and availability denial fields for unregistered protocol refs",
    )
    expect(
        forward_protocol.get("registration_status") == -4
        and forward_protocol.get("last_registration_status") == -4,
        "expected forward protocol conformance metadata to fail image registration",
    )
    expect(
        forward_protocol.get("registered_image_count") == 0
        and forward_protocol.get("realized_class_count") == 0
        and forward_protocol.get("class_found") == 0,
        "expected forward protocol conformance metadata to publish no image or class graph",
    )
    expect(
        forward_protocol.get("malformed_class_metadata_rejection_count", 0) >= 1,
        "expected forward protocol conformance metadata to increment malformed metadata rejection count",
    )
    expect(
        forward_protocol.get("last_malformed_class_graph_reason")
        == "forward protocol reference in class ForwardProtocolRef",
        "expected stable fail-closed diagnostic reason for forward protocol refs",
    )
    expect(
        forward_protocol.get("target_kind") == "protocol"
        and forward_protocol.get("visibility_state") == "forward-declaration"
        and forward_protocol.get("availability_state") == "unavailable-for-conformance",
        "expected structured visibility and availability denial fields for forward protocol refs",
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
    expect(
        missing_category.get("target_kind") == "class"
        and missing_category.get("category_name") == "Tracing"
        and missing_category.get("availability_state") == "missing-target",
        "expected structured unavailable-target fields for missing category target",
    )
    expect(
        conflicting_category.get("registration_status") == -4
        and conflicting_category.get("last_registration_status") == -4,
        "expected conflicting category owner metadata to fail image registration",
    )
    expect(
        conflicting_category.get("registered_image_count") == 0
        and conflicting_category.get("realized_class_count") == 0
        and conflicting_category.get("class_found") == 0,
        "expected conflicting category owner metadata to publish no image or class graph",
    )
    expect(
        conflicting_category.get("malformed_class_metadata_rejection_count", 0)
        >= 1,
        "expected conflicting category owner metadata to increment malformed metadata rejection count",
    )
    expect(
        conflicting_category.get("last_malformed_class_graph_reason")
        == "conflicting category implementation owner for ConflictOwner(Tracing)",
        "expected stable fail-closed diagnostic reason for category owner conflicts",
    )
    expect(
        duplicate_requirement.get("registration_status") == -4
        and duplicate_requirement.get("last_registration_status") == -4,
        "expected duplicate/conflicting protocol requirements to fail image registration",
    )
    expect(
        duplicate_requirement.get("registered_image_count") == 0
        and duplicate_requirement.get("realized_class_count") == 0,
        "expected duplicate/conflicting protocol requirements to publish no image or class graph",
    )
    expect(
        duplicate_requirement.get("last_malformed_class_graph_reason")
        == "conflicting protocol instance method requirement work in protocol Worker",
        "expected stable fail-closed diagnostic reason for duplicate protocol requirements",
    )
    expect(
        inherited_requirement.get("registration_status") == -4
        and inherited_requirement.get("last_registration_status") == -4,
        "expected inherited protocol requirement conflicts to fail image registration",
    )
    expect(
        inherited_requirement.get("registered_image_count") == 0
        and inherited_requirement.get("realized_class_count") == 0,
        "expected inherited protocol requirement conflicts to publish no image or class graph",
    )
    expect(
        inherited_requirement.get("last_malformed_class_graph_reason")
        == (
            "conflicting protocol instance method requirement readValue in protocol "
            "MixedReadable"
        ),
        "expected stable fail-closed diagnostic reason for inherited protocol requirement conflicts",
    )
    expect(
        adopted_requirement.get("registration_status") == -4
        and adopted_requirement.get("last_registration_status") == -4,
        "expected adopted protocol requirement conflicts to fail image registration",
    )
    expect(
        adopted_requirement.get("registered_image_count") == 0
        and adopted_requirement.get("realized_class_count") == 0
        and adopted_requirement.get("class_found") == 0,
        "expected adopted protocol requirement conflicts to publish no image or class graph",
    )
    expect(
        adopted_requirement.get("last_malformed_class_graph_reason")
        == (
            "conflicting protocol instance method requirement readValue in class "
            "RequirementConflictAdopter"
        ),
        "expected stable fail-closed diagnostic reason for adopted protocol requirement conflicts",
    )
    expect(
        forward_inherited.get("registration_status") == -4
        and forward_inherited.get("last_registration_status") == -4,
        "expected inherited forward protocol references to fail image registration",
    )
    expect(
        forward_inherited.get("registered_image_count") == 0
        and forward_inherited.get("realized_class_count") == 0,
        "expected inherited forward protocol references to publish no image or class graph",
    )
    expect(
        forward_inherited.get("last_malformed_class_graph_reason")
        == "forward protocol reference in protocol ConcreteChild",
        "expected stable fail-closed diagnostic reason for inherited forward protocol refs",
    )
    expect(
        conflicting_category.get("target_kind") == "category"
        and conflicting_category.get("category_name") == "Tracing"
        and conflicting_category.get("availability_state") == "conflicting-owner",
        "expected structured conflict fields for duplicate category owners",
    )


def _assert_category_conflict_diagnostic_report(report: dict[str, Any]) -> None:
    expect(
        report.get("contract_id")
        == "objc3c.runtime.acceptance.negative.diagnostics.batch.v1",
        "expected category/protocol diagnostic batch contract id",
    )
    expect(
        report.get("fixture_count") == 12,
        "expected full protocol/category negative diagnostic matrix",
    )
    for result in report.get("results", []):
        key = result.get("key", "")
        expected_codes = set(result.get("expected_codes", []))
        diagnostic_codes = set(result.get("diagnostic_codes", []))
        expect(expected_codes <= diagnostic_codes, f"{key} lost expected codes")
        expect(
            isinstance(result.get("sidecar"), str)
            and result["sidecar"].endswith(".meta.json"),
            f"{key} should publish a durable negative fixture sidecar",
        )
        expect(
            result.get("sidecar_stage") == "compile",
            f"{key} should be a compile-stage protocol/category diagnostic",
        )
        sidecar_tokens = result.get("sidecar_required_diagnostic_tokens", [])
        for expected_code in expected_codes:
            expect(
                expected_code in sidecar_tokens,
                f"{key} sidecar should preserve expected diagnostic code {expected_code}",
            )
        locations = result.get("diagnostic_locations", [])
        expect(
            any(
                location.get("code") in expected_codes
                and location.get("line", 0) > 0
                and location.get("column", 0) > 0
                for location in locations
            ),
            f"{key} should publish source-stable diagnostic line and column fields",
        )


def _compile_category_conflict_diagnostics(case_dir: Path) -> dict[str, Any]:
    return compile_negative_diagnostic_batch(
        case_id=RUNTIME_OBJECT_FOUNDATION_CASE_ID,
        out_dir=case_dir / "category-conflicts",
        expectations=[
            NegativeDiagnosticExpectation(
                key="category-attachment-collision",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "category_attachment_collision.objc3",
                expected_snippets=[
                    "duplicate category interface 'Root(Extras)'",
                ],
                expected_codes=["O3S200"],
            ),
            NegativeDiagnosticExpectation(
                key="category-merge-conflicting-method",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "category_merge_conflicting_method.objc3",
                expected_snippets=[
                    "category merge failure: category 'Widget(Beta)' selector '+value' conflicts with attached category 'Widget(Alpha)'",
                ],
                expected_codes=["O3S219"],
            ),
            NegativeDiagnosticExpectation(
                key="category-merge-conflicting-property",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "category_merge_conflicting_property.objc3",
                expected_snippets=[
                    "category merge failure: category 'Widget(Beta)' property 'token' conflicts with attached category 'Widget(Alpha)'",
                ],
                expected_codes=["O3S219"],
            ),
            NegativeDiagnosticExpectation(
                key="category-merge-missing-pair",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "category_merge_missing_pair.objc3",
                expected_snippets=[
                    "category merge failure: category interface 'Widget(Debug)' is missing category implementation for realized class 'Widget'",
                ],
                expected_codes=["O3S219"],
            ),
            NegativeDiagnosticExpectation(
                key="category-unknown-class-rejected",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "category_unknown_class_rejected.objc3",
                expected_snippets=[
                    "category attachment failure: category interface 'MissingOwner(Tracing)' attaches to unknown class 'MissingOwner'",
                    "category attachment failure: category implementation 'MissingOwner(Tracing)' attaches to unknown class 'MissingOwner'",
                ],
                expected_codes=["O3S219"],
            ),
            NegativeDiagnosticExpectation(
                key="category-unavailable-class-rejected",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "category_unavailable_class_rejected.objc3",
                expected_snippets=[
                    "category attachment failure: category interface 'Widget(Tracing)' attaches to unavailable class 'Widget' without a realized implementation",
                    "category attachment failure: category implementation 'Widget(Tracing)' attaches to unavailable class 'Widget' without a realized implementation",
                ],
                expected_codes=["O3S219"],
            ),
            NegativeDiagnosticExpectation(
                key="protocol-requirement-duplicate-conflict",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "protocol_requirement_duplicate_conflict_rejected.objc3",
                expected_snippets=[
                    "protocol requirement conflict: duplicate selector '-work' in protocol 'Worker'",
                ],
                expected_codes=["O3S218"],
            ),
            NegativeDiagnosticExpectation(
                key="protocol-requirement-inherited-conflict",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "protocol_requirement_inherited_conflict_rejected.objc3",
                expected_snippets=[
                    "protocol requirement conflict: protocol 'MixedReadable' inherits incompatible selector '-readValue' from protocol 'TextReadable'",
                ],
                expected_codes=["O3S218"],
            ),
            NegativeDiagnosticExpectation(
                key="protocol-dispatch-intent-rejected",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "protocol_dispatch_intent_rejected.objc3",
                expected_snippets=[
                    "dispatch-control semantics failed: selector '-shared' in protocol 'P' cannot use Part 9 dispatch-control callable attributes",
                ],
                expected_codes=["O3S314"],
            ),
            NegativeDiagnosticExpectation(
                key="category-method-dispatch-intent-rejected",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "category_method_dispatch_intent_rejected.objc3",
                expected_snippets=[
                    "dispatch-control semantics failed: selector '-shared' in category 'Box(Ext)' cannot use Part 9 dispatch-control callable attributes",
                ],
                expected_codes=["O3S315"],
            ),
            NegativeDiagnosticExpectation(
                key="category-container-dispatch-intent-rejected",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
                / "category_container_dispatch_intent_rejected.objc3",
                expected_snippets=[
                    "dispatch-control semantics failed: category 'Box(Ext)' cannot use objc_direct_members, objc_final, or objc_sealed container attributes",
                ],
                expected_codes=["O3S316"],
            ),
            NegativeDiagnosticExpectation(
                key="duplicate-protocol-runtime-export",
                fixture=NATIVE_EXECUTION_NEGATIVE_ROOT
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
    _assert_category_conflict_diagnostic_report(conflict_diagnostics)

    return CaseResult(
        case_id=RUNTIME_OBJECT_FOUNDATION_CASE_ID,
        probe=RUNTIME_OBJECT_FOUNDATION_PROBE,
        fixture=RUNTIME_OBJECT_FOUNDATION_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "category_value": payload["category_value"],
            "category_cached_value": payload["category_cached_value"],
            "auxiliary_category_value": payload["auxiliary_category_value"],
            "category_bool_value": payload["category_bool_value"],
            "category_bool_status": payload["category_bool_typed_result"][
                "status_code"
            ],
            "class_value": payload["class_value"],
            "super_inherited_value": payload["super_inherited_value"],
            "nil_receiver_value": payload["nil_receiver_value"],
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
            "missing_protocol_query_protocol_found": payload[
                "missing_protocol_query"
            ]["protocol_found"],
            "missing_class_query_class_found": payload["missing_class_query"][
                "class_found"
            ],
            "category_conflict_diagnostic_count": conflict_diagnostics[
                "results"
            ][0]["diagnostic_count"],
            "category_protocol_negative_fixture_count": conflict_diagnostics[
                "fixture_count"
            ],
            "category_protocol_negative_diagnostic_results": conflict_diagnostics[
                "results"
            ],
            "invalid_protocol_metadata_registration_status": invalid_metadata_payload[
                "registration_status"
            ],
            "invalid_protocol_metadata_diagnostic_code": invalid_metadata_payload[
                "diagnostic_code"
            ],
            "invalid_protocol_metadata_reason": invalid_metadata_payload[
                "last_malformed_class_graph_reason"
            ],
            "forward_protocol_reference_registration_status": invalid_metadata_payload[
                "forward_protocol_reference"
            ]["registration_status"],
            "forward_protocol_reference_reason": invalid_metadata_payload[
                "forward_protocol_reference"
            ]["last_malformed_class_graph_reason"],
            "missing_category_target_registration_status": invalid_metadata_payload[
                "missing_category_target"
            ]["registration_status"],
            "missing_category_target_reason": invalid_metadata_payload[
                "missing_category_target"
            ]["last_malformed_class_graph_reason"],
            "conflicting_category_owner_registration_status": invalid_metadata_payload[
                "conflicting_category_owner"
            ]["registration_status"],
            "conflicting_category_owner_reason": invalid_metadata_payload[
                "conflicting_category_owner"
            ]["last_malformed_class_graph_reason"],
            "duplicate_protocol_requirement_reason": invalid_metadata_payload[
                "duplicate_protocol_requirement"
            ]["last_malformed_class_graph_reason"],
            "inherited_protocol_requirement_conflict_reason": invalid_metadata_payload[
                "inherited_protocol_requirement_conflict"
            ]["last_malformed_class_graph_reason"],
            "adopted_protocol_requirement_conflict_reason": invalid_metadata_payload[
                "adopted_protocol_requirement_conflict"
            ]["last_malformed_class_graph_reason"],
            "forward_inherited_protocol_reference_reason": invalid_metadata_payload[
                "forward_inherited_protocol_reference"
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
