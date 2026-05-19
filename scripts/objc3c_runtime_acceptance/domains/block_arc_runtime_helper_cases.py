"""Block helper and ARC linked-runtime acceptance case."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.domains.block_arc_runtime_arc_assertions import (
    assert_arc_cleanup_scope_runtime_fixture,
    assert_arc_implicit_cleanup_runtime_fixture,
    assert_arc_inference_runtime_fixture,
    assert_arc_mode_runtime_fixture,
)
from objc3c_runtime_acceptance.domains.block_arc_runtime_block_assertions import (
    assert_byref_forwarding_probe_payload,
    assert_byref_runtime_fixture,
    assert_nonowning_runtime_fixture,
    assert_owned_runtime_fixture,
    byref_forwarding_probe_summary,
)
from objc3c_runtime_acceptance.domains.block_arc_runtime_shared import (
    block_copy_dispose_surface,
    compile_link_run_fixture,
    compile_run_json_probe,
    native_fixture,
    runtime_probe,
    sema_pass_manager,
)


def check_block_helper_runtime_execution_case(
    clangxx: str,
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "block-helper-runtime-execution"

    byref = compile_link_run_fixture(
        clangxx,
        native_fixture("byref_cell_copy_dispose_runtime_positive.objc3"),
        case_dir / "byref-runtime-positive",
        "byref_runtime.exe",
    )
    byref_copy_dispose_surface = block_copy_dispose_surface(byref.manifest)
    assert_byref_runtime_fixture(byref.returncode, byref_copy_dispose_surface)

    byref_forwarding_payload = compile_run_json_probe(
        clangxx,
        runtime_probe("block_runtime_byref_forwarding_probe.cpp"),
        case_dir / "block_runtime_byref_forwarding_probe.exe",
        "block runtime byref forwarding probe",
    )
    assert_byref_forwarding_probe_payload(byref_forwarding_payload)

    copy_dispose_payload = compile_run_json_probe(
        clangxx,
        runtime_probe("block_runtime_copy_dispose_invoke_probe.cpp"),
        case_dir / "block_runtime_copy_dispose_invoke_probe.exe",
        "block runtime copy/dispose invoke probe",
    )
    expect(
        copy_dispose_payload.get("copy_count_after_promotion") == 1
        and copy_dispose_payload.get("invoke_result") == 117
        and copy_dispose_payload.get("dispose_count_before_final_release") == 0
        and copy_dispose_payload.get("dispose_count_after_final_release") == 1
        and copy_dispose_payload.get("post_release_callback_count") == 0
        and copy_dispose_payload.get("invoke_after_release_result") == 0,
        "expected block runtime copy/dispose invoke probe to preserve promoted pointer capture lifetime and reject stale post-release invocation",
    )

    owned = compile_link_run_fixture(
        clangxx,
        native_fixture("owned_object_capture_runtime_positive.objc3"),
        case_dir / "owned-runtime-positive",
        "owned_runtime.exe",
    )
    owned_copy_dispose_surface = block_copy_dispose_surface(owned.manifest)
    assert_owned_runtime_fixture(owned.returncode, owned_copy_dispose_surface)

    nonowning = compile_link_run_fixture(
        clangxx,
        native_fixture("nonowning_object_capture_runtime_positive.objc3"),
        case_dir / "nonowning-runtime-positive",
        "nonowning_runtime.exe",
    )
    nonowning_copy_dispose_surface = block_copy_dispose_surface(nonowning.manifest)
    assert_nonowning_runtime_fixture(
        nonowning.returncode,
        nonowning_copy_dispose_surface,
    )

    arc_mode = compile_link_run_fixture(
        clangxx,
        native_fixture("arc_mode_handling_positive.objc3"),
        case_dir / "arc-mode-runtime-positive",
        "arc_mode.exe",
        extra_args=["-fobjc-arc"],
    )
    arc_mode_sema = sema_pass_manager(arc_mode.manifest)
    assert_arc_mode_runtime_fixture(arc_mode.returncode, arc_mode_sema)

    arc_inference = compile_link_run_fixture(
        clangxx,
        native_fixture("arc_inference_lifetime_positive.objc3"),
        case_dir / "arc-inference-runtime-positive",
        "arc_inference.exe",
        extra_args=["-fobjc-arc"],
    )
    arc_inference_sema = sema_pass_manager(arc_inference.manifest)
    assert_arc_inference_runtime_fixture(
        arc_inference.returncode,
        arc_inference_sema,
    )

    arc_cleanup_scope = compile_link_run_fixture(
        clangxx,
        native_fixture("arc_cleanup_scope_positive.objc3"),
        case_dir / "arc-cleanup-scope-runtime-positive",
        "arc_cleanup_scope.exe",
        extra_args=["-fobjc-arc"],
    )
    arc_cleanup_scope_sema = sema_pass_manager(arc_cleanup_scope.manifest)
    assert_arc_cleanup_scope_runtime_fixture(
        arc_cleanup_scope.returncode,
        arc_cleanup_scope_sema,
    )

    arc_implicit_cleanup = compile_link_run_fixture(
        clangxx,
        native_fixture("arc_implicit_cleanup_void_positive.objc3"),
        case_dir / "arc-implicit-cleanup-runtime-positive",
        "arc_implicit_cleanup.exe",
        extra_args=["-fobjc-arc"],
    )
    arc_implicit_cleanup_sema = sema_pass_manager(arc_implicit_cleanup.manifest)
    assert_arc_implicit_cleanup_runtime_fixture(
        arc_implicit_cleanup.returncode,
        arc_implicit_cleanup_sema,
    )

    return CaseResult(
        case_id="block-helper-runtime-execution",
        probe="linked-fixture-main",
        fixture="tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "byref_runtime_exit_code": byref.returncode,
            "owned_runtime_exit_code": owned.returncode,
            "nonowning_runtime_exit_code": nonowning.returncode,
            "arc_mode_runtime_exit_code": arc_mode.returncode,
            "arc_inference_runtime_exit_code": arc_inference.returncode,
            "arc_cleanup_scope_runtime_exit_code": arc_cleanup_scope.returncode,
            "arc_implicit_cleanup_runtime_exit_code": arc_implicit_cleanup.returncode,
            **byref_forwarding_probe_summary(byref_forwarding_payload),
            "copy_dispose_probe_copy_count_after_promotion": copy_dispose_payload.get(
                "copy_count_after_promotion"
            ),
            "copy_dispose_probe_invoke_result": copy_dispose_payload.get(
                "invoke_result"
            ),
            "copy_dispose_probe_dispose_count_after_final_release": (
                copy_dispose_payload.get("dispose_count_after_final_release")
            ),
            "copy_dispose_probe_post_release_callback_count": (
                copy_dispose_payload.get("post_release_callback_count")
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


__all__ = ["check_block_helper_runtime_execution_case"]
