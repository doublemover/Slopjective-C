"""Compiler artifact acceptance case orchestration."""

from __future__ import annotations

import shutil
import subprocess
from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.compile_backends import DIRECT_COMPILE_BACKEND
from objc3c_runtime_acceptance.compile_backends import WRAPPER_COMPILE_BACKEND
from objc3c_runtime_acceptance.fixture_compilation import run_fixture_compile
from objc3c_runtime_acceptance.progress_format import repo_display_path
from objc3c_runtime_acceptance.runtime_artifact_registry import (
    ACCEPTANCE_ARTIFACT_REGISTRY,
)

from .assertions import expect_artifact_digest_parity
from .assertions import expect_artifact_registry_compile_success
from .assertions import expect_backend_compile_success
from .assertions import expect_compile_truthfulness_parity
from .assertions import expect_direct_artifact_registry_backend
from .assertions import expect_direct_compile_backend_provenance
from .assertions import expect_distinct_artifact_cache_keys
from .assertions import expect_no_distinct_args_reuse
from .assertions import expect_reuse_event_paths
from .assertions import expect_same_args_reuse
from .catalog import ARTIFACT_REGISTRY_IMPORT_SURFACE_A_NAME
from .catalog import ARTIFACT_REGISTRY_IMPORT_SURFACE_B_NAME
from .catalog import ARTIFACT_REGISTRY_KEY_ISOLATION_CASE_ID
from .catalog import ARTIFACT_REGISTRY_KEY_ISOLATION_FIXTURE
from .catalog import ARTIFACT_REGISTRY_REUSE_POLICY
from .catalog import ARTIFACT_REGISTRY_SHARED_EMIT_PREFIX
from .catalog import COMPILE_BACKEND_PARITY_CACHE_ROOT_NAME
from .catalog import COMPILE_BACKEND_PARITY_CASE_ID
from .catalog import COMPILE_BACKEND_PARITY_DIRECT_DIR_NAME
from .catalog import COMPILE_BACKEND_PARITY_FIXTURE
from .catalog import COMPILE_BACKEND_PARITY_TRUTHFULNESS_FIELDS
from .catalog import COMPILE_BACKEND_PARITY_WRAPPER_DIR_NAME
from .catalog import bootstrap_ordinal_args
from .data import ArtifactRegistryCacheEvidence
from .data import ArtifactRegistryCaseEvidence
from .data import CompileBackendParityEvidence
from .payloads import artifact_registry_key_isolation_case_result
from .payloads import compile_backend_parity_case_result
from .predicates import artifact_registry_cache_key
from .predicates import read_json_object
from .predicates import registry_event_snapshot


def check_compile_backend_parity_case(run_dir: Path) -> CaseResult:
    fixture = COMPILE_BACKEND_PARITY_FIXTURE
    case_dir = run_dir / COMPILE_BACKEND_PARITY_CASE_ID
    direct_dir = case_dir / COMPILE_BACKEND_PARITY_DIRECT_DIR_NAME
    wrapper_dir = case_dir / COMPILE_BACKEND_PARITY_WRAPPER_DIR_NAME
    parity_cache_root = case_dir / COMPILE_BACKEND_PARITY_CACHE_ROOT_NAME
    parity_args = [
        "--objc3-metaprogramming-cache-root",
        repo_display_path(parity_cache_root),
    ]

    direct_result, direct_backend = _compile_parity_fixture(
        fixture,
        direct_dir,
        backend=DIRECT_COMPILE_BACKEND,
        extra_args=parity_args,
        backend_label="direct",
    )
    shutil.rmtree(parity_cache_root, ignore_errors=True)
    wrapper_result, wrapper_backend = _compile_parity_fixture(
        fixture,
        wrapper_dir,
        backend=WRAPPER_COMPILE_BACKEND,
        extra_args=parity_args,
        backend_label="wrapper",
    )

    direct_provenance = read_json_object(
        direct_dir / "module.compile-provenance.json"
    )
    wrapper_provenance = read_json_object(
        wrapper_dir / "module.compile-provenance.json"
    )
    direct_truthfulness = direct_provenance.get("compile_output_truthfulness", {})
    wrapper_truthfulness = wrapper_provenance.get("compile_output_truthfulness", {})
    compared_truthfulness_fields = list(COMPILE_BACKEND_PARITY_TRUTHFULNESS_FIELDS)
    expect_compile_truthfulness_parity(
        direct_truthfulness,
        wrapper_truthfulness,
        compared_truthfulness_fields,
    )
    expect_artifact_digest_parity(direct_provenance, wrapper_provenance)
    expect_direct_compile_backend_provenance(direct_provenance)

    return compile_backend_parity_case_result(
        CompileBackendParityEvidence(
            fixture=fixture,
            direct_backend=direct_backend,
            wrapper_backend=wrapper_backend,
            direct_provenance=direct_provenance,
            direct_truthfulness=direct_truthfulness,
            compared_truthfulness_fields=compared_truthfulness_fields,
        )
    )


def check_artifact_registry_key_isolation_case(run_dir: Path) -> CaseResult:
    fixture = ARTIFACT_REGISTRY_KEY_ISOLATION_FIXTURE
    case_dir = run_dir / ARTIFACT_REGISTRY_KEY_ISOLATION_CASE_ID
    args_a = bootstrap_ordinal_args("21")
    args_b = bootstrap_ordinal_args("22")
    event_snapshot = registry_event_snapshot()

    first_result = _compile_direct_for_registry(
        fixture,
        case_dir / "ordinal-21-producer",
        args_a,
    )
    second_result = _compile_direct_for_registry(
        fixture,
        case_dir / "ordinal-22-distinct-args",
        args_b,
    )
    reuse_after_distinct_args = len(ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events)
    expect_no_distinct_args_reuse(
        event_snapshot.reuse_events,
        reuse_after_distinct_args,
    )
    third_result = _compile_direct_for_registry(
        fixture,
        case_dir / "ordinal-21-consumer",
        args_a,
    )
    reuse_after_same_args = len(ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events)
    expect_same_args_reuse(event_snapshot.reuse_events, reuse_after_same_args)
    reuse_event = ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events[-1]
    expect_reuse_event_paths(
        reuse_event,
        producer_dir=case_dir / "ordinal-21-producer",
        consumer_dir=case_dir / "ordinal-21-consumer",
    )

    cache_evidence = _artifact_registry_cache_evidence(
        fixture,
        case_dir,
        args_a,
        args_b,
    )
    expect_distinct_artifact_cache_keys(
        cache_evidence.changed_args_key_a,
        cache_evidence.changed_args_key_b,
        cache_evidence.changed_import_surface_key_a,
        cache_evidence.changed_import_surface_key_b,
    )

    return artifact_registry_key_isolation_case_result(
        ArtifactRegistryCaseEvidence(
            fixture=fixture,
            first_result=first_result,
            second_result=second_result,
            third_result=third_result,
            reuse_events_before=event_snapshot.reuse_events,
            miss_events_before=event_snapshot.miss_events,
            reuse_events_after_distinct_args=reuse_after_distinct_args,
            reuse_events_after_same_args=reuse_after_same_args,
            miss_events_after=len(ACCEPTANCE_ARTIFACT_REGISTRY.miss_events),
            cache=cache_evidence,
        )
    )


def _compile_parity_fixture(
    fixture: Path,
    out_dir: Path,
    *,
    backend: str,
    extra_args: list[str],
    backend_label: str,
) -> tuple[subprocess.CompletedProcess[str], str]:
    result, selected_backend = run_fixture_compile(
        fixture,
        out_dir,
        backend=backend,
        extra_args=extra_args,
    )
    expect_backend_compile_success(backend_label, result)
    return result, selected_backend


def _compile_direct_for_registry(
    fixture: Path,
    out_dir: Path,
    extra_args: list[str],
) -> subprocess.CompletedProcess[str]:
    result, selected_backend = run_fixture_compile(
        fixture,
        out_dir,
        extra_args=extra_args,
        backend=DIRECT_COMPILE_BACKEND,
        reuse_policy=ARTIFACT_REGISTRY_REUSE_POLICY,
    )
    expect_artifact_registry_compile_success(fixture, result)
    expect_direct_artifact_registry_backend(selected_backend)
    ACCEPTANCE_ARTIFACT_REGISTRY.validate_artifacts(
        out_dir,
        ARTIFACT_REGISTRY_SHARED_EMIT_PREFIX,
    )
    return result


def _artifact_registry_cache_evidence(
    fixture: Path,
    case_dir: Path,
    args_a: list[str],
    args_b: list[str],
) -> ArtifactRegistryCacheEvidence:
    key_a, key_payload_a = artifact_registry_cache_key(fixture, extra_args=args_a)
    key_b, key_payload_b = artifact_registry_cache_key(fixture, extra_args=args_b)
    import_surface_a = repo_display_path(
        case_dir / ARTIFACT_REGISTRY_IMPORT_SURFACE_A_NAME
    )
    import_surface_b = repo_display_path(
        case_dir / ARTIFACT_REGISTRY_IMPORT_SURFACE_B_NAME
    )
    key_import_a, key_payload_import_a = artifact_registry_cache_key(
        fixture,
        extra_args=["--objc3-import-runtime-surface", import_surface_a],
    )
    key_import_b, key_payload_import_b = artifact_registry_cache_key(
        fixture,
        extra_args=["--objc3-import-runtime-surface", import_surface_b],
    )
    return ArtifactRegistryCacheEvidence(
        changed_args_key_a=key_a,
        changed_args_key_b=key_b,
        changed_args_payload_a=key_payload_a,
        changed_args_payload_b=key_payload_b,
        changed_import_surface_key_a=key_import_a,
        changed_import_surface_key_b=key_import_b,
        changed_import_surface_payload_a=key_payload_import_a,
        changed_import_surface_payload_b=key_payload_import_b,
    )
