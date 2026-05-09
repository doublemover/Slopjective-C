"""Compiler artifact runtime acceptance domain."""

from __future__ import annotations

import json
import shutil
import subprocess
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.compile_backends import DEFAULT_COMPILE_BACKEND
from objc3c_runtime_acceptance.compile_backends import DIRECT_COMPILE_BACKEND
from objc3c_runtime_acceptance.compile_backends import WRAPPER_COMPILE_BACKEND
from objc3c_runtime_acceptance.fixture_compilation import run_fixture_compile
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.progress_format import repo_display_path
from objc3c_runtime_acceptance.runtime_artifact_registry import ACCEPTANCE_ARTIFACT_REGISTRY


def check_compile_backend_parity_case(run_dir: Path) -> CaseResult:
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "synthesized_accessor_property_lowering_positive.objc3"
    )
    case_dir = run_dir / "compile-backend-parity"
    direct_dir = case_dir / "direct"
    wrapper_dir = case_dir / "wrapper"
    parity_cache_root = case_dir / "metaprogramming-cache-root"
    parity_args = [
        "--objc3-metaprogramming-cache-root",
        repo_display_path(parity_cache_root),
    ]
    direct_result, direct_backend = run_fixture_compile(
        fixture,
        direct_dir,
        backend=DIRECT_COMPILE_BACKEND,
        extra_args=parity_args,
    )
    if direct_result.returncode != 0:
        raise RuntimeError(
            "direct compile backend failed during parity check:\nSTDOUT:\n"
            + direct_result.stdout
            + "\nSTDERR:\n"
            + direct_result.stderr
        )
    shutil.rmtree(parity_cache_root, ignore_errors=True)
    wrapper_result, wrapper_backend = run_fixture_compile(
        fixture,
        wrapper_dir,
        backend=WRAPPER_COMPILE_BACKEND,
        extra_args=parity_args,
    )
    if wrapper_result.returncode != 0:
        raise RuntimeError(
            "wrapper compile backend failed during parity check:\nSTDOUT:\n"
            + wrapper_result.stdout
            + "\nSTDERR:\n"
            + wrapper_result.stderr
        )

    direct_provenance = json.loads(
        (direct_dir / "module.compile-provenance.json").read_text(encoding="utf-8")
    )
    wrapper_provenance = json.loads(
        (wrapper_dir / "module.compile-provenance.json").read_text(encoding="utf-8")
    )
    direct_truthfulness = direct_provenance.get("compile_output_truthfulness", {})
    wrapper_truthfulness = wrapper_provenance.get("compile_output_truthfulness", {})
    compared_truthfulness_fields = [
        "runtime_dispatch_symbol",
        "runtime_dispatch_declaration_count",
        "runtime_dispatch_call_count",
        "property_descriptor_count_expected",
        "property_descriptor_definition_count",
        "property_descriptor_section_present",
        "ivar_descriptor_count_expected",
        "ivar_descriptor_definition_count",
        "ivar_descriptor_section_present",
        "property_synthesis_sites_expected",
        "synthesized_accessor_definition_count",
        "current_property_helper_call_count",
        "property_descriptor_counts_match",
        "ivar_descriptor_counts_match",
        "synthesized_property_surface_matches",
        "truthful",
    ]
    for field in compared_truthfulness_fields:
        if direct_truthfulness.get(field) != wrapper_truthfulness.get(field):
            raise RuntimeError(
                "direct compile backend truthfulness drifted from wrapper for "
                f"{field}: direct={direct_truthfulness.get(field)!r} "
                f"wrapper={wrapper_truthfulness.get(field)!r}"
            )
    if (
        direct_provenance.get("artifact_set_digest_sha256")
        != wrapper_provenance.get("artifact_set_digest_sha256")
    ):
        raise RuntimeError(
            "direct compile backend artifact digest drifted from wrapper output"
        )
    if direct_provenance.get("compile_backend") != DIRECT_COMPILE_BACKEND:
        raise RuntimeError("direct compile backend did not stamp direct provenance")

    return CaseResult(
        case_id="compile-backend-parity",
        probe="direct-native-compile-plus-wrapper-contract-parity",
        fixture=repo_display_path(fixture),
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "direct_backend": direct_backend,
            "wrapper_backend": wrapper_backend,
            "default_backend": DEFAULT_COMPILE_BACKEND,
            "artifact_set_digest_sha256": direct_provenance.get(
                "artifact_set_digest_sha256"
            ),
            "truthfulness_fields_compared": compared_truthfulness_fields,
            "provenance_contract_id": direct_provenance.get("contract_id"),
            "compile_output_truthfulness_contract_id": direct_truthfulness.get(
                "contract_id"
            ),
        },
    )


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


__all__ = [
    "check_artifact_registry_key_isolation_case",
    "check_compile_backend_parity_case",
]
