"""Block/ARC runtime ABI and helper execution acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.commands import run
from objc3c_runtime_acceptance.native_build import (
    ROOT,
    compile_fixture_outputs,
    compile_fixture_outputs_with_args,
    link_fixture_executable,
)
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
    BLOCK_ARC_RUNTIME_ABI_PROBE,
    BLOCK_ARC_RUNTIME_ARC_MODEL,
    BLOCK_ARC_RUNTIME_BLOCK_MODEL,
    BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
)

def check_block_arc_runtime_abi_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "block-arc-runtime-abi"
    probe = ROOT / Path(BLOCK_ARC_RUNTIME_ABI_PROBE)
    exe_path = case_dir / "block_arc_runtime_abi_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_json_output(run_probe(exe_path), "block ARC runtime ABI probe")

    expected_string_fields = {
        "block_promote_symbol": "objc3_runtime_promote_block_i32",
        "block_invoke_symbol": "objc3_runtime_invoke_block_i32",
        "retain_symbol": "objc3_runtime_retain_i32",
        "release_symbol": "objc3_runtime_release_i32",
        "autorelease_symbol": "objc3_runtime_autorelease_i32",
        "autoreleasepool_push_symbol": "objc3_runtime_push_autoreleasepool_scope",
        "autoreleasepool_pop_symbol": "objc3_runtime_pop_autoreleasepool_scope",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": (
            "objc3_runtime_load_weak_current_property_i32"
        ),
        "weak_current_property_store_symbol": (
            "objc3_runtime_store_weak_current_property_i32"
        ),
        "arc_debug_state_snapshot_symbol": (
            "objc3_runtime_copy_arc_debug_state_for_testing"
        ),
        "runtime_abi_boundary_model": BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
        "block_runtime_model": BLOCK_ARC_RUNTIME_BLOCK_MODEL,
        "arc_runtime_model": BLOCK_ARC_RUNTIME_ARC_MODEL,
        "fail_closed_model": BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
    }
    for field, expected_value in expected_string_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected block ARC runtime ABI probe to preserve {field}",
        )

    expected_integer_fields = {
        "abi_status": 0,
        "arc_status": 0,
        "retained": 77,
        "autoreleased": 77,
        "released": 77,
        "invoke_result": 17,
        "private_runtime_abi_ready": 1,
        "public_runtime_header_unchanged": 1,
        "deterministic": 1,
        "live_runtime_block_handle_count": 0,
        "block_promote_call_count": 1,
        "block_invoke_call_count": 1,
        "retain_call_count": 2,
        "release_call_count": 3,
        "autorelease_call_count": 1,
        "autoreleasepool_push_count": 1,
        "autoreleasepool_pop_count": 1,
        "current_property_read_count": 0,
        "current_property_write_count": 0,
        "current_property_exchange_count": 0,
        "weak_current_property_load_count": 0,
        "weak_current_property_store_count": 0,
        "last_promote_has_pointer_capture_storage": 1,
        "last_block_invoke_result": 17,
        "last_autorelease_value": 77,
        "arc_retain_call_count": 2,
        "arc_release_call_count": 3,
        "arc_autorelease_call_count": 1,
        "arc_autoreleasepool_push_count": 1,
        "arc_autoreleasepool_pop_count": 1,
    }
    for field, expected_value in expected_integer_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected block ARC runtime ABI probe to preserve {field}",
        )

    handle = payload.get("handle")
    expect(isinstance(handle, int) and handle > 0, "expected block ARC runtime ABI probe to publish a live promoted block handle")
    expect(
        payload.get("retain_handle_result") == handle
        and payload.get("release_handle_result") == handle
        and payload.get("final_release_result") == handle,
        "expected block ARC runtime ABI probe to preserve block handle retain/release traffic",
    )
    expect(
        payload.get("last_promoted_block_handle") == handle
        and payload.get("last_invoked_block_handle") == handle,
        "expected block ARC runtime ABI probe to preserve the last promoted and invoked block handle",
    )
    expect(
        payload.get("last_retain_value") == handle
        and payload.get("last_release_value") == handle,
        "expected block ARC runtime ABI probe to preserve the last ARC retain/release value",
    )

    return CaseResult(
        case_id="block-arc-runtime-abi",
        probe=BLOCK_ARC_RUNTIME_ABI_PROBE,
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "handle": handle,
            "invoke_result": payload.get("invoke_result"),
            "block_promote_call_count": payload.get("block_promote_call_count"),
            "block_invoke_call_count": payload.get("block_invoke_call_count"),
            "retain_call_count": payload.get("retain_call_count"),
            "release_call_count": payload.get("release_call_count"),
            "autorelease_call_count": payload.get("autorelease_call_count"),
        },
    )
def check_block_helper_runtime_execution_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "block-helper-runtime-execution"

    byref_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "byref_cell_copy_dispose_runtime_positive.objc3"
    )
    byref_obj, _, byref_manifest_path = compile_fixture_outputs(
        byref_fixture, case_dir / "byref-runtime-positive"
    )
    byref_manifest = json.loads(byref_manifest_path.read_text(encoding="utf-8"))
    byref_copy_dispose_surface = (
        byref_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )
    byref_exe = case_dir / "byref-runtime-positive" / "byref_runtime.exe"
    link_fixture_executable(clangxx, byref_obj, byref_exe)
    byref_run = run([str(byref_exe)])
    expect(
        byref_run.returncode == 14,
        f"expected byref runtime positive fixture to exit 14, saw {byref_run.returncode}",
    )
    expect(
        byref_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and byref_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected byref runtime positive fixture to require copy/dispose helpers",
    )
    byref_forwarding_probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "block_runtime_byref_forwarding_probe.cpp"
    )
    byref_forwarding_exe = (
        case_dir / "block_runtime_byref_forwarding_probe.exe"
    )
    compile_probe(clangxx, byref_forwarding_probe, byref_forwarding_exe, [])
    byref_forwarding_payload = parse_json_output(
        run_probe(byref_forwarding_exe),
        "block runtime byref forwarding probe",
    )
    expect(
        isinstance(byref_forwarding_payload.get("handle"), int)
        and byref_forwarding_payload.get("handle", 0) > 0,
        "expected byref forwarding probe to publish a positive runtime block handle",
    )
    expect(
        byref_forwarding_payload.get("copy_count_after_promotion") == 1,
        "expected byref forwarding probe to execute one copy helper during promotion",
    )
    expect(
        byref_forwarding_payload.get("first_invoke_result") == 23
        and byref_forwarding_payload.get("second_invoke_result") == 25,
        "expected byref forwarding probe to preserve runtime-owned forwarded cell state across invokes",
    )
    expect(
        byref_forwarding_payload.get("dispose_count_before_final_release") == 0
        and byref_forwarding_payload.get("dispose_count_after_final_release") == 1,
        "expected byref forwarding probe to defer dispose helper execution until final release",
    )
    expect(
        byref_forwarding_payload.get("last_disposed_value") == 11,
        "expected byref forwarding probe to dispose the original owned capture payload",
    )
    expect(
        byref_forwarding_payload.get("final_release_result")
        == byref_forwarding_payload.get("handle")
        and byref_forwarding_payload.get("invoke_after_release_result") == 0,
        "expected byref forwarding probe to release the block handle and reject post-release invocation",
    )

    owned_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "owned_object_capture_runtime_positive.objc3"
    )
    owned_obj, _, owned_manifest_path = compile_fixture_outputs(
        owned_fixture, case_dir / "owned-runtime-positive"
    )
    owned_manifest = json.loads(owned_manifest_path.read_text(encoding="utf-8"))
    owned_copy_dispose_surface = (
        owned_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )
    owned_exe = case_dir / "owned-runtime-positive" / "owned_runtime.exe"
    link_fixture_executable(clangxx, owned_obj, owned_exe)
    owned_run = run([str(owned_exe)])
    expect(
        owned_run.returncode == 11,
        f"expected owned object capture runtime positive fixture to exit 11, saw {owned_run.returncode}",
    )
    expect(
        owned_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and owned_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected owned runtime positive fixture to require copy/dispose helpers",
    )

    nonowning_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "nonowning_object_capture_runtime_positive.objc3"
    )
    nonowning_obj, _, nonowning_manifest_path = compile_fixture_outputs(
        nonowning_fixture, case_dir / "nonowning-runtime-positive"
    )
    nonowning_manifest = json.loads(
        nonowning_manifest_path.read_text(encoding="utf-8")
    )
    nonowning_copy_dispose_surface = (
        nonowning_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )
    nonowning_exe = (
        case_dir / "nonowning-runtime-positive" / "nonowning_runtime.exe"
    )
    link_fixture_executable(clangxx, nonowning_obj, nonowning_exe)
    nonowning_run = run([str(nonowning_exe)])
    expect(
        nonowning_run.returncode == 9,
        f"expected non-owning object capture runtime positive fixture to exit 9, saw {nonowning_run.returncode}",
    )
    expect(
        nonowning_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and nonowning_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected non-owning runtime positive fixture to elide copy/dispose helpers",
    )

    arc_mode_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_mode_handling_positive.objc3"
    )
    arc_mode_obj, _, arc_mode_manifest_path = compile_fixture_outputs_with_args(
        arc_mode_fixture,
        case_dir / "arc-mode-runtime-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_mode_manifest = json.loads(arc_mode_manifest_path.read_text(encoding="utf-8"))
    arc_mode_sema = arc_mode_manifest.get("frontend", {}).get("pipeline", {}).get(
        "sema_pass_manager", {}
    )
    arc_mode_exe = case_dir / "arc-mode-runtime-positive" / "arc_mode.exe"
    link_fixture_executable(clangxx, arc_mode_obj, arc_mode_exe)
    arc_mode_run = run([str(arc_mode_exe)])
    expect(
        arc_mode_run.returncode == 17,
        f"expected ARC mode runtime positive fixture to exit 17, saw {arc_mode_run.returncode}",
    )
    expect(
        arc_mode_sema.get("retain_release_operation_lowering_retain_insertion_sites")
        == 8
        and arc_mode_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8,
        "expected ARC mode runtime positive fixture to preserve eight retain and eight release insertions",
    )

    arc_inference_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_inference_lifetime_positive.objc3"
    )
    arc_inference_obj, _, arc_inference_manifest_path = (
        compile_fixture_outputs_with_args(
            arc_inference_fixture,
            case_dir / "arc-inference-runtime-positive",
            extra_args=["-fobjc-arc"],
        )
    )
    arc_inference_manifest = json.loads(
        arc_inference_manifest_path.read_text(encoding="utf-8")
    )
    arc_inference_sema = arc_inference_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})
    arc_inference_exe = (
        case_dir / "arc-inference-runtime-positive" / "arc_inference.exe"
    )
    link_fixture_executable(clangxx, arc_inference_obj, arc_inference_exe)
    arc_inference_run = run([str(arc_inference_exe)])
    expect(
        arc_inference_run.returncode == 17,
        f"expected ARC inference runtime positive fixture to exit 17, saw {arc_inference_run.returncode}",
    )
    expect(
        arc_inference_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 8
        and arc_inference_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8,
        "expected ARC inference runtime positive fixture to preserve eight retain and eight release insertions",
    )

    arc_cleanup_scope_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_cleanup_scope_positive.objc3"
    )
    arc_cleanup_scope_obj, _, arc_cleanup_scope_manifest_path = (
        compile_fixture_outputs_with_args(
            arc_cleanup_scope_fixture,
            case_dir / "arc-cleanup-scope-runtime-positive",
            extra_args=["-fobjc-arc"],
        )
    )
    arc_cleanup_scope_manifest = json.loads(
        arc_cleanup_scope_manifest_path.read_text(encoding="utf-8")
    )
    arc_cleanup_scope_sema = arc_cleanup_scope_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})
    arc_cleanup_scope_exe = (
        case_dir / "arc-cleanup-scope-runtime-positive" / "arc_cleanup_scope.exe"
    )
    link_fixture_executable(clangxx, arc_cleanup_scope_obj, arc_cleanup_scope_exe)
    arc_cleanup_scope_run = run([str(arc_cleanup_scope_exe)])
    expect(
        arc_cleanup_scope_run.returncode == 9,
        f"expected ARC cleanup scope runtime positive fixture to exit 9, saw {arc_cleanup_scope_run.returncode}",
    )
    expect(
        arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1,
        "expected ARC cleanup scope runtime positive fixture to preserve one retain/release cleanup pair",
    )

    arc_implicit_cleanup_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_implicit_cleanup_void_positive.objc3"
    )
    arc_implicit_cleanup_obj, _, arc_implicit_cleanup_manifest_path = (
        compile_fixture_outputs_with_args(
            arc_implicit_cleanup_fixture,
            case_dir / "arc-implicit-cleanup-runtime-positive",
            extra_args=["-fobjc-arc"],
        )
    )
    arc_implicit_cleanup_manifest = json.loads(
        arc_implicit_cleanup_manifest_path.read_text(encoding="utf-8")
    )
    arc_implicit_cleanup_sema = arc_implicit_cleanup_manifest.get(
        "frontend", {}
    ).get("pipeline", {}).get("sema_pass_manager", {})
    arc_implicit_cleanup_exe = (
        case_dir
        / "arc-implicit-cleanup-runtime-positive"
        / "arc_implicit_cleanup.exe"
    )
    link_fixture_executable(
        clangxx, arc_implicit_cleanup_obj, arc_implicit_cleanup_exe
    )
    arc_implicit_cleanup_run = run([str(arc_implicit_cleanup_exe)])
    expect(
        arc_implicit_cleanup_run.returncode == 0,
        f"expected ARC implicit cleanup runtime positive fixture to exit 0, saw {arc_implicit_cleanup_run.returncode}",
    )
    expect(
        arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1,
        "expected ARC implicit cleanup runtime positive fixture to preserve one retain/release cleanup pair",
    )

    return CaseResult(
        case_id="block-helper-runtime-execution",
        probe="linked-fixture-main",
        fixture="tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "byref_runtime_exit_code": byref_run.returncode,
            "owned_runtime_exit_code": owned_run.returncode,
            "nonowning_runtime_exit_code": nonowning_run.returncode,
            "arc_mode_runtime_exit_code": arc_mode_run.returncode,
            "arc_inference_runtime_exit_code": arc_inference_run.returncode,
            "arc_cleanup_scope_runtime_exit_code": arc_cleanup_scope_run.returncode,
            "arc_implicit_cleanup_runtime_exit_code": arc_implicit_cleanup_run.returncode,
            "byref_forwarding_probe_handle": byref_forwarding_payload.get("handle"),
            "byref_forwarding_first_invoke_result": byref_forwarding_payload.get(
                "first_invoke_result"
            ),
            "byref_forwarding_second_invoke_result": byref_forwarding_payload.get(
                "second_invoke_result"
            ),
            "byref_copy_helper_required_sites": byref_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "owned_copy_helper_required_sites": owned_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "nonowning_copy_helper_required_sites": nonowning_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "arc_mode_retain_insertions": arc_mode_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_inference_retain_insertions": arc_inference_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_cleanup_scope_release_insertions": arc_cleanup_scope_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_implicit_cleanup_release_insertions": arc_implicit_cleanup_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
        },
    )
