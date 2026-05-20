"""Async error and foreign-boundary runtime-trace acceptance case."""

from __future__ import annotations

import json
import re
from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.fixture_compilation import (
    NegativeDiagnosticExpectation,
    compile_live_error_runtime_fixture_outputs,
    compile_negative_diagnostic_batch,
)
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe


def _semantic_surface(manifest: dict[str, object], field: str) -> dict[str, object]:
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get(field, {})
    )
    expect(isinstance(surface, dict), f"expected manifest to publish {field}")
    return surface


def _assert_compile_surfaces(manifest: dict[str, object]) -> dict[str, object]:
    effects = _semantic_surface(manifest, "objc_effects_ownership_semantic_model")
    bridge = _semantic_surface(manifest, "objc_error_handling_error_bridge_legality")
    task = _semantic_surface(
        manifest, "objc_concurrency_task_runtime_lowering_contract"
    )
    interop = _semantic_surface(
        manifest, "objc_interop_foreign_call_and_lifetime_lowering"
    )

    expected_effect_minimums = {
        "throws_propagation_sites": 1,
        "bridged_error_sites": 1,
        "nested_cleanup_sites": 1,
        "foreign_boundary_sites": 1,
        "async_continuation_sites": 1,
        "cancellation_propagation_sites": 1,
    }
    for field, minimum in expected_effect_minimums.items():
        expect(
            int(effects.get(field, 0)) >= minimum,
            f"expected async error/foreign-boundary fixture to publish {field}",
        )
    for field in (
        "throws_cleanup_semantics_landed",
        "async_task_semantics_landed",
        "foreign_boundary_semantics_landed",
        "deterministic",
    ):
        expect(
            effects.get(field) is True,
            f"expected async error/foreign-boundary effects surface to preserve {field}",
        )

    expect(
        bridge.get("bridge_callable_sites") == 1
        and bridge.get("objc_status_code_callable_sites") == 1
        and bridge.get("try_eligible_bridge_callable_sites") == 1
        and bridge.get("unsupported_combinations_fail_closed") is True
        and bridge.get("ready_for_lowering_and_runtime") is True,
        "expected async error/foreign-boundary fixture to preserve status bridge legality",
    )
    expect(
        task.get("deterministic_handoff") is True
        and task.get("ready_for_ir_emission") is True
        and int(task.get("task_group_scope_sites", 0)) >= 1
        and int(task.get("task_group_cancel_all_sites", 0)) >= 1
        and int(task.get("cancellation_probe_sites", 0)) >= 1
        and int(task.get("cancellation_handler_sites", 0)) >= 1
        and int(task.get("runtime_cancel_sites", 0)) >= 1,
        "expected async error/foreign-boundary fixture to preserve task cancellation lowering metadata",
    )
    expect(
        interop.get("contract_id")
        == "objc3c.interop.foreign.call.and.lifetime.lowering.v1"
        and int(interop.get("foreign_callable_sites", 0)) >= 1
        and int(interop.get("metadata_preservation_sites", 0)) >= 1
        and int(interop.get("contract_violation_sites", 0)) == 0,
        "expected async error/foreign-boundary fixture to preserve foreign-call lowering metadata without contract violations",
    )

    return {
        "effects_replay_key": effects.get("replay_key"),
        "bridge_callable_sites": bridge.get("bridge_callable_sites"),
        "task_cancellation_probe_sites": task.get("cancellation_probe_sites"),
        "task_runtime_cancel_sites": task.get("runtime_cancel_sites"),
        "foreign_callable_sites": interop.get("foreign_callable_sites"),
        "foreign_metadata_preservation_sites": interop.get(
            "metadata_preservation_sites"
        ),
    }


def _assert_ir_coupling(ir_text: str) -> dict[str, object]:
    required_tokens = {
        "throw_store": "call void @objc3_runtime_store_thrown_error_i32",
        "throw_load": "call i32 @objc3_runtime_load_thrown_error_i32",
        "status_bridge": "call i32 @objc3_runtime_bridge_status_error_i32",
        "catch_match": "call i32 @objc3_runtime_catch_matches_error_i32",
        "cleanup_resource": "call void @cleanup_scope_close_fd",
        "cleanup_defer": "call void @cleanup_release_temp(i32 6)",
        "autoreleasepool_push": "call void @objc3_runtime_push_autoreleasepool_scope",
        "autoreleasepool_pop": "call void @objc3_runtime_pop_autoreleasepool_scope",
        "foreign_entry": "call i32 @foreignEntry",
        "task_cancel_all": "call i32 @objc3_runtime_cancel_task_group_i32",
        "task_on_cancel": "call i32 @objc3_runtime_task_on_cancel_i32",
        "await_handoff": "call i32 @objc3_runtime_handoff_async_continuation_to_executor_i32",
        "await_resume": "call i32 @objc3_runtime_resume_async_continuation_i32",
    }
    for label, token in required_tokens.items():
        expect(token in ir_text, f"expected IR to preserve {label}")

    try_failure_index = ir_text.find("try_fail_")
    catch_dispatch_index = ir_text.find("do_catch_dispatch_", try_failure_index)
    expect(
        try_failure_index >= 0 and catch_dispatch_index > try_failure_index,
        "expected thrown bridged flow to expose cleanup before catch dispatch",
    )
    failure_cleanup_segment = ir_text[try_failure_index:catch_dispatch_index]
    resource_cleanup_index = failure_cleanup_segment.find(
        "call void @cleanup_scope_close_fd"
    )
    defer_cleanup_index = failure_cleanup_segment.find(
        "call void @cleanup_release_temp(i32 6)"
    )
    expect(
        0 <= resource_cleanup_index < defer_cleanup_index,
        "expected nested thrown cleanup to preserve resource-before-defer LIFO order",
    )

    cancellation_body_match = re.search(
        r"define\s+[^@]+@cancellationTrace\([^)]*\)\s*\{\n(.*?)\n\}",
        ir_text,
        flags=re.DOTALL,
    )
    expect(
        cancellation_body_match is not None,
        "expected IR to include cancellationTrace async body",
    )
    cancellation_body = cancellation_body_match.group(1)
    defer_index = cancellation_body.find("call i32 @objc3_runtime_task_on_cancel_i32")
    pop_index = cancellation_body.find(
        "call void @objc3_runtime_pop_autoreleasepool_scope"
    )
    handoff_index = cancellation_body.find(
        "call i32 @objc3_runtime_handoff_async_continuation_to_executor_i32"
    )
    expect(
        0 <= defer_index < pop_index < handoff_index,
        "expected async cancellation cleanup to run before continuation handoff",
    )

    return {
        "required_ir_tokens": sorted(required_tokens),
        "thrown_cleanup_order": "resource-before-defer-before-catch-dispatch",
        "async_cleanup_order": "on-cancel-defer-before-autoreleasepool-pop-before-handoff",
    }


def _assert_runtime_trace_payload(payload: dict[str, object]) -> dict[str, object]:
    expected_fields = {
        "error_status": 0,
        "loaded": 34,
        "bridged_status": 45,
        "bridged_nserror": 77,
        "bridged_foreign": 99,
        "missing_foreign_payload": -3902,
        "unsupported_foreign_kind": -3901,
        "match_foreign": 1,
        "match_catch_all": 1,
        "error_store_call_count": 1,
        "error_load_call_count": 1,
        "error_status_bridge_call_count": 1,
        "error_nserror_bridge_call_count": 1,
        "error_foreign_exception_bridge_call_count": 3,
        "error_catch_match_call_count": 2,
        "last_foreign_exception_kind": 9,
        "last_foreign_exception_payload_value": 88,
        "last_foreign_exception_mapped_error_value": 99,
        "last_foreign_exception_bridge_result": -3901,
        "task_status": 0,
        "task_scope": 1,
        "task_add_task": 1,
        "task_add_second_task": 1,
        "task_cancel_all": 31,
        "task_scope_call_count": 1,
        "task_add_task_call_count": 2,
        "task_cancel_all_call_count": 1,
        "task_lifecycle_state": 4,
        "task_selected_executor_tag": 6,
        "task_active_group_executor_tag": 6,
        "task_active_group_task_count": 2,
        "task_pending_group_task_count": 0,
        "task_completed_group_task_count": 0,
        "task_cancelled_group_task_count": 2,
        "task_group_cancelled": 1,
        "task_cancellation_generation": 1,
        "task_last_queue_depth": 0,
        "task_last_queue_drain_result": 28,
        "task_scheduler_enqueue_count": 2,
        "task_scheduler_dequeue_count": 0,
        "task_scheduler_cancelled_count": 2,
        "task_last_cancelled_task_handle": 28,
        "task_last_cancelled_executor_tag": 6,
        "task_deadlock_guard_passed": 1,
        "task_race_guard_passed": 1,
        "task_replay_equal": 1,
    }
    for field, expected in expected_fields.items():
        expect(
            payload.get(field) == expected,
            f"expected async error/foreign-boundary runtime trace to preserve {field}",
        )
    expect(
        payload.get("last_foreign_exception_kind_name")
        == "unsupported-foreign-exception",
        "expected runtime trace to preserve the unsupported foreign exception diagnostic label",
    )
    expect(
        payload.get("last_catch_kind_name") == "unknown",
        "expected runtime trace to preserve the catch-all diagnostic label",
    )
    return {
        "error_fields": {
            key: payload[key]
            for key in (
                "bridged_foreign",
                "missing_foreign_payload",
                "unsupported_foreign_kind",
                "last_foreign_exception_kind_name",
            )
        },
        "task_fields": {
            key: payload[key]
            for key in (
                "task_lifecycle_state",
                "task_cancelled_group_task_count",
                "task_scheduler_cancelled_count",
                "task_replay_equal",
            )
        },
    }


def check_async_error_foreign_boundary_runtime_trace_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "async-error-foreign-boundary-runtime-trace"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "async_error_foreign_boundary_trace_positive.objc3"
    )
    _, ll_path, manifest_path = compile_live_error_runtime_fixture_outputs(
        fixture, case_dir / "compile", ["-fobjc-arc"]
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    compile_summary = _assert_compile_surfaces(manifest)
    ir_summary = _assert_ir_coupling(ll_path.read_text(encoding="utf-8"))

    probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "async_error_foreign_boundary_trace_probe.cpp"
    )
    exe_path = case_dir / "async_error_foreign_boundary_trace_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_key_value_output(
        run_probe(exe_path), "async error foreign-boundary runtime trace probe"
    )
    runtime_summary = _assert_runtime_trace_payload(payload)

    negative_batch = compile_negative_diagnostic_batch(
        case_id="async-error-foreign-boundary-runtime-trace",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="async_throws_rejected",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "async_throws_rejected.objc3",
                expected_snippets=[
                    "async throws functions remain unsupported until async error propagation lands"
                ],
                expected_codes=["O3S226"],
            ),
            NegativeDiagnosticExpectation(
                key="bridge_legality_throws_conflict",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "bridge_legality_throws_conflict_negative.objc3",
                expected_snippets=[
                    "NSError/status bridge markers cannot currently be combined with throws"
                ],
                expected_codes=["O3S277"],
            ),
            NegativeDiagnosticExpectation(
                key="foreign_type_non_string",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "recovery"
                / "negative"
                / "negative_parser_draft_syntax_foreign_type_non_string.objc3",
                expected_snippets=[
                    "objc_foreign_type named payload requires string literal"
                ],
                expected_codes=["O3P343"],
            ),
        ],
    )

    return CaseResult(
        case_id="async-error-foreign-boundary-runtime-trace",
        probe="tests/tooling/runtime/async_error_foreign_boundary_trace_probe.cpp",
        fixture="tests/tooling/fixtures/native/async_error_foreign_boundary_trace_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "compile": compile_summary,
            "ir": ir_summary,
            "runtime": runtime_summary,
            "negative_diagnostics_batch": negative_batch,
        },
    )


__all__ = ["check_async_error_foreign_boundary_runtime_trace_case"]
