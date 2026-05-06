"""Compiler artifact runtime acceptance domain."""

from __future__ import annotations

import subprocess
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    ACCEPTANCE_ARTIFACT_REGISTRY,
    DIRECT_COMPILE_BACKEND,
    ROOT,
    run_fixture_compile,
)
from objc3c_runtime_acceptance.progress import repo_display_path


def check_artifact_registry_key_isolation_case(run_dir: Path) -> CaseResult:
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
    case_dir = run_dir / "artifact-registry-key-isolation"
    args_a = ["--objc3-bootstrap-registration-order-ordinal", "21"]
    args_b = ["--objc3-bootstrap-registration-order-ordinal", "22"]
    reuse_before = len(ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events)
    miss_before = len(ACCEPTANCE_ARTIFACT_REGISTRY.miss_events)

    def compile_direct(
        out_dir: Path, extra_args: list[str]
    ) -> subprocess.CompletedProcess[str]:
        result, selected_backend = run_fixture_compile(
            fixture,
            out_dir,
            extra_args=extra_args,
            backend=DIRECT_COMPILE_BACKEND,
            reuse_policy="immutable-inspection",
        )
        if result.returncode != 0:
            raise RuntimeError(
                "artifact registry key isolation compile failed for "
                f"{fixture}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
            )
        expect(
            selected_backend == DIRECT_COMPILE_BACKEND,
            "expected artifact registry key isolation to use direct-native backend",
        )
        ACCEPTANCE_ARTIFACT_REGISTRY.validate_artifacts(out_dir, "module")
        return result

    first_result = compile_direct(case_dir / "ordinal-21-producer", args_a)
    second_result = compile_direct(case_dir / "ordinal-22-distinct-args", args_b)
    reuse_after_distinct_args = len(ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events)
    expect(
        reuse_after_distinct_args == reuse_before,
        "artifact registry reused stale outputs after bootstrap ordinal changed",
    )
    third_result = compile_direct(case_dir / "ordinal-21-consumer", args_a)
    reuse_after_same_args = len(ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events)
    expect(
        reuse_after_same_args == reuse_before + 1,
        "artifact registry did not reuse immutable outputs when the key was unchanged",
    )
    reuse_event = ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events[-1]
    expect(
        reuse_event.get("producer_dir")
        == repo_display_path(case_dir / "ordinal-21-producer")
        and reuse_event.get("consumer_dir")
        == repo_display_path(case_dir / "ordinal-21-consumer"),
        "artifact registry reused from an unexpected producer or consumer directory",
    )

    key_a, key_payload_a = ACCEPTANCE_ARTIFACT_REGISTRY.cache_key(
        fixture,
        extra_args=args_a,
        backend=DIRECT_COMPILE_BACKEND,
        emit_prefix="module",
    )
    key_b, key_payload_b = ACCEPTANCE_ARTIFACT_REGISTRY.cache_key(
        fixture,
        extra_args=args_b,
        backend=DIRECT_COMPILE_BACKEND,
        emit_prefix="module",
    )
    import_surface_a = repo_display_path(
        case_dir / "surface-a.runtime-import-surface.json"
    )
    import_surface_b = repo_display_path(
        case_dir / "surface-b.runtime-import-surface.json"
    )
    key_import_a, key_payload_import_a = ACCEPTANCE_ARTIFACT_REGISTRY.cache_key(
        fixture,
        extra_args=["--objc3-import-runtime-surface", import_surface_a],
        backend=DIRECT_COMPILE_BACKEND,
        emit_prefix="module",
    )
    key_import_b, key_payload_import_b = ACCEPTANCE_ARTIFACT_REGISTRY.cache_key(
        fixture,
        extra_args=["--objc3-import-runtime-surface", import_surface_b],
        backend=DIRECT_COMPILE_BACKEND,
        emit_prefix="module",
    )
    expect(
        key_a != key_b and key_import_a != key_import_b,
        "artifact registry key did not distinguish changed args or changed import surfaces",
    )

    return CaseResult(
        case_id="artifact-registry-key-isolation",
        probe="direct-native-artifact-registry-negative-reuse-proof",
        fixture=repo_display_path(fixture),
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "producer_returncode": first_result.returncode,
            "distinct_args_returncode": second_result.returncode,
            "same_args_reuse_returncode": third_result.returncode,
            "reuse_events_before": reuse_before,
            "reuse_events_after_distinct_args": reuse_after_distinct_args,
            "reuse_events_after_same_args": reuse_after_same_args,
            "miss_events_added": len(ACCEPTANCE_ARTIFACT_REGISTRY.miss_events)
            - miss_before,
            "changed_args_key_a": key_a,
            "changed_args_key_b": key_b,
            "changed_args_payload_a": key_payload_a,
            "changed_args_payload_b": key_payload_b,
            "changed_import_surface_key_a": key_import_a,
            "changed_import_surface_key_b": key_import_b,
            "changed_import_surface_payload_a": key_payload_import_a,
            "changed_import_surface_payload_b": key_payload_import_b,
            "negative_reuse_model": (
                "changed bootstrap args and changed import-surface paths produce "
                "different immutable artifact registry keys; only exact key "
                "matches may copy producer artifacts"
            ),
        },
    )
